# Install script for directory: /emphaticsoft/StandardRecord

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libStandardRecord.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so"
         OLD_RPATH "/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.rootmap")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/lib/libStandardRecord_dict_rdict.pcm")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so"
         OLD_RPATH "/build/slf7.x86_64.e20.prof/lib:/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libStandardRecord_dict.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/StandardRecord" TYPE FILE FILES
    "/emphaticsoft/StandardRecord/SRARing.h"
    "/emphaticsoft/StandardRecord/SRARingBranch.h"
    "/emphaticsoft/StandardRecord/SRBACkov.h"
    "/emphaticsoft/StandardRecord/SRBACkovBranch.h"
    "/emphaticsoft/StandardRecord/SREventQual.h"
    "/emphaticsoft/StandardRecord/SRGasCkov.h"
    "/emphaticsoft/StandardRecord/SRGasCkovBranch.h"
    "/emphaticsoft/StandardRecord/SRHeader.h"
    "/emphaticsoft/StandardRecord/SRLineSegment.h"
    "/emphaticsoft/StandardRecord/SRLineSegmentBranch.h"
    "/emphaticsoft/StandardRecord/SRParticle.h"
    "/emphaticsoft/StandardRecord/SRSSDClust.h"
    "/emphaticsoft/StandardRecord/SRSSDClustBranch.h"
    "/emphaticsoft/StandardRecord/SRSSDHits.h"
    "/emphaticsoft/StandardRecord/SRSSDHitsBranch.h"
    "/emphaticsoft/StandardRecord/SRSpacePoint.h"
    "/emphaticsoft/StandardRecord/SRSpacePointBranch.h"
    "/emphaticsoft/StandardRecord/SRTrack.h"
    "/emphaticsoft/StandardRecord/SRTrackBranch.h"
    "/emphaticsoft/StandardRecord/SRTrackSegment.h"
    "/emphaticsoft/StandardRecord/SRTrackSegmentBranch.h"
    "/emphaticsoft/StandardRecord/SRTrajectory.h"
    "/emphaticsoft/StandardRecord/SRTrueParticle.h"
    "/emphaticsoft/StandardRecord/SRTrueSSDHits.h"
    "/emphaticsoft/StandardRecord/SRTrueSSDHitsBranch.h"
    "/emphaticsoft/StandardRecord/SRTruth.h"
    "/emphaticsoft/StandardRecord/SRVector3D.h"
    "/emphaticsoft/StandardRecord/StandardRecord.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/StandardRecord" TYPE FILE FILES
    "/emphaticsoft/StandardRecord/SRARing.cxx"
    "/emphaticsoft/StandardRecord/SRARing.h"
    "/emphaticsoft/StandardRecord/SRARingBranch.cxx"
    "/emphaticsoft/StandardRecord/SRARingBranch.h"
    "/emphaticsoft/StandardRecord/SRBACkov.cxx"
    "/emphaticsoft/StandardRecord/SRBACkov.h"
    "/emphaticsoft/StandardRecord/SRBACkovBranch.cxx"
    "/emphaticsoft/StandardRecord/SRBACkovBranch.h"
    "/emphaticsoft/StandardRecord/SREventQual.cxx"
    "/emphaticsoft/StandardRecord/SREventQual.h"
    "/emphaticsoft/StandardRecord/SRGasCkov.cxx"
    "/emphaticsoft/StandardRecord/SRGasCkov.h"
    "/emphaticsoft/StandardRecord/SRGasCkovBranch.cxx"
    "/emphaticsoft/StandardRecord/SRGasCkovBranch.h"
    "/emphaticsoft/StandardRecord/SRHeader.cxx"
    "/emphaticsoft/StandardRecord/SRHeader.h"
    "/emphaticsoft/StandardRecord/SRLineSegment.cxx"
    "/emphaticsoft/StandardRecord/SRLineSegment.h"
    "/emphaticsoft/StandardRecord/SRLineSegmentBranch.cxx"
    "/emphaticsoft/StandardRecord/SRLineSegmentBranch.h"
    "/emphaticsoft/StandardRecord/SRParticle.cxx"
    "/emphaticsoft/StandardRecord/SRParticle.h"
    "/emphaticsoft/StandardRecord/SRSSDClust.cxx"
    "/emphaticsoft/StandardRecord/SRSSDClust.h"
    "/emphaticsoft/StandardRecord/SRSSDClustBranch.cxx"
    "/emphaticsoft/StandardRecord/SRSSDClustBranch.h"
    "/emphaticsoft/StandardRecord/SRSSDHits.cxx"
    "/emphaticsoft/StandardRecord/SRSSDHits.h"
    "/emphaticsoft/StandardRecord/SRSSDHitsBranch.cxx"
    "/emphaticsoft/StandardRecord/SRSSDHitsBranch.h"
    "/emphaticsoft/StandardRecord/SRSpacePoint.cxx"
    "/emphaticsoft/StandardRecord/SRSpacePoint.h"
    "/emphaticsoft/StandardRecord/SRSpacePointBranch.cxx"
    "/emphaticsoft/StandardRecord/SRSpacePointBranch.h"
    "/emphaticsoft/StandardRecord/SRTrack.cxx"
    "/emphaticsoft/StandardRecord/SRTrack.h"
    "/emphaticsoft/StandardRecord/SRTrackBranch.cxx"
    "/emphaticsoft/StandardRecord/SRTrackBranch.h"
    "/emphaticsoft/StandardRecord/SRTrackSegment.cxx"
    "/emphaticsoft/StandardRecord/SRTrackSegment.h"
    "/emphaticsoft/StandardRecord/SRTrackSegmentBranch.cxx"
    "/emphaticsoft/StandardRecord/SRTrackSegmentBranch.h"
    "/emphaticsoft/StandardRecord/SRTrajectory.cxx"
    "/emphaticsoft/StandardRecord/SRTrajectory.h"
    "/emphaticsoft/StandardRecord/SRTrueParticle.cxx"
    "/emphaticsoft/StandardRecord/SRTrueParticle.h"
    "/emphaticsoft/StandardRecord/SRTrueSSDHits.cxx"
    "/emphaticsoft/StandardRecord/SRTrueSSDHits.h"
    "/emphaticsoft/StandardRecord/SRTrueSSDHitsBranch.cxx"
    "/emphaticsoft/StandardRecord/SRTrueSSDHitsBranch.h"
    "/emphaticsoft/StandardRecord/SRTruth.cxx"
    "/emphaticsoft/StandardRecord/SRTruth.h"
    "/emphaticsoft/StandardRecord/SRVector3D.cxx"
    "/emphaticsoft/StandardRecord/SRVector3D.h"
    "/emphaticsoft/StandardRecord/StandardRecord.cxx"
    "/emphaticsoft/StandardRecord/StandardRecord.h"
    "/emphaticsoft/StandardRecord/classes.h"
    "/emphaticsoft/StandardRecord/classes_def.xml"
    )
endif()

