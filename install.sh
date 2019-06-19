#!/bin/bash

git clone https://github.com/alex1701c/QuickWebShortcuts
cd QuickWebShortcuts/
mkdir build
cd build
cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` ..
make
sudo make install

quitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";