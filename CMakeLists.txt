cmake_minimum_required(VERSION 3.0)

project(quickwebshortcuts)

set(QT_MIN_VERSION "5.0.0")
set(KF5_MIN_VERSION "5.0.0")

find_package(ECM 5.12.0 REQUIRED NO_MODULE)
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})

# Find the required Libaries
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Core Quick QuickWidgets Network)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS I18n Service Runner TextWidgets ConfigWidgets)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# KWallet is used for storing the proxy credentials, but it is not for the core functionallity required
find_package(KF5 ${KF5_MIN_VERSION} COMPONENTS Wallet)
if (NOT ${KF5Wallet_FOUND})
    message(WARNING "Could not find Wallet component, disabeling proxy feature!")
    add_definitions(-D NO_PROXY_INTEGRATION)
endif ()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

add_subdirectory(src)

