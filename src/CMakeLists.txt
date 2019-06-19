add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.quickwebshortcuts\")

set(quickwebshortcuts_SRCS quick_web_shortcuts.h quick_web_shortcuts.cpp
        SearchEngines.h SearchEngines.cpp)

add_library(krunner_quickwebshortcuts MODULE ${quickwebshortcuts_SRCS})
target_link_libraries(krunner_quickwebshortcuts KF5::Runner KF5::I18n)


set(kcm_krunner_quickwebshortcuts_SRCS
        config/quick_web_shortcuts_config.cpp
        SearchEngines.h
        SearchEngines.cpp
        )

ki18n_wrap_ui(kcm_krunner_quickwebshortcuts_SRCS config/quick_web_shortcuts_config.ui)
add_library(kcm_krunner_quickwebshortcuts MODULE ${kcm_krunner_quickwebshortcuts_SRCS})
target_link_libraries(kcm_krunner_quickwebshortcuts
        Qt5::Core
        Qt5::Gui
        KF5::CoreAddons
        KF5::ConfigCore
        KF5::I18n
        KF5::ConfigWidgets
        KF5::Runner
        )

add_dependencies(krunner_quickwebshortcuts kcm_krunner_quickwebshortcuts)


install(TARGETS krunner_quickwebshortcuts kcm_krunner_quickwebshortcuts DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-quickwebshortcuts.desktop config/plasma-runner-quickwebshortcuts_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
