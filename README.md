# Arena-Tracker
Arena Tracker reads the Hearthstone log to keep track of your arena deck, games and rewards.
* Draft with HearthArena advice.
* Upload your stats to your ArenaMastery account.
* Go over your deck while you play.
* Get value info of enemy's hand. Cards sent back from the board, spare parts or the coin (among others).
* Test enemy secrets. Watch the remaining posibilities while you make your moves.

Arena Tracker is free and open source. Made by one person(me) in his spare time.

My Hearthstone ID: triodo#2541 (EU region)

If you like the app, donations are much appreciated. It allows me to spend more time developing Arena Tracker.
[![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)


## Download (Windows)
https://github.com/supertriodo/Arena-Tracker/releases/download/v2.1/Arena.Tracker.v2.1.zip


## Build from source (Linux, Mac & Windows) (Only advanced users)
* Download & install QT Community.
 * http://www.qt.io/download-open-source/
* Download, compile & install Opencv v2.4.11 (NOT 3.0) (This step can be tricky)
 * http://opencv.org/downloads.html
 * In Ubuntu I followed this guide (changing the version of ovencv to v2.4.11
   http://rodrigoberriel.com/2014/10/installing-opencv-3-0-0-on-ubuntu-14-04/
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


## Is Blizzard okay with this?
[Yes](https://twitter.com/bdbrode/status/511151446038179840)  


## Video examples
* v2.0: http://youtu.be/IMckn_lmW84
* v1.3: https://www.youtube.com/watch?v=9TsaSWSe9X4
* v1.1: https://www.youtube.com/watch?v=MyeUy9RSY7E


## More info:
* https://github.com/supertriodo/Arena-Tracker
* http://www.reddit.com/r/ArenaTracker/
* http://www.arenamastery.com/
* http://www.heartharena.com/
