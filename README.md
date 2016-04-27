# Arena-Tracker
Arena Tracker reads the Hearthstone log to give your all the info you need. Arena Tracker works in arena, constructed, adventures and tavern-brawl. [Learn More...](https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/More.md)

![InGame](https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/inGame.png)


## Download (Windows/Mac/Linux)
https://github.com/supertriodo/Arena-Tracker/releases/latest
* Tested on Windows 7/8/10
* Tested on Mac OS X 10.11
* Tested on Ubuntu/Linux Mint
* For Arch Linux users there's a package in AUR maintained by the community
 * https://aur.archlinux.org/packages/arena-tracker/

Extract the zip wherever you want and double click the executable, no installation required.

Data used by Arena Tracker will be stored in USER/Arena Tracker with the following structure:
* Arena Tracker\
 * ArenaTrackerLog.txt     -- (Log created by Arena Tracker, used for debugging)
 * ArenaTrackerDecks.json  -- (Your deck collection in Arena Tracker)
 * Extra\                  -- (Template and scripts used by Arena Tracker)
 * Games Drafts Logs\      -- (Logs of Games/Drafts to upload to Zero To Heroes)
 * Hearthstone Cards\      -- (Hearthstone cards images)


##  First Run
The first time you run Arena Tracker you will be asked for:

* 1) Logs dir location (If not default).
 * Default (Win/WINE): C:\Program Files (x86)\Hearthstone\Logs
 * Default (Mac): /Applications/Hearthstone/Logs
 * If the Logs dir doesn't exist create it.
* 2) log.config location (If not default).
 * Default (Win 7-10): USER\AppData\Local\Blizzard\Hearthstone\log.config
 * Default (Win XP/WINE): USER\Local Settings\Application Data\Blizzard\Hearthstone\log.config
 * Default (Mac): ~/Library/Preferences/Blizzard/Hearthstone/log.config
 * If the file doesn't exist create an empty log.config in that dir.
* 3) Restart Hearthstone (if you have it opened).

If you have problems installing it or using it I'm happy to help you on
http://www.reddit.com/r/ArenaTracker/


## FAQ
* Does it work with Hearthstone in fullscreen?
 * (Windows) Actually does. It's because Hearthstone fullscreen is just a borderless window.
 * (Linux) In Linux Mint I start Hearthstone in windows mode and when it's loaded I use ctrl-enter to enter fullscreen, this way Arena Tracker keep on top of Hearthstone while Hearthstone is in fullscreen.
 * (Mac) You need to play with Hearthstone in windows mode.
* Sometimes ArenaTracker doesn't stay on top of Hearthstone.
 * I have seen this behaviour in windows. It seems to be related to a QT/Windows bug. Try to restart ArenaTracker.
* I setup the app but it didn't upload my arena game/ it didn't start drafting.
 * This usually means Hearthstone is not writting in its log.
 * Did you forget to restart Hearthstone after the initial config of Arena Tracker?
 * If you use other trackers they can conflict configuring [log.config](https://github.com/supertriodo/Arena-Tracker/issues/8) (Reset the app (ctrl+r) and log.config will be recreated).
* Drafting doesn't read next card.
 * Once you start an arena draft you cannot move hearthstone window until the end of the draft.
 * After picking a card move the mouse to the zone below or above the card so the next one can be read.
 * If you use other trackers they can conflict configuring [log.config](https://github.com/supertriodo/Arena-Tracker/issues/8) (Reset the app (ctrl+r) and log.config will be recreated).
* What does it mean when text cards are shown in yellow/orange?
 * Minions are shown in white.
 * Spells are shown in yellow.
 * Weapons are shown in orange.
* What does it mean when cards have a colored background?
 * It means they are class cards.
* How do I reset the app so I can set the config info again? Or move the app to its initial position and size.
 * With the app selected press ctrl+r
* I can't browse hidden .dirs when looking for log.config/output_log.txt in the file browser.
 * The file browser shown is OS dependent. In Linux Mint for instance you can use ctrl-h to show hidden dirs/files.
* How can I back-up my data.
 * Your arena stats are kept online in Arena Mastery. The only data kept locally is your constructed deck collection, they are stored in USER/Arena Tracker/ArenaTrackerDecks.json


## Build from source (Windows/Linux/Mac) (Only advanced users)
* (Mac) Download & install XCode
 * https://developer.apple.com/downloads/?name=Xcode
* Download & install QT Community.
 * http://www.qt.io/download-open-source/
* Download, compile & install Opencv v2.4.11 (NOT 3.0) (This step can be tricky)
 * http://opencv.org/downloads.html
 * On Ubuntu I followed this guide: (changing the version of ovencv to v2.4.11)
   http://rodrigoberriel.com/2014/10/installing-opencv-3-0-0-on-ubuntu-14-04/
 * On Mac I followed this guide:
   https://jjyap.wordpress.com/2014/05/24/installing-opencv-2-4-9-on-mac-osx-with-python-support/
* Download or clone Arena Tracker source code.
 * https://github.com/supertriodo/Arena-Tracker/archive/master.zip
* Open Qt Creator and open Arena Tracker project.
 * Open File or project... Look for ArenaTracker.pro
* In the botton left set the build to Release and Run.
* Copy HSCards directory to the build dir, to the same place the executable is.
 
If you build on Mac check:
* https://github.com/supertriodo/Arena-Tracker/issues/1


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
* http://www.zerotoheroes.com/s/hearthstone
* http://hearthstonejson.com/
* Arena Tracker is a [HearthSim](http://hearthsim.info) project. Come join us in #hearthsim on chat.freenode.net.

Thanks to all these projects ArenaTracker is possible.


## Top donors:
* [Michael Bull](https://github.com/MikeBull94): €10,00 EUR
* Dennis Kniel: €5,00 EUR
* Innotus: €5,00 EUR
 
(Thanks for keeping this project alive.)

[![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)
