# Arena-Tracker
Arena Tracker reads the Hearthstone log to keep track of your arena deck, games and rewards.
* Draft with HearthArena advice.
* Upload your stats to your ArenaMastery account.
* Go over your deck while you play.
* Get value info of enemy's hand. Cards sent back from the board, spare parts or the coin (among others).
* Test enemy secrets. Watch the remaining posibilities while you make your moves.

My Hearthstone ID: triodo#2541

If you like the app, donations are much appreciated. It allows me to spend more time developing Arena Tracker.
[Donate](https://www.paypal.com/uk/cgi-bin/webscr?cmd=_flow&SESSION=Blywt-6y-rq9ASehsJn_7QfivQ7tk32oI5nMMXlmSL4dLXTkagolTWHLwIW&dispatch=5885d80a13c0db1f8e263663d3faee8de6030e9239419d79c3f52f70a3ed57ec)


## Download (Windows)
https://github.com/supertriodo/Arena-Tracker/releases/download/v1.41/Arena.Tracker.v1.41.zip


## Build from source (Linux, Mac & Windows)
* Download & install QT Community.
 * http://www.qt.io/download-open-source/
* Download or clone Arena Tracker source code.
 * https://github.com/supertriodo/Arena-Tracker/archive/master.zip
* Open Qt Creator and open Arena Tracker project.
 * Open File or project... Look for ArenaTracker.pro
* In the botton left set the build to Release and Run.
* Move HSCards directory to the build dir.


## First Run
The first time you run Arena Tracker you will be asked for:

* 1) output_log.txt location (If not default).
 * Default (Win): C:\Program Files (x86)\Hearthstone\Hearthstone_Data\output_log.txt
* 2) log.config location (If not default).
 * Default (Win 8): USER\AppData\Local\Blizzard\Hearthstone\log.config
 * Default (Win XP): USER\Local Settings\Application Data\Blizzard\Hearthstone\log.config
 * If the file doesn't exist create an empty log.config in that dir.
* 3) Your Arena Mastery user/password.
 * If you don't have one. Go to www.arenamastery.com and create one. 
* 4) Restart Hearthstone (if you have it opened).

After your first game:
* 5) Your Hearthstone name.
 * A pop up message will appear asking your name.

If you have problems installing it or using it I'm happy to help you on
http://www.reddit.com/r/ArenaTracker/
or on email: triodo@gmail.com


## FAQ
* How do I reset the app so I can set the config info again?
 * With the app selected press ctrl+alt+r
* I setup the app but didn't upload my arena game.
 * Did you forget to restart Hearthstone after the initial config of Arena Tracker?
* I get "File: ERROR:Saving card image to disk. Make sure HSCards dir is in the same place as the exe." or "File: ERROR:Cannot find arenaTemplate.png. Make sure HSCards dir is in the same place as the exe."
 * Make sure HSCards dir is in the same dir as the binary and make sure you are calling the binary from the dir where both (the binary and HSCards dir) are. If you are using the terminal, calling from other dir (./ArenaTracker/ArenaTracker) is wrong.


## Video examples
* v1.1: https://www.youtube.com/watch?v=MyeUy9RSY7E
* v1.3: https://www.youtube.com/watch?v=9TsaSWSe9X4


## More info:
* https://github.com/supertriodo/Arena-Tracker
* http://www.reddit.com/r/ArenaTracker/
* http://www.arenamastery.com/
* http://www.heartharena.com/
