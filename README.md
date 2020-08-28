# Arena-Tracker
Arena Tracker reads the Hearthstone log to give your all the info you need. [Learn More...](https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/More.md)

Too complicated? [Check the User Guide](https://triodo.gitbook.io/arena-tracker-documentation/en).

Join our community at the [Arena Tracker subreddit](https://www.reddit.com/r/ArenaTracker/) and support its development at the [Arena Tracker patreon](https://www.patreon.com/triodo).

[![Patreon](https://c5.patreon.com/external/logo/become_a_patron_button.png)](https://www.patreon.com/triodo)

![InGame](https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/inGame.png)


## Download (Windows/Mac/Linux)
[![Download](https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/downloadButton.png)](https://github.com/supertriodo/Arena-Tracker/releases/latest)

* Tested on Windows 7/8/10
* Tested on Mac OS X 10.11
* Tested on Ubuntu/Linux Mint
  * You can downloading **Arena.Tracker.X.Linux.zip** or **ArenaTracker.Linux.AppImage**.
  * For Arch Linux users there's a package in [AUR](https://aur.archlinux.org/packages/arenatracker-bin/) maintained by [Strit](https://github.com/Strit).
  * If you're having problems running HS with AT on linux, HS dont accept mouse clicks, try Windowed (virtual desktop) option and it will work perfectly. You can do it easily with lutris.

Extract the zip wherever you want and double click the executable, no installation required.
(If you want to run it from a terminal don't use sudo.)

Data used by Arena Tracker will be stored in `USER/Arena Tracker` (Windows/Mac) or `USER/.local/share/Arena Tracker` (Linux) with the following structure:
* Arena Tracker\
  * ArenaTrackerLog.txt     -- (Log created by Arena Tracker, used for debugging)
  * ArenaTrackerDecks.json  -- (Your deck collection in Arena Tracker)
  * Extra\                  -- (Template and scripts used by Arena Tracker)
  * Games Drafts Logs\      -- (Logs of Games/Drafts to upload to Zero To Heroes)
  * Hearthstone Cards\      -- (Hearthstone cards images)
  
Arena Tracker will need to download all Hearthstone card images onto Hearthstone Cards dir. This is done in the background while you use it.


##  First Run
On **Windows and Mac**: Arena Tracker setup is done automatically unless you didn't install Hearthstone in its default dir. In that case the first time you run Arena Tracker you will be asked for:

1) Logs dir location (If not default).
 * Default (Win/WINE): C:\Program Files (x86)\Hearthstone\Logs
 * Default (Mac): /Applications/Hearthstone/Logs
 * If the Logs dir doesn't exist create it.
2) log.config location (If not default).
 * Default (Win 7-10): USER\AppData\Local\Blizzard\Hearthstone\log.config
 * Default (Win XP/WINE): USER\Local Settings\Application Data\Blizzard\Hearthstone\log.config
 * Default (Mac): ~/Library/Preferences/Blizzard/Hearthstone/log.config
 * If the file doesn't exist create an empty log.config in that dir.
3) Restart Hearthstone (if you have it opened).

On **Linux**: Arena Tracker will try to find Hearthstone dir automatically using the command find (Only your HOME dir will be searched). If it doesn't success it will ask you to find the above locations. This process can take several minutes, I suggest you to empty your trash bin before, to make it short.

If you have problems installing it or using it I'm happy to help you on [reddit](http://www.reddit.com/r/ArenaTracker/).


## [FAQ](https://arena-tracker.gitbooks.io/arena-tracker-documentation/content/en/faq.html)



## Build from source (Windows/Linux/Mac) (Only advanced users)
* (Only Mac) Download & install XCode
  * https://developer.apple.com/downloads/?name=Xcode
* Download & install QT Community.
  * http://www.qt.io/download-open-source/
* Download, compile & install Opencv v2.4.11 (NOT 3.0) (This step can be tricky)
  * http://opencv.org/downloads.html
  * On Ubuntu I followed this guide: (changing the version of ovencv to v2.4.11)
   http://rodrigoberriel.com/2014/10/installing-opencv-3-0-0-on-ubuntu-14-04/
  * On Mac I followed this guide:
   https://www.learnopencv.com/configuring-qt-for-opencv-on-osx/
* Download or clone Arena Tracker source code.
  * https://github.com/supertriodo/Arena-Tracker/archive/master.zip
* Open Qt Creator and open Arena Tracker project.
  * Open File or project... Look for ArenaTracker.pro
* In the botton left set the build to Release and Run.
 
If you build on Mac check:
* https://github.com/supertriodo/Arena-Tracker/issues/1


## Is Blizzard okay with this?
[Yes](https://twitter.com/bdbrode/status/511151446038179840)  


## Video examples
* v4.41: https://www.youtube.com/watch?v=k6pOHXFQrzQ
* v3.21: https://www.youtube.com/watch?v=U-XzJZ5UG2k
* v2.0: https://www.youtube.com/watch?v=IMckn_lmW84
* v1.3: https://www.youtube.com/watch?v=9TsaSWSe9X4
* v1.1: https://www.youtube.com/watch?v=MyeUy9RSY7E


## Reddit:
* http://www.reddit.com/r/ArenaTracker


## Thanks to:
* https://hsreplay.net
* http://hearthstonejson.com
* http://www.zerotoheroes.com/s/hearthstone
* http://thelightforge.com/TierList
* http://www.heartharena.com
* https://sourceforge.net/projects/libxls/
* https://github.com/stevschmid/track-o-bot
* https://github.com/probonopd/linuxdeployqt
* Arena Tracker is a [HearthSim](http://hearthsim.info) project. Join us on [Discord](https://discord.gg/hearthsim).

Thanks to all these projects ArenaTracker is possible.


## Support:
[![Patreon](https://c5.patreon.com/external/logo/become_a_patron_button.png)](https://www.patreon.com/triodo)

[![PayPal](https://www.paypalobjects.com/webstatic/en_US/i/btn/png/gold-rect-paypal-44px.png)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)
