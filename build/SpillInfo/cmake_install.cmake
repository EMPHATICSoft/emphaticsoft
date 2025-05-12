# Install script for directory: /emphaticsoft/SpillInfo

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE MODULE FILES "/build/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/ifdh_art/v2_15_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/artdaq_core/v3_09_03/slf7.x86_64.e20.s118.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/ifbeam/v2_5_14/Linux64bit+3.10-2.17-e20-p3913-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/ifdhc/v2_6_8/Linux64bit+3.10-2.17-e20-p3913-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art/v3_12_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/messagefacility/v2_09_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/clhep/v2_4_5_3a/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libSpillInfo_module.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/fcl" TYPE FILE FILES
    "/emphaticsoft/SpillInfo/SpillInfo.fcl"
    "/emphaticsoft/SpillInfo/spillinfo_job.fcl"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/SpillInfo" TYPE FILE FILES
    "/emphaticsoft/SpillInfo/SpillInfo.cxx"
    "/emphaticsoft/SpillInfo/SpillInfo_module.cc"
    )
endif()

