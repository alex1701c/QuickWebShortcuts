#!/bin/bash

#git clone https://github.com/alex1701c/QuickWebShortcuts
#cd QuickWebShortcuts/
#mkdir build
#cd build
#cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` ..
#make
#sudo make install

choices=(google duckduckgo stackoverflow bing github youtube)
urls=(https://www.google.com/search?q= https://duckduckgo.com/?q= https://stackoverflow.com/search?q= https://www.bing.com/search?q= https://github.com/search?q= https://www.youtube.com/results?search_query= )
echo "Please enter the number of the search engine you want to use (default google)"
for index in ${!choices[*]}
do
    printf "%d: %s\n" ${index} ${choices[$index]}
done
read user_choice
echo ""  >> ~/.config/krunnerrc
echo "[Runners][QuickWebShortcuts]" >> ~/.config/krunnerrc
printf "url=%s\n" ${urls[$user_choice]} >> ~/.config/krunnerrc
history_choices=(all quick false)
echo "Do you want to clear the history of 0: All Web Shortcuts (default), 1: Quick Web Shortcuts, 2: Do not clear"
read clean_history
printf "clean_history=%s\n\n" ${history_choices[$clean_history]} >> ~/.config/krunnerrc


quitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";