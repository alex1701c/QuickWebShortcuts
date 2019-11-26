add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.quickwebshortcuts\")

set(quickwebshortcuts_SRCS quick_web_shortcuts.h quick_web_shortcuts.cpp searchproviders/Bing.h searchproviders/RequiredData.h
        SearchEngines.h searchproviders/Google.h searchproviders/DuckDuckGo.h SearchEngine.h)

add_library(krunner_quickwebshortcuts MODULE ${quickwebshortcuts_SRCS})
target_link_libraries(krunner_quickwebshortcuts KF5::Runner KF5::Plasma Qt5::Core Qt5::Network)


set(kcm_krunner_quickwebshortcuts_SRCS
        config/quick_web_shortcuts_config.cpp
        SearchEngines.h
        searchproviders/RequiredData.h SearchEngine.h)

ki18n_wrap_ui(kcm_krunner_quickwebshortcuts_SRCS config/quick_web_shortcuts_config.ui)
ki18n_wrap_ui(kcm_krunner_quickwebshortcuts_SRCS config/browser_item.ui)
add_library(kcm_krunner_quickwebshortcuts MODULE ${kcm_krunner_quickwebshortcuts_SRCS})
target_link_libraries(kcm_krunner_quickwebshortcuts
        Qt5::Core
        Qt5::Network
        Qt5::Gui
        KF5::CoreAddons
        KF5::ConfigCore
        KF5::I18n
        KF5::ConfigWidgets
        )

add_dependencies(krunner_quickwebshortcuts kcm_krunner_quickwebshortcuts)

#The icons have not been modified, sources:
#https://www.iconfinder.com/icons/1220346/overflow_stachoverflow_stack_stackoverflow_icon
#https://www.iconfinder.com/icons/317714/video_youtube_icon
#https://www.iconfinder.com/icons/1217121/bing_icon
#https://www.iconfinder.com/icons/298822/github_mark_icon
#https://www.iconfinder.com/icons/1313555/business_duckduckgo_engine_internet_optimization_search_web_icon
#https://www.iconfinder.com/icons/1298745/google_icon

set(ICON_INSTALL_DIR "/usr/share/pixmaps" CACHE STRING "Default icon directory")

install(TARGETS krunner_quickwebshortcuts kcm_krunner_quickwebshortcuts DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-quickwebshortcuts.desktop config/plasma-runner-quickwebshortcuts_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES icons/bing.svg icons/duckduckgo.svg icons/github.svg icons/google.svg icons/stackoverflow.svg icons/youtube.svg DESTINATION /usr/share/icons/)
install(DIRECTORY icons/ DESTINATION ${ICON_INSTALL_DIR})
