cmake_minimum_required(VERSION 3.16)

project(quickwebshortcuts)

set(QT_MIN_VERSION "5.15.0")
set(KF_MIN_VERSION "5.91.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
add_definitions(-DQT_NO_KEYWORDS)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(QtVersionOption)
include(KDEInstallDirs)
include(KDEClangFormat)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMDeprecationSettings)
include(FeatureSummary)


# Find the required Libaries
find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Core Network)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n Service Runner KCMUtils Notifications)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# KWallet is used for storing the proxy credentials, but it is not for the core functionallity required
find_package(KF${QT_MAJOR_VERSION} ${KF5_MIN_VERSION} COMPONENTS Wallet)
if (NOT ${KF${QT_MAJOR_VERSION}Wallet_FOUND})
    message(WARNING "Could not find Wallet component, disabling proxy feature!")
    add_definitions(-DNO_PROXY_INTEGRATION)
endif ()

ecm_set_disabled_deprecation_versions(
	QT 6.0.0
	KF ${KF_MIN_VERSION}
)

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
