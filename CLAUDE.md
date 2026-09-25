# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Language

Communicate with the user in Russian.

Write all new code comments in English (existing Spanish comments can stay as they are).

## Git

**Never push to any remote** (`git push` in any form, including `--force`, tags, or pushing via `gh`). The user pushes personally. Committing locally is fine when asked.

## Project

Arena Tracker (AT) is a Qt 6 / C++ desktop deck tracker for Hearthstone, focused on Arena drafting. Single qmake project, no test suite, no linter. Code comments are frequently in Spanish.

## Build

Dependencies: Qt (modules `core gui network widgets websockets`), OpenCV (found via pkg-config as `opencv`), libzip, zlib. The README targets OpenCV 2.4.x.

```sh
qmake ArenaTracker.pro && make        # or open ArenaTracker.pro in Qt Creator, Release build
```

- Every new `.cpp`/`.h` must be added to `SOURCES`/`HEADERS` in `ArenaTracker.pro`.
- `Sources/Utils/capturemanager.*` is only compiled on Linux (Wayland screen capture via the external `Extra/captureHelper` binary).
- Resources (images, fonts) are bundled through `arenatracker.qrc`; the main UI layout lives in `mainwindow.ui`.
- App version is `VERSION` in `Sources/versionchecker.h`.

Debug toggles (compile-time) are the `DEBUG_*` defines in `Sources/utility.h`.

## Architecture

**Wiring hub:** `MainWindow` (`Sources/mainwindow.cpp`, ~5k lines) creates every handler in `create*Handler()` methods and connects them with old-style `SIGNAL()/SLOT()` string connections. Handlers are mostly decoupled from each other; cross-handler data flow goes through signals connected in `MainWindow`. When adding a signal, remember to wire it there (and handlers commonly re-emit `pDebug`, `startProgressBar`, etc. to MainWindow).

**Game-state pipeline (Hearthstone log parsing):**
1. `LogLoader` finds the HS logs dir and creates one `LogWorker` per component (`LoadingScreen`, `Power`, `Zone`, `Arena`, `Asset`), each tailing its log file.
2. Lines are optionally merged/sorted across components by timestamp (`sortLogs`) and emitted as `newLogLineRead(LogComponent, line, ...)`.
3. `GameWatcher` parses lines with regexes (`processPower`, `processZone`, `processArena`, ...) and emits high-level signals (`newArena`, `startGame`, `playerCardDraw`, `enemySecretPlayed`, `playerMinionZonePlayAdd`, ...).
4. Feature handlers consume those signals: `DeckHandler`, `EnemyHandHandler`, `EnemyDeckHandler`, `SecretsHandler`, `PlanHandler` (board replay), `ArenaHandler` (run results/winrates), `GraveyardHandler`, `DrawCardHandler`, `RngCardHandler`, `PopularCardsHandler`.

**Drafting:** `DraftHandler` (largest file) screen-captures the draft, locates card slots via template images in `Extra/*Template*.png`, and identifies cards by OpenCV histogram comparison against downloaded card images. It scores picks from several sources (`DraftMethod`/`ScoreSource` enums: HearthArena tier list, Firestone, HSReplay via `WinratesDownloader`) and owns `SynergyHandler`, which uses the counters in `Sources/Synergies/` driven by per-card synergy tags.

**Premium gating:** `PremiumHandler` emits `setPremium(bool)` to most handlers; premium status is tied to the Track-o-Bot account (`TrackobotUploader`) checked against `Premium/premium.json`.

**Card model:** Card data comes from HearthstoneJSON `cards.json` (cached); card metadata lookups are static helpers in `Utility`. Card-ID constants (secrets, special cards) are in `Sources/constants.h`. `Sources/Cards/` holds UI list-item card types (DeckCard base, with Draft/Hand/Secret/Synergy/etc. subclasses).

**Settings/storage:** `QSettings("Arena Tracker", "Arena Tracker")`; user data dir is `~/Arena Tracker` (Win/Mac) or `~/.local/share/Arena Tracker` (Linux).

## Repo data served to running clients

The installed app downloads data directly from this repo's `master` branch via `raw.githubusercontent.com` (URLs in `mainwindow.h`, `versionchecker.h`, `hscarddownloader.h`). Committing to these directories affects all live users immediately:

- `Version/version.json` — latest version + download URLs; `versionFree` lists versions allowed without premium.
- `Arena/arenaVersion.json` — current arena card sets, `trustHA`, reset counters.
- `Synergies/synergies.json` (card ID → list of synergy/mechanic tags; tags must be ones the `Synergies/` counters recognize) + `synergiesVersion.json`.
- `HearthArena/hearthArena.json` + `haVersion.json`, `LightForge/`, `CardsJson/`, `Themes/`, `Extra/`, `Images/`, `HearthstoneCards/`, `HearthstoneSignatureCards/`, `Premium/premium.json`.

Clients only re-download a JSON when its companion `*Version.json` number increases — bump the version number whenever the data file changes.


