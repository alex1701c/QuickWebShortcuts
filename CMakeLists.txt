cmake_minimum_required(VERSION 3.16)

project(quickwebshortcuts)
set(CMAKE_PROJECT_VERSION "3.0.1")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/alex1701c/QuickWebShortcuts")
set(CMAKE_PROJECT_AUTHOR "alex1701c")
set(CMAKE_PROJECT_AUTHOR_EMAIL "alex1701c.dev@gmx.net")

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
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n Runner KCMUtils Notifications)
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
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CMAKE_PROJECT_AUTHOR}")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")

set(KRUNNER_VERSION "${KF${QT_MAJOR_VERSION}Runner_VERSION}")
set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_OS}-plasma${QT_MAJOR_VERSION})

if(CPACK_OS STREQUAL "ubuntu")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libkf${QT_MAJOR_VERSION}runner${QT_MAJOR_VERSION} (>=${KRUNNER_VERSION})")
	set(CPACK_GENERATOR "DEB")
elseif(CPACK_OS STREQUAL "opensuse" AND QT_MAJOR_VERSION STREQUAL 5)
	set(CPACK_RPM_PACKAGE_REQUIRES "libKF5Runner5 >= ${KRUNNER_VERSION}")
	set(CPACK_GENERATOR "RPM")
elseif(CPACK_OS STREQUAL "fedora" OR CPACK_OS STREQUAL "opensuse")
	set(CPACK_RPM_PACKAGE_REQUIRES "kf${QT_MAJOR_VERSION}-krunner >= ${KRUNNER_VERSION}")
	set(CPACK_GENERATOR "RPM")
else()
  unset(CPACK_PACKAGE_FILE_NAME)
endif()

# These scripts will kill krunner and when it is invoked next time the plugins will be loaded
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/postinst;${CMAKE_CURRENT_SOURCE_DIR}/postrm")
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postrm")

INCLUDE(CPack)

