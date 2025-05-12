# Install script for directory: /emphaticsoft/OnlineMonitoring

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OnlineMonitoring/plotter" TYPE FILE FILES "/emphaticsoft/OnlineMonitoring/plotter/HistoSet.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OnlineMonitoring/util" TYPE FILE FILES
    "/emphaticsoft/OnlineMonitoring/util/HistoData.h"
    "/emphaticsoft/OnlineMonitoring/util/HistoTable.h"
    "/emphaticsoft/OnlineMonitoring/util/IPC.h"
    "/emphaticsoft/OnlineMonitoring/util/IPCBlock.h"
    "/emphaticsoft/OnlineMonitoring/util/RegexMatch.h"
    "/emphaticsoft/OnlineMonitoring/util/Settings.h"
    "/emphaticsoft/OnlineMonitoring/util/Ticker.h"
    "/emphaticsoft/OnlineMonitoring/util/TickerSubscriber.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OnlineMonitoring/viewer" TYPE FILE FILES
    "/emphaticsoft/OnlineMonitoring/viewer/ButtonBank.h"
    "/emphaticsoft/OnlineMonitoring/viewer/CaptionBox.h"
    "/emphaticsoft/OnlineMonitoring/viewer/ComparisonBox.h"
    "/emphaticsoft/OnlineMonitoring/viewer/ComparisonOptions.h"
    "/emphaticsoft/OnlineMonitoring/viewer/CurrentHistogram.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIMain.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModel.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModelData.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModelSubscriber.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HeaderBar.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HistoSource.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HistogramBrowser.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HistogramInfo.h"
    "/emphaticsoft/OnlineMonitoring/viewer/Icons.h"
    "/emphaticsoft/OnlineMonitoring/viewer/Layout.h"
    "/emphaticsoft/OnlineMonitoring/viewer/LinkDef.h"
    "/emphaticsoft/OnlineMonitoring/viewer/LogViewer.h"
    "/emphaticsoft/OnlineMonitoring/viewer/MenuBar.h"
    "/emphaticsoft/OnlineMonitoring/viewer/OStream.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotClickHandler.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotClickerHandler.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotOptions.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotViewer.h"
    "/emphaticsoft/OnlineMonitoring/viewer/TabsBox.h"
    "/emphaticsoft/OnlineMonitoring/viewer/UTCLabel.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/OnlineMonitoring/plotter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/OnlineMonitoring/util/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/OnlineMonitoring/viewer/cmake_install.cmake")
endif()

