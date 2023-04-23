# Arena-Tracker
Arena Tracker is a deck Tracker for Hearthstone with arena in focus. It has a free and a premium version that you can unlock by becoming a patron (3€/month). Learn the difference between the two [here](https://github.com/supertriodo/Arena-Tracker/blob/master/Readme/More.md).

Too complicated? [Check the User Guide](https://triodo.gitbook.io/arena-tracker-documentation/en).

Join our community at the [Arena Tracker subreddit](https://www.reddit.com/r/ArenaTracker/).

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

Data used by Arena Tracker will be stored in `USER/Arena Tracker` (Windows/Mac) or `USER/.local/share/Arena Tracker` (Linux) with [this](https://triodo.gitbook.io/arena-tracker-documentation/en/installation#ATdir) structure
  
Arena Tracker will need to download all Hearthstone card images onto Hearthstone Cards dir. This is done in the background while you use it. You don't need to wait, start your draft or your next game, AT will know what to do.


##  First Run
On **Windows and Mac**: Arena Tracker setup is done automatically unless you didn't install Hearthstone in its default dir. In that case the first time you run Arena Tracker you will be asked for several dir/files. Check [this](https://triodo.gitbook.io/arena-tracker-documentation/#i_setup_the_app_but_it_doesnt_work_it_didnt_upload_my_arena_game_it_didnt_start_drafting-1).

On **Linux**: Arena Tracker will try to find Hearthstone dir automatically using the command find (Only your HOME dir will be searched). This process can take several minutes, I suggest you to empty your trash bin before, to make it short.
If it doesn't success it will ask you to find some dir/files, check [this](https://triodo.gitbook.io/arena-tracker-documentation/en/installation).

If AT doesn't react to what you do in HS, you can try to manually configure HS log like [this](https://triodo.gitbook.io/arena-tracker-documentation/#activate-hs-logs-manually).

If you have problems installing it or using it I'm happy to help you on [reddit](http://www.reddit.com/r/ArenaTracker/).


## [FAQ](https://triodo.gitbook.io/arena-tracker-documentation/)



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


## The evolution of AT
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
* http://www.heartharena.com
* https://github.com/probonopd/linuxdeployqt
* Arena Tracker is a [HearthSim](http://hearthsim.info) project. Join us on [Discord](https://discord.gg/hearthsim).

Thanks to all these projects ArenaTracker is possible.


## Support:
[![Patreon](https://c5.patreon.com/external/logo/become_a_patron_button.png)](https://www.patreon.com/triodo)

[![PayPal](https://www.paypalobjects.com/webstatic/en_US/i/btn/png/gold-rect-paypal-44px.png)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=triodo%40gmail%2ecom&lc=GB&item_name=Arena%20Tracker&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)

[![HumblePartner-320w](https://user-images.githubusercontent.com/8259393/233829125-6763862b-5c71-419d-a1f7-dc56f3e28c2c.png)](https://www.humblebundle.com/?partner=arenatracker)


