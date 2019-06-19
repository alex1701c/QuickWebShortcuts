# Build Instructions

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
