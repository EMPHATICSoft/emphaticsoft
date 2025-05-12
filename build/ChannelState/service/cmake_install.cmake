# Install script for directory: /emphaticsoft/ChannelState/service

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libChannelStateService.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art/v3_12_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/clhep/v2_4_5_3a/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/messagefacility/v2_09_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/slf7.x86_64.e20.prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE MODULE FILES "/build/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art/v3_12_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/messagefacility/v2_09_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/lib:/cvmfs/emphatic.opensciencegrid.org/products/clhep/v2_4_5_3a/Linux64bit+3.10-2.17-e20-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libChannelStateService_service.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ChannelState/service" TYPE FILE FILES "/emphaticsoft/ChannelState/service/ChannelStateService.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/fcl" TYPE FILE FILES "/emphaticsoft/ChannelState/service/ChannelState.fcl")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/ChannelState/service" TYPE FILE FILES
    "/emphaticsoft/ChannelState/service/ChannelStateService.cc"
    "/emphaticsoft/ChannelState/service/ChannelStateService.h"
    "/emphaticsoft/ChannelState/service/ChannelStateService_service.cc"
    )
endif()

