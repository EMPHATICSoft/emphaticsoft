# Install script for directory: /emphaticsoft/SSD_FERMerger

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSSD_FERMerger.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/Splitter")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Splitter")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin" TYPE EXECUTABLE FILES "/build/slf7.x86_64.e20.prof/bin/Merger")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/bin/Merger")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/SSD_FERMerger" TYPE FILE FILES
    "/emphaticsoft/SSD_FERMerger/DataDecoder.cpp"
    "/emphaticsoft/SSD_FERMerger/DataDecoder.h"
    "/emphaticsoft/SSD_FERMerger/Event.cpp"
    "/emphaticsoft/SSD_FERMerger/Event.h"
    "/emphaticsoft/SSD_FERMerger/FilesMerger.cpp"
    "/emphaticsoft/SSD_FERMerger/FilesMerger.h"
    "/emphaticsoft/SSD_FERMerger/FilesSplitter.cpp"
    "/emphaticsoft/SSD_FERMerger/FilesSplitter.h"
    "/emphaticsoft/SSD_FERMerger/Hit.cpp"
    "/emphaticsoft/SSD_FERMerger/Hit.h"
    "/emphaticsoft/SSD_FERMerger/Merger.cc"
    "/emphaticsoft/SSD_FERMerger/PxEvent.cpp"
    "/emphaticsoft/SSD_FERMerger/PxEvent.h"
    "/emphaticsoft/SSD_FERMerger/README.md"
    "/emphaticsoft/SSD_FERMerger/Run.cpp"
    "/emphaticsoft/SSD_FERMerger/Run.h"
    "/emphaticsoft/SSD_FERMerger/SSDConsts.h"
    "/emphaticsoft/SSD_FERMerger/Splitter.cc"
    "/emphaticsoft/SSD_FERMerger/Status.h"
    "/emphaticsoft/SSD_FERMerger/runall.sh"
    "/emphaticsoft/SSD_FERMerger/setup.sh"
    "/emphaticsoft/SSD_FERMerger/stib.cpp"
    "/emphaticsoft/SSD_FERMerger/stib.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SSD_FERMerger" TYPE FILE FILES
    "/emphaticsoft/SSD_FERMerger/DataDecoder.h"
    "/emphaticsoft/SSD_FERMerger/Event.h"
    "/emphaticsoft/SSD_FERMerger/FilesMerger.h"
    "/emphaticsoft/SSD_FERMerger/FilesSplitter.h"
    "/emphaticsoft/SSD_FERMerger/Hit.h"
    "/emphaticsoft/SSD_FERMerger/PxEvent.h"
    "/emphaticsoft/SSD_FERMerger/Run.h"
    "/emphaticsoft/SSD_FERMerger/SSDConsts.h"
    "/emphaticsoft/SSD_FERMerger/Status.h"
    "/emphaticsoft/SSD_FERMerger/stib.h"
    )
endif()

