# Install script for directory: /emphaticsoft/OnlineMonitoring/viewer

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil_rdict.pcm")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.rootmap")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringViewer.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/onmon_viewer")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_viewer")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/OnlineMonitoring/viewer" TYPE FILE FILES
    "/emphaticsoft/OnlineMonitoring/viewer/ButtonBank.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/ButtonBank.h"
    "/emphaticsoft/OnlineMonitoring/viewer/CaptionBox.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/CaptionBox.h"
    "/emphaticsoft/OnlineMonitoring/viewer/ComparisonBox.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/ComparisonBox.h"
    "/emphaticsoft/OnlineMonitoring/viewer/ComparisonOptions.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/ComparisonOptions.h"
    "/emphaticsoft/OnlineMonitoring/viewer/CurrentHistogram.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/CurrentHistogram.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIMain.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIMain.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModel.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModel.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModelData.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModelData.h"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModelSubscriber.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/GUIModelSubscriber.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HeaderBar.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/HeaderBar.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HistoSource.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/HistoSource.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HistogramBrowser.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/HistogramBrowser.h"
    "/emphaticsoft/OnlineMonitoring/viewer/HistogramInfo.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/HistogramInfo.h"
    "/emphaticsoft/OnlineMonitoring/viewer/Icons.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/Icons.h"
    "/emphaticsoft/OnlineMonitoring/viewer/Layout.h"
    "/emphaticsoft/OnlineMonitoring/viewer/LinkDef.h"
    "/emphaticsoft/OnlineMonitoring/viewer/LogViewer.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/LogViewer.h"
    "/emphaticsoft/OnlineMonitoring/viewer/MenuBar.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/MenuBar.h"
    "/emphaticsoft/OnlineMonitoring/viewer/OStream.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/OStream.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotClickHandler.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotClickHandler.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotClickerHandler.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotOptions.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotOptions.h"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotViewer.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/PlotViewer.h"
    "/emphaticsoft/OnlineMonitoring/viewer/TabsBox.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/TabsBox.h"
    "/emphaticsoft/OnlineMonitoring/viewer/UTCLabel.cxx"
    "/emphaticsoft/OnlineMonitoring/viewer/UTCLabel.h"
    "/emphaticsoft/OnlineMonitoring/viewer/onmon_viewer.cc"
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

