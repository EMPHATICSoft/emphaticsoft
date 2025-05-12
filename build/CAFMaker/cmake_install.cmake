# Install script for directory: /emphaticsoft/CAFMaker

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libCAFMaker.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/artdaq_core/v3_09_03/slf7.x86_64.e20.s118.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art/v3_12_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/clhep/v2_4_5_3a/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/messagefacility/v2_09_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.rootmap")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/lib/libCAFMaker_dict_rdict.pcm")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art/v3_12_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/clhep/v2_4_5_3a/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/messagefacility/v2_09_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_dict.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art_root_io/v1_11_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/artdaq_core/v3_09_03/slf7.x86_64.e20.s118.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/art/v3_12_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas_root_io/v1_11_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/boost/v1_80_0/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/canvas/v3_14_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/messagefacility/v2_09_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/clhep/v2_4_5_3a/Linux64bit+3.10-2.17-e20-prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/fhiclcpp/v4_17_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib/v3_16_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/hep_concurrency/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/cetlib_except/v1_08_00/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/tbb/v2021_7_0/Linux64bit+3.10-2.17-e20/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libCAFMaker_module.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CAFMaker" TYPE FILE FILES
    "/emphaticsoft/CAFMaker/ARICHFiller.h"
    "/emphaticsoft/CAFMaker/BACkovFiller.h"
    "/emphaticsoft/CAFMaker/CAFMakerParams.h"
    "/emphaticsoft/CAFMaker/ClusterFiller.h"
    "/emphaticsoft/CAFMaker/EventQualFiller.h"
    "/emphaticsoft/CAFMaker/FillerBase.h"
    "/emphaticsoft/CAFMaker/GasCkovFiller.h"
    "/emphaticsoft/CAFMaker/HeaderFiller.h"
    "/emphaticsoft/CAFMaker/SRTruthFiller.h"
    "/emphaticsoft/CAFMaker/SSDHitsFiller.h"
    "/emphaticsoft/CAFMaker/SpacePointFiller.h"
    "/emphaticsoft/CAFMaker/TrackFiller.h"
    "/emphaticsoft/CAFMaker/TrackSegmentFiller.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/fcl" TYPE FILE FILES
    "/emphaticsoft/CAFMaker/CAFMaker.fcl"
    "/emphaticsoft/CAFMaker/cafmaker_job.fcl"
    "/emphaticsoft/CAFMaker/prod_reco_caf_job.fcl"
    "/emphaticsoft/CAFMaker/prod_reco_ssdonly_caf_job.fcl"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/CAFMaker" TYPE FILE FILES
    "/emphaticsoft/CAFMaker/ARICHFiller.cxx"
    "/emphaticsoft/CAFMaker/ARICHFiller.h"
    "/emphaticsoft/CAFMaker/BACkovFiller.cxx"
    "/emphaticsoft/CAFMaker/BACkovFiller.h"
    "/emphaticsoft/CAFMaker/CAFMakerParams.h"
    "/emphaticsoft/CAFMaker/CAFMaker_module.cc"
    "/emphaticsoft/CAFMaker/ClusterFiller.cxx"
    "/emphaticsoft/CAFMaker/ClusterFiller.h"
    "/emphaticsoft/CAFMaker/EventQualFiller.cxx"
    "/emphaticsoft/CAFMaker/EventQualFiller.h"
    "/emphaticsoft/CAFMaker/FillerBase.h"
    "/emphaticsoft/CAFMaker/GasCkovFiller.cxx"
    "/emphaticsoft/CAFMaker/GasCkovFiller.h"
    "/emphaticsoft/CAFMaker/HeaderFiller.cxx"
    "/emphaticsoft/CAFMaker/HeaderFiller.h"
    "/emphaticsoft/CAFMaker/SRTruthFiller.cxx"
    "/emphaticsoft/CAFMaker/SRTruthFiller.h"
    "/emphaticsoft/CAFMaker/SSDHitsFiller.cxx"
    "/emphaticsoft/CAFMaker/SSDHitsFiller.h"
    "/emphaticsoft/CAFMaker/SpacePointFiller.cxx"
    "/emphaticsoft/CAFMaker/SpacePointFiller.h"
    "/emphaticsoft/CAFMaker/TrackFiller.cxx"
    "/emphaticsoft/CAFMaker/TrackFiller.h"
    "/emphaticsoft/CAFMaker/TrackSegmentFiller.cxx"
    "/emphaticsoft/CAFMaker/TrackSegmentFiller.h"
    "/emphaticsoft/CAFMaker/classes.h"
    "/emphaticsoft/CAFMaker/classes_def.xml"
    )
endif()

