# Install script for directory: /emphaticsoft/OnlineMonitoring/util

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so"
         OLD_RPATH "/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libOnlineMonitoringUtil.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/testHistoTable")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testHistoTable")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/testIPCBlockServer")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockServer")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/testIPCBlockClient")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/testIPCBlockClient")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/onmon_reset_locks")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/onmon_reset_locks")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/OnlineMonitoring/util" TYPE FILE FILES
    "/emphaticsoft/OnlineMonitoring/util/HistoData.cxx"
    "/emphaticsoft/OnlineMonitoring/util/HistoData.h"
    "/emphaticsoft/OnlineMonitoring/util/HistoTable.cxx"
    "/emphaticsoft/OnlineMonitoring/util/HistoTable.h"
    "/emphaticsoft/OnlineMonitoring/util/IPC.cxx"
    "/emphaticsoft/OnlineMonitoring/util/IPC.h"
    "/emphaticsoft/OnlineMonitoring/util/IPCBlock.h"
    "/emphaticsoft/OnlineMonitoring/util/RegexMatch.cxx"
    "/emphaticsoft/OnlineMonitoring/util/RegexMatch.h"
    "/emphaticsoft/OnlineMonitoring/util/Settings.cxx"
    "/emphaticsoft/OnlineMonitoring/util/Settings.h"
    "/emphaticsoft/OnlineMonitoring/util/Ticker.cxx"
    "/emphaticsoft/OnlineMonitoring/util/Ticker.h"
    "/emphaticsoft/OnlineMonitoring/util/TickerSubscriber.cxx"
    "/emphaticsoft/OnlineMonitoring/util/TickerSubscriber.h"
    "/emphaticsoft/OnlineMonitoring/util/hdltohex.cc"
    "/emphaticsoft/OnlineMonitoring/util/hextohdl.cc"
    "/emphaticsoft/OnlineMonitoring/util/onmon_reset_locks.cc"
    "/emphaticsoft/OnlineMonitoring/util/testHistoTable.cc"
    "/emphaticsoft/OnlineMonitoring/util/testIPCBlockClient.cc"
    "/emphaticsoft/OnlineMonitoring/util/testIPCBlockServer.cc"
    )
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

