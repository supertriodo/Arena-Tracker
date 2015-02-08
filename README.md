# Arena-Tracker
Arena Tracker reads the Hearthstone log to keep track of your arena games and rewards.
It connects to www.arenamastery.com and automatically upload them to your account.
Watch the remaining cards of your arena deck while you play.

## First Run
The first time you run Arena Tracker you will be asked for:

* 1) output_log.txt location.
 * Default (Win): C:\Program Files (x86)\Hearthstone\Hearthstone_Data\output_log.txt
* 2) log.config location.
 * Default (Win 8): USER\AppData\Local\Blizzard\Hearthstone\log.config
 * Default (Win XP): USER\Local Settings\Application Data\Blizzard\Hearthstone\log.config
 * If the file doesn't exist create an empty log.config in that dir.
* 3) Your Arena Mastery user/password.
 * If you don't have one. Go to www.arenamastery.com and create one. 
* After your first game:
* 4) Your Hearthstone name.
 * A pop up message will appear asking your name.

## Build from source (Linux, Windows & Mac)
* Download & install QT Community.
 * http://www.qt.io/download-open-source/
* Download or clone Arena Tracker source code.
* Open Qt Creator and open Arena Tracker project.
 * Open File or project... Look for ArenaTracker.pro
* In the botton left set the build to Release and Run.
* Move HSCards directory to the build dir.

If you find anything is missing or wrong please share it.  
http://www.reddit.com/r/ArenaTracker/
