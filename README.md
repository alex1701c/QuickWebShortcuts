# QuickWebShortcuts

This plugin allows you to use search engines inside Krunner. But instead of specifying the search
engine every time can set one you want to use. The default is Google. If you type ":example query" it will open your 
browser with the google search results for "example query". The default options are Bing, DuckDuckGo,
Github, Google, Stackoverflow and Youtube. But you can add custom ones: The query gets appended to the provided url. 
For instance "https://www.amazon.com/s?k=" is valid.

Additionally you can use this plugin to open search results in private/incognito mode. 
This feature gets activated by typing "::example query".
It has been tested with Firefox, Chromium, Google Chrome and Epiphany.
But you have to manually select one of these as your default browser, otherwise Firefox is used as default.

This project also has a minimal branch. This branch is smaller and more efficient but it has no config dialog
and the settings have to be configured in the src/Config.h file. After changing the settings you have to recompile the project. 
The master branch (default) is recommended for most users.

### Build Instructions

Required Dependencies on Debian/Ubuntu:  
`sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5textwidgets-dev qtdeclarative5-dev gettext`

The easiest way to install is:  
`curl https://raw.githubusercontent.com/alex1701c/QuickWebShortcuts/master/install.sh | bash`

Or you can do it manually:

```
git clone https://github.com/alex1701c/QuickWebShortcuts.git  
cd QuickWebShortcuts/
mkdir build
cd build
cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` ..
make
sudo make install
```
Restart krunner to load the runner (in a terminal type: `kquitapp5 krunner;kstart5 krunner` )

## Screenshots

#### Simple search
![Simple search](https://raw.githubusercontent.com/alex1701c/Screenshots/master/QuickWebShortcuts/simple_search.png)

#### Search in private window
![Search in private window](https://raw.githubusercontent.com/alex1701c/Screenshots/master/QuickWebShortcuts/private_window_search.png)

#### Change search engine in runner
![Change search engine in runner](https://raw.githubusercontent.com/alex1701c/Screenshots/master/QuickWebShortcuts/change_searchengine_inside_runner.png)

#### Add search engine
![Add search engine](https://raw.githubusercontent.com/alex1701c/Screenshots/master/QuickWebShortcuts/dialog_add_searchengine.png)

#### Edit search engine
![Edit search engine](https://raw.githubusercontent.com/alex1701c/Screenshots/master/QuickWebShortcuts/dialog_edit_searchengines.png)

