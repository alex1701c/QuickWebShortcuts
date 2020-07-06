add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.quickwebshortcuts\")

set(quickwebshortcuts_SRCS quick_web_shortcuts.h quick_web_shortcuts.cpp searchproviders/Bing.h
        searchproviders/Google.h searchproviders/DuckDuckGo.h)

add_library(krunner_quickwebshortcuts MODULE ${quickwebshortcuts_SRCS})
target_link_libraries(krunner_quickwebshortcuts KF5::Runner KF5::Notifications Qt5::Core Qt5::Network)

if (${KF5Wallet_FOUND})
    target_link_libraries(krunner_quickwebshortcuts KF5::Wallet)
endif ()


set(kcm_krunner_quickwebshortcuts_SRCS
        config/quick_web_shortcuts_config.cpp
        config/SearchEngineItem.cpp)

ki18n_wrap_ui(kcm_krunner_quickwebshortcuts_SRCS config/quick_web_shortcuts_config.ui)
ki18n_wrap_ui(kcm_krunner_quickwebshortcuts_SRCS config/search_engine_item.ui)
add_library(kcm_krunner_quickwebshortcuts MODULE ${kcm_krunner_quickwebshortcuts_SRCS})
target_link_libraries(kcm_krunner_quickwebshortcuts
        Qt5::Core
        Qt5::Network
        Qt5::Gui
        KF5::CoreAddons
        KF5::ConfigCore
        KF5::I18n
        KF5::ConfigWidgets
        KF5::Notifications
        )

if (${KF5Wallet_FOUND})
    target_link_libraries(kcm_krunner_quickwebshortcuts KF5::Wallet)
endif ()

add_dependencies(krunner_quickwebshortcuts kcm_krunner_quickwebshortcuts)

#The icons have not been modified, sources:
#https://www.iconfinder.com/icons/1220346/overflow_stachoverflow_stack_stackoverflow_icon
#https://www.iconfinder.com/icons/317714/video_youtube_icon
#https://www.iconfinder.com/icons/1217121/bing_icon
#https://www.iconfinder.com/icons/298822/github_mark_icon
#https://www.iconfinder.com/icons/1313555/business_duckduckgo_engine_internet_optimization_search_web_icon
#https://www.iconfinder.com/icons/1298745/google_icon

set(SEARCH_ENGINE_ICON_DIR "/usr/share/pixmaps" CACHE STRING "Default icon directory")

install(TARGETS kcm_krunner_quickwebshortcuts DESTINATION ${KDE_INSTALL_QTPLUGINDIR})
install(TARGETS krunner_quickwebshortcuts DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/kf5/krunner)
install(FILES config/plasma-runner-quickwebshortcuts_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES icons/bing.svg icons/duckduckgo.svg icons/github.svg icons/google.svg icons/stackoverflow.svg icons/youtube.svg DESTINATION ${SEARCH_ENGINE_ICON_DIR})

if(KF5Runner_VERSION VERSION_LESS 5.72.0)
    install(FILES plasma-runner-quickwebshortcuts.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
else()
    kcoreaddons_desktop_to_json(krunner_quickwebshortcuts plasma-runner-quickwebshortcuts.desktop)
endif()
