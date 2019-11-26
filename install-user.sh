#!/bin/bash

# Exit if something fails
set -e

if [[ $(basename "$PWD") != "QuickWebShortcuts"* ]];then
    git clone https://github.com/alex1701c/QuickWebShortcuts
    cd QuickWebShortcuts/
fi

# Create folders
mkdir -p build
mkdir -p ~/.local/lib/qt/plugins
mkdir -p ~/.local/share/kservices5
mkdir -p ~/.local/share/pixmaps

cd build

# Add the installation path to the QT_PLUGIN_PATH
if [[ -z "${QT_PLUGIN_PATH}" || "${QT_PLUGIN_PATH}" != *".local/lib/qt/plugins/"* ]]; then
    echo "export QT_PLUGIN_PATH=~/.local/lib/qt/plugins/:$QT_PLUGIN_PATH" >> ~/.bashrc
    export QT_PLUGIN_PATH=~/.local/lib/qt/plugins/:$QT_PLUGIN_PATH
fi

cmake -DQT_PLUGIN_INSTALL_DIR="~/.local/lib/qt/plugins" -DKDE_INSTALL_KSERVICES5DIR="~/.local/share/kservices5" -DICON_INSTALL_DIR="~/.local/share/pixmaps/" -DCMAKE_BUILD_TYPE=Release  ..
make -j$(nproc)
make install

kquitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";
