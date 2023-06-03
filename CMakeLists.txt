cmake_minimum_required(VERSION 3.16)

project(quickwebshortcuts)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.90.0")

find_package(ECM 5.12.0 REQUIRED NO_MODULE)
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
add_definitions(-DQT_NO_KEYWORDS)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})

# Find the required Libaries
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Core Quick QuickWidgets Network)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS I18n Service Runner ConfigWidgets Notifications)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# KWallet is used for storing the proxy credentials, but it is not for the core functionallity required
find_package(KF5 ${KF5_MIN_VERSION} COMPONENTS Wallet)
if (NOT ${KF5Wallet_FOUND})
    message(WARNING "Could not find Wallet component, disabling proxy feature!")
    add_definitions(-D NO_PROXY_INTEGRATION)
endif ()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x056700)

add_subdirectory(src)

# CPack build
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Krunner plugin to quickly use webshortcuts and search suggestions")
set(CPACK_PACKAGE_NAME krunner-${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "2.3.0")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "alex1701c")
SET(CPACK_GENERATOR "DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "kwalletmanager")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_REQUIRES "kwalletmanager")
INCLUDE(CPack)
