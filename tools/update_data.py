#!/usr/bin/env python3
"""Refresh the data files that Arena Tracker downloads from this repo.

- CardsJson/cards.json        <- HearthstoneJSON (bumps cardsVersion.json when it changes)
- Arena/arenaVersion.json     <- arena sets derived from Firestone arena card stats
                                 (bumps arenaVersion when the set list changes)
- HearthstoneCards/<id>.png   <- HearthstoneJSON 256x renders, cropped/scaled to AT's 200x304 format,
                                 for arena-pool cards that have no image yet

Usage:  python3 tools/update_data.py [--dry-run] [--sets SET1,SET2,...]
Requires Pillow (pip install pillow).
"""

import argparse
import gzip
import io
import json
import sys
import urllib.request
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
HSJ_CARDS_URL = "https://api.hearthstonejson.com/v1/latest/all/cards.json"
HSJ_RENDER_URL = "https://art.hearthstonejson.com/v1/render/latest/enUS/256x/{}.png"
FIRE_GLOBAL_URL = "https://static.zerotoheroes.com/api/arena/stats/cards/arena-underground/last-patch/global.gz.json"
USER_AGENT = "ArenaTracker-data-updater (+https://github.com/Inoooooor/Arena-Tracker)"

# A set is in the arena pool when at least this fraction of its collectible cards shows up
# in Firestone's arena pick stats. Pool sets sit near 100%; discovered/generated cards from
# other sets only add a handful of entries per set.
ARENA_SET_MIN_COVERAGE = 0.5

# Geometry fitted against existing HearthstoneCards images: crop the 256x388 render at
# (5, -8) with width 245 (height keeps the 200:304 ratio), then scale to 200x304.
# Same idea as the commented-out "Old cut for all hearthsim plain cards" in hscarddownloader.cpp.
CROP_X, CROP_Y, CROP_W = 5, -8, 245
OUT_W, OUT_H = 200, 304


def fetch(url, timeout=120):
    req = urllib.request.Request(url, headers={"User-Agent": USER_AGENT, "Accept-Encoding": "gzip"})
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        data = resp.read()
        if resp.headers.get("Content-Encoding") == "gzip" or data[:2] == b"\x1f\x8b":
            data = gzip.decompress(data)
        return data


def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path, obj, dry_run):
    print(f"  write {path.relative_to(ROOT)}")
    if not dry_run:
        path.write_text(json.dumps(obj, indent=2) + "\n", encoding="utf-8")


def update_cards_json(dry_run):
    print("cards.json")
    data = fetch(HSJ_CARDS_URL)
    cards = json.loads(data)
    path = ROOT / "CardsJson" / "cards.json"
    if path.exists() and path.read_bytes() == data:
        print("  up to date")
        return cards

    old_ids = {c["id"] for c in read_json(path)} if path.exists() else set()
    new_ids = [c["id"] for c in cards if c["id"] not in old_ids]
    print(f"  {len(cards)} cards, {len(new_ids)} new")
    if not dry_run:
        path.write_bytes(data)
    version_path = ROOT / "CardsJson" / "cardsVersion.json"
    version = read_json(version_path)
    version["cardsVersion"] += 1
    write_json(version_path, version, dry_run)
    return cards


def arena_pool_codes(cards, sets):
    return sorted(c["id"] for c in cards
                  if c.get("set") in sets and c.get("collectible")
                  and not c["id"].startswith(("HERO_0", "HERO_1")))


def detect_arena_sets(cards):
    stats = json.loads(fetch(FIRE_GLOBAL_URL))
    picked = {s["cardId"] for s in stats["stats"]}
    collectible = {}
    for c in cards:
        if c.get("collectible") and c.get("set"):
            collectible.setdefault(c["set"], set()).add(c["id"])
    coverage = {s: len(ids & picked) / len(ids) for s, ids in collectible.items()}
    sets = [s for s, cov in coverage.items() if cov >= ARENA_SET_MIN_COVERAGE]
    print(f"  Firestone stats from {stats.get('lastUpdated')}: "
          + ", ".join(f"{s} {coverage[s]:.0%}" for s in sets))
    return sets


def update_arena_version(cards, forced_sets, dry_run):
    print("arenaVersion.json")
    path = ROOT / "Arena" / "arenaVersion.json"
    arena = read_json(path)
    sets = forced_sets or detect_arena_sets(cards)
    if not sets:
        sys.exit("  no arena sets detected, refusing to write an empty pool")

    # Keep the current order for sets that stay, append new ones (the app doesn't care, diffs stay small).
    ordered = [s for s in arena["arenaSets"] if s in sets] + sorted(s for s in sets if s not in arena["arenaSets"])
    if ordered == arena["arenaSets"]:
        print("  up to date")
        return ordered
    print(f"  {arena['arenaSets']} -> {ordered}")
    arena["arenaSets"] = ordered
    arena["arenaVersion"] += 1
    write_json(path, arena, dry_run)
    return ordered


def render_card(code):
    src = Image.open(io.BytesIO(fetch(HSJ_RENDER_URL.format(code)))).convert("RGBA")
    crop_h = round(CROP_W * OUT_H / OUT_W)
    canvas = Image.new("RGBA", (CROP_W, crop_h), (0, 0, 0, 0))
    canvas.paste(src, (-CROP_X, -CROP_Y))
    return canvas.resize((OUT_W, OUT_H), Image.LANCZOS)


def update_card_images(cards, sets, dry_run):
    print("HearthstoneCards")
    out_dir = ROOT / "HearthstoneCards"
    missing = [code for code in arena_pool_codes(cards, sets) if not (out_dir / f"{code}.png").exists()]
    print(f"  {len(missing)} arena cards without image")
    failed = []
    for code in missing:
        try:
            image = render_card(code)
        except Exception as e:
            print(f"  {code}: {e}")
            failed.append(code)
            continue
        print(f"  {code}")
        if dry_run:
            continue
        image.save(out_dir / f"{code}.png")
        # No golden renders on HearthstoneJSON: reuse the plain one so golden picks still get a histogram.
        premium = out_dir / f"{code}_premium.png"
        if not premium.exists():
            image.save(premium)
    if failed:
        print(f"  failed: {', '.join(failed)}")
    return failed


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--dry-run", action="store_true", help="show what would change without writing files")
    parser.add_argument("--sets", help="comma-separated arena sets, skips Firestone detection")
    args = parser.parse_args()

    cards = update_cards_json(args.dry_run)
    sets = update_arena_version(cards, args.sets.split(",") if args.sets else None, args.dry_run)
    failed = update_card_images(cards, sets, args.dry_run)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
