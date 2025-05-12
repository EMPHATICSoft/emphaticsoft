# Install script for directory: /emphaticsoft

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
  # Tweak the value of CMAKE_INSTALL_PREFIX used by the project's
  # cmake_install.cmake files per UPS conventions.
  string(APPEND CMAKE_INSTALL_PREFIX "/emphaticsoft/v2_00_00")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "/emphaticsoft/LICENSE"
    "/emphaticsoft/README.md"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/Align/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/ADCReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/ADCUtils/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/ARICHReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/BACkovHitReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/CAFMaker/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/ChannelMap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/ChannelState/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/DataQuality/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/Demo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/DetGeoMap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/Digitization/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/emphatic-artdaq/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/EventDisplay/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/G4Base/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/G4EMPH/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/GasCherenkov/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/GasCkovHitReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/Geometry/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/LGCalo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/MagneticField/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/MCValidation/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/MomReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/OnlineMonitoring/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/RawData/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/RawDataMerger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/RawDataUnpacker/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/RecoBase/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/RecoUtils/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/ReadoutWaveform/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/RunHistory/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SignalTime/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/Simulation/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SimulationBase/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SpillInfo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/StandardRecord/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SSDCalibration/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SSDAlignment/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SSDReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/SSD_FERMerger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/TrackReco/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/TOF/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/build/Utilities/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/ups" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/ups/emphaticsoft.table")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/../v2_00_00.version" TYPE FILE FILES "/build/slf7.x86_64.e20.prof/ups/Linux64bit+6.12-2.17_e20_prof")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  # Handle placeholders in target definitions.
  file(READ "/build/CMakeFiles/Export/72f7585649c0463f3cbc72723fb78777/emphaticsoftTargets.cmake" _targetFileData)
  string(REPLACE "@CET_DOLLAR@" "$" _targetFileData_new "${_targetFileData}")
  if (NOT _targetFileData_new STREQUAL _targetFileData)
    file(WRITE "/build/CMakeFiles/Export/72f7585649c0463f3cbc72723fb78777/emphaticsoftTargets.cmake" "${_targetFileData_new}")
  endif()
  if (NOT "" STREQUAL "x")
    # Check for unwanted absolute transitive dependencies.
    include("/cvmfs/emphatic.opensciencegrid.org/products/cetmodules/v3_20_00/Modules/private/CetFindAbsoluteTransitiveDependencies.cmake")
    _cet_find_absolute_transitive_dependencies("/build/CMakeFiles/Export/72f7585649c0463f3cbc72723fb78777/emphaticsoftTargets.cmake"
       "${_targetFileData_new}"
       "")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake/emphaticsoftTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake/emphaticsoftTargets.cmake"
         "/build/CMakeFiles/Export/72f7585649c0463f3cbc72723fb78777/emphaticsoftTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake/emphaticsoftTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake/emphaticsoftTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake" TYPE FILE FILES "/build/CMakeFiles/Export/72f7585649c0463f3cbc72723fb78777/emphaticsoftTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake" TYPE FILE FILES "/build/CMakeFiles/Export/72f7585649c0463f3cbc72723fb78777/emphaticsoftTargets-relwithdebinfo.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  # Handle placeholders in target definitions.
  file(READ "/build/genConfig/emphaticsoftConfig.cmake" _targetFileData)
  string(REPLACE "@CET_DOLLAR@" "$" _targetFileData_new "${_targetFileData}")
  if (NOT _targetFileData_new STREQUAL _targetFileData)
    file(WRITE "/build/genConfig/emphaticsoftConfig.cmake" "${_targetFileData_new}")
  endif()
  if (NOT "" STREQUAL "x")
    # Check for unwanted absolute transitive dependencies.
    include("/cvmfs/emphatic.opensciencegrid.org/products/cetmodules/v3_20_00/Modules/private/CetFindAbsoluteTransitiveDependencies.cmake")
    _cet_find_absolute_transitive_dependencies("/build/genConfig/emphaticsoftConfig.cmake"
       "${_targetFileData_new}"
       "")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/slf7.x86_64.e20.prof/lib/emphaticsoft/cmake" TYPE FILE FILES
    "/build/genConfig/emphaticsoftConfig.cmake"
    "/build/emphaticsoftConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  # Detect misplaced installs from older, cetbuildtools-using packages.
  if (IS_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/emphaticsoft/v2_00_00")
    message(STATUS "tidying legacy installations: relocate emphaticsoft/v2_00_00/*")
    execute_process(COMMAND /cvmfs/emphatic.opensciencegrid.org/products/cmake/v3_24_1/Linux64bit+3.10-2.17/bin/cmake -E tar c "../../emphaticsoft_v2_00_00-tmpinstall.tar" .
                    WORKING_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/emphaticsoft/v2_00_00"
                    COMMAND_ERROR_IS_FATAL ANY)
    file(REMOVE_RECURSE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/emphaticsoft/v2_00_00")
    execute_process(COMMAND /cvmfs/emphatic.opensciencegrid.org/products/cmake/v3_24_1/Linux64bit+3.10-2.17/bin/cmake -E tar xv "emphaticsoft_v2_00_00-tmpinstall.tar"
                    WORKING_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}"
                    OUTPUT_VARIABLE _cet_install_emphaticsoft_legacy
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND rmdir "emphaticsoft"
                    WORKING_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}"
                    OUTPUT_QUIET
                    ERROR_QUIET)
    message(STATUS "in $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}: ${_cet_install_emphaticsoft_legacy}")
    unset(_cet_install_emphaticsoft_legacy)
    file(REMOVE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/emphaticsoft_v2_00_00-tmpinstall.tar")
  endif()

  # We need to reset CMAKE_INSTALL_PREFIX to its original value at this
  # time.
  get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" DIRECTORY)
  get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" DIRECTORY)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(TRANSFORM CMAKE_INSTALL_MANIFEST_FILES REPLACE "/emphaticsoft/v2_00_00/emphaticsoft/v2_00_00" "/emphaticsoft/v2_00_00")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
