# Arena-Tracker
Arena Tracker reads the Hearthstone log to give your all the info you need. Arena Tracker works in arena, constructed, adventures and tavern-brawl.
* Draft your arena with HearthArena advice.
* Upload your arena stats to your ArenaMastery account.
* Save and organize your constructed decks. No more 9 slots limit.
* Import decks from HearthHead.
* Go over your deck while you play.
* Get value info of enemy's hand. Cards sent back from the board, spare parts or the coin (among others).
* Test enemy secrets. Watch the remaining posibilities while you make your moves.

Arena Tracker is free and open source. Made by one person(me) in his spare time.

My Hearthstone ID: triodo#2541 (EU region)

If you like the app, donations are much appreciated. It allows me to spend more time developing Arena Tracker.

[![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)


## Download (Windows/Linux)
https://github.com/supertriodo/Arena-Tracker/releases/latest
* Tested on Windows 7/8/10
* Tested on Ubuntu/Linux Mint


##  First Run
The first time you run Arena Tracker you will be asked for:

* 1) output_log.txt location (If not default).
 * Default (Win): C:\Program Files (x86)\Hearthstone\Hearthstone_Data\output_log.txt
* 2) log.config location (If not default).
 * Default (Win 7-10): USER\AppData\Local\Blizzard\Hearthstone\log.config
 * Default (Win XP): USER\Local Settings\Application Data\Blizzard\Hearthstone\log.config
 * If the file doesn't exist create an empty log.config in that dir.
* 3) Restart Hearthstone (if you have it opened).

If you have problems installing it or using it I'm happy to help you on
http://www.reddit.com/r/ArenaTracker/
or on email: arenatracker@gmail.com

También puedes preguntarme en español.


## FAQ (v3.0+)
* Does it work with Hearthstone in fullscreen?
 * Actually does. It's because Hearthstone fullscreen is just a borderless window.
* Sometimes ArenaTracker doesn't stay on top of Hearthstone.
 * I have seen this behaviour in windows. It seems to be related to a QT/Windows bug. Try to restart ArenaTracker.
* I setup the app but it didn't upload my arena game/ it didn't start drafting.
 * This usually means Hearthstone is not writting in its log.
 * Did you forget to restart Hearthstone after the initial config of Arena Tracker?
 * If you use other trackers they can conflict configuring [log.config](https://github.com/supertriodo/Arena-Tracker/issues/8) (Remove log.config and start Arena Tracker).
* Drafting doesn't read next card.
 * Once you start an arena draft you cannot move hearthstone window until the end of the draft.
 * After picking a card move the mouse to the zone below or above the card so the next one can be read.
 * If you use other trackers they can conflict configuring [log.config](https://github.com/supertriodo/Arena-Tracker/issues/8) (Remove log.config and start Arena Tracker).
* What does it mean when cards in the decklist are highlighted in yellow/orange?
 * Minions are shown in white.
 * Spells are shown in yellow.
 * Weapons are shown in orange.
* How do I reset the app so I can set the config info again? Or move the app its initial position and size.
 * With the app selected press ctrl+r
* I get "File: ERROR:Saving card image to disk. Make sure HSCards dir is in the same place as the exe." or "File: ERROR:Cannot find arenaTemplate.png. Make sure HSCards dir is in the same place as the exe."
 * Make sure HSCards dir is in the same dir as the binary.
* I can't browse hidden .dirs when looking for log.config/output_log.txt in the file browser.
 * The file browser shown is OS dependent. In Linux Mint for instance you can use ctrl-h to show hidden dirs/files.


## Build from source (Windows/Linux/Mac) (Only advanced users)
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
 
If you build on Mac check: https://github.com/supertriodo/Arena-Tracker/issues/1


## Is Blizzard okay with this?
[Yes](https://twitter.com/bdbrode/status/511151446038179840)  


## Video examples
* v3.21: https://www.youtube.com/watch?v=U-XzJZ5UG2k
* v2.0: https://www.youtube.com/watch?v=IMckn_lmW84
* v1.3: https://www.youtube.com/watch?v=9TsaSWSe9X4
* v1.1: https://www.youtube.com/watch?v=MyeUy9RSY7E


## More info:
* http://www.reddit.com/r/ArenaTracker/
* http://www.arenamastery.com/
* http://www.heartharena.com/
* http://www.hearthhead.com/
* http://hearthstonejson.com/
* Arena Tracker is a [HearthSim](http://hearthsim.info) project. Come join us in #hearthsim on chat.freenode.net.

Thanks to all these projects ArenaTracker is possible.


## Top donors:
* Innotus: €5,00 EUR
 
(Thanks for keeping this project alive.)

[![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)
