# Install script for directory: /emphaticsoft/MagneticField/field_map_tools

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib" TYPE SHARED_LIBRARY FILES "/build/slf7.x86_64.e20.prof/lib/libfield_map_tools.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so"
         OLD_RPATH "/cvmfs/emphatic.opensciencegrid.org/products/root/v6_26_06/Linux64bit+3.10-2.17-e20-p3913-prof/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/libfield_map_tools.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/MagneticField/field_map_tools" TYPE FILE FILES
    "/emphaticsoft/MagneticField/field_map_tools/FieldMap.h"
    "/emphaticsoft/MagneticField/field_map_tools/ProbeCalib.h"
    "/emphaticsoft/MagneticField/field_map_tools/filehamna.h"
    "/emphaticsoft/MagneticField/field_map_tools/funcs.h"
    "/emphaticsoft/MagneticField/field_map_tools/inputtextfile.h"
    "/emphaticsoft/MagneticField/field_map_tools/polyMint.h"
    "/emphaticsoft/MagneticField/field_map_tools/quadd.h"
    "/emphaticsoft/MagneticField/field_map_tools/ra.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/MagneticField/field_map_tools" TYPE FILE FILES
    "/emphaticsoft/MagneticField/field_map_tools/FieldMap.cxx"
    "/emphaticsoft/MagneticField/field_map_tools/FieldMap.h"
    "/emphaticsoft/MagneticField/field_map_tools/ProbeCalib.cxx"
    "/emphaticsoft/MagneticField/field_map_tools/ProbeCalib.h"
    "/emphaticsoft/MagneticField/field_map_tools/filehamna.h"
    "/emphaticsoft/MagneticField/field_map_tools/funcs.h"
    "/emphaticsoft/MagneticField/field_map_tools/inputtextfile.cpp"
    "/emphaticsoft/MagneticField/field_map_tools/inputtextfile.h"
    "/emphaticsoft/MagneticField/field_map_tools/polyMint.cpp"
    "/emphaticsoft/MagneticField/field_map_tools/polyMint.h"
    "/emphaticsoft/MagneticField/field_map_tools/quadd.cpp"
    "/emphaticsoft/MagneticField/field_map_tools/quadd.h"
    "/emphaticsoft/MagneticField/field_map_tools/ra.h"
    )
endif()

