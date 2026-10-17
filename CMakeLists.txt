cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(emphaticsoft VERSION 7.04.00 LANGUAGES CXX C)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  #WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-parameter -Wno-unused-variable
)

cet_enable_asserts()

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules
		      $ENV{ARTDAQ_DIR}/Modules
		      $ENV{NUFINDER_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

MESSAGE(VERBOSE "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")

add_definitions( -D_REENTRANT ) # needed for stan_math v4

# Spackification subsystem switches (spack-dev branch only; main builds
# everything unconditionally). Defaults match the emphaticsoft spack package
# variants: ~daq +beamdb +align ~ml ~caen.
option(EMPH_BUILD_DAQ "Find the full artdaq stack, for online components (emphatic-artdaq/ArtModules, not built at present). Raw data unpacking, merging and WebDisplay need only artdaq-core and TRACE and are always built" OFF)
option(EMPH_BEAMDB "Build beam/conditions DB components (ifdh_art: CAFMaker, SpillInfo)" ON)
option(EMPH_ALIGN "Build Millepede-II alignment (Alignment)" ON)
option(EMPH_BUILD_ML "Build libtorch ML components (TorchML, ARICHReco)" OFF)
option(EMPH_CAEN "Reserved: CAEN hardware support (no spack recipes yet)" OFF)

# packages pulled in by cetmodules
find_package( libwda          2.20.1      REQUIRED ) # core: ChannelState, RunHistory
find_package( art 	     	       	        REQUIRED EXPORT)
# artdaq_core carries the raw-data product definitions used by offline code
# (RawData) and the unpackers; only the full artdaq stack is DAQ-gated.
find_package( artdaq_core 		            REQUIRED EXPORT)
find_package( TRACE                       REQUIRED ) # RawDataMerger
if(EMPH_BUILD_DAQ)
  find_package( artdaq 		       	        REQUIRED EXPORT)
endif()
find_package( canvas_root_io 	       	    REQUIRED EXPORT)
find_package( art_root_io 	       	      REQUIRED EXPORT)
find_package( fhiclcpp 	     	       	    REQUIRED EXPORT)
find_package( cetlib                      REQUIRED EXPORT)
find_package( cetlib_except               REQUIRED EXPORT)
find_package( nlohmann_json               REQUIRED )
if(EMPH_ALIGN)
  find_package( millepede_ii              REQUIRED )
endif()
if(EMPH_BEAMDB)
  find_package( ifdh_art      2.15.00  	  REQUIRED )
endif()
find_package( messagefacility 2.08.04  	  REQUIRED )
find_package( ROOT            6.22.08d    REQUIRED COMPONENTS Core EG Eve Geom Gdml Gpad Graf Graf3d Gui Hist Matrix MathCore MathMore Minuit Minuit2 Net Physics RIO Spectrum TMVA Thread Tree TreePlayer X3d XMLIO )
cmake_policy(PUSH)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "Suppress dev warnings")
find_package( Geant4          4.11.0.p03b REQUIRED )
cmake_policy(POP)
# G4OpenGL and G4zlib exist in the UPS Geant4 but not in a Spack geant4
# built ~opengl with an external zlib. Nothing includes their headers.
set(EMPH_G4_OPTIONAL_LIBS)
foreach(_g4lib G4OpenGL G4zlib)
  if(TARGET Geant4::${_g4lib})
    list(APPEND EMPH_G4_OPTIONAL_LIBS Geant4::${_g4lib})
  endif()
endforeach()
if(DEFINED ENV{PYTHON_INCLUDE}) # UPS: Modules/Findpython.cmake reads PYTHON_INCLUDE/PYTHON_LIB
  find_package( python         3.9.15    	  REQUIRED )
  set( PYTHON_EXECUTABLE $ENV{PYTHONHOME}/bin/python )
else() # spack: no UPS environment, use CMake's own module
  find_package( Python3 3.9 REQUIRED COMPONENTS Interpreter Development )
  set( PYTHON_EXECUTABLE ${Python3_EXECUTABLE} )
endif()

# packages for ML
find_package( protobuf                    REQUIRED )
if(EMPH_BUILD_ML)
  find_package( libtorch                  REQUIRED )
  # A CUDA libtorch puts absolute paths (libc10.so, libc10_cuda.so, CUDA's
  # libnvrtc.so) in TORCH_LIBRARIES, so TorchML's exported link interface has
  # them and cet_cmake_config() stops the install. Nothing builds against
  # emphaticsoft's exported targets, so accept them.
  set(${CETMODULES_CURRENT_PROJECT_NAME}_IGNORE_ABSOLUTE_TRANSITIVE_DEPENDENCIES_REGEX
    "/(py-torch|cuda)-[^/]+/")
endif()

# macros for artdaq_dictionary and cet_build_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(Align)
if(EMPH_ALIGN)
  add_subdirectory(Alignment)
endif()
add_subdirectory(ADCReco)
add_subdirectory(ADCUtils)
add_subdirectory(ARICHReco) # the ARICHReco plugin itself needs EMPH_BUILD_ML (TorchML)
add_subdirectory(ARICHRecoUtils)
add_subdirectory(BACkovHitReco)
if(EMPH_BEAMDB)
  add_subdirectory(CAFMaker) # IFDH_service
endif()
add_subdirectory(ChannelMap)
add_subdirectory(ChannelState)
add_subdirectory(DataQuality)
add_subdirectory(Demo)
add_subdirectory(DetGeoMap)
add_subdirectory(Digitization)
add_subdirectory(emphatic-artdaq) # fragment overlays: artdaq-core only
add_subdirectory(EventDisplay)
add_subdirectory(G4Base)
add_subdirectory(G4EMPH)
add_subdirectory(GasCherenkov)
add_subdirectory(GasCkovHitReco)
add_subdirectory(Geometry)
add_subdirectory(KalmanReco)
add_subdirectory(LGCalo)
add_subdirectory(MagneticField)
add_subdirectory(MCUtils)
add_subdirectory(MCValidation)
add_subdirectory(MLUtils)
add_subdirectory(MomReco)
add_subdirectory(OnlineMonitoring)
add_subdirectory(RawData)
add_subdirectory(RawDataUnpacker) # emphatic-artdaq_Overlays, artdaq-core
add_subdirectory(RawDataMerger)   # emphatic-artdaq_Overlays, TRACE
add_subdirectory(RecoBase)
add_subdirectory(RecoUtils)
add_subdirectory(ReadoutWaveform)
add_subdirectory(RunHistory)
add_subdirectory(SignalTime)
add_subdirectory(Simulation)
add_subdirectory(SimulationBase)
if(EMPH_BEAMDB)
  add_subdirectory(SpillInfo) # ifbeam, IFBeam_service
endif()
add_subdirectory(StandardRecord)
add_subdirectory(SSDCalibration)
add_subdirectory(SSDAlignment)
add_subdirectory(SSDEfficiency)
add_subdirectory(SSDReco)
add_subdirectory(SSD_FERMerger)
add_subdirectory(TrackReco)
if(EMPH_BUILD_ML)
  add_subdirectory(TorchML)
endif()
add_subdirectory(TOF)
add_subdirectory(Utilities)
add_subdirectory(VertexReco)

# Runtime data that modules open through $CETPKG_SOURCE (geometry, channel maps,
# calibrations, bad channels, ...). Install it under <prefix>/source with the same
# layout as the source tree, next to what install_source() puts there, so an
# installed release can point CETPKG_SOURCE at that directory (the Spack recipe does).
set(EMPH_RUNTIME_DATA_DIR ${${CETMODULES_CURRENT_PROJECT_NAME}_INSTALLED_SOURCE_DIR})
if(NOT EMPH_RUNTIME_DATA_DIR)
  set(EMPH_RUNTIME_DATA_DIR source)
endif()
install(DIRECTORY ConstBase DESTINATION ${EMPH_RUNTIME_DATA_DIR})
install(DIRECTORY ARICHRecoUtils/data DESTINATION ${EMPH_RUNTIME_DATA_DIR}/ARICHRecoUtils)
install(FILES ARICHReco/PMTDet.root DESTINATION ${EMPH_RUNTIME_DATA_DIR}/ARICHReco)
if(EMPH_BUILD_ML)
  install(FILES TorchML/arich_model.pt DESTINATION ${EMPH_RUNTIME_DATA_DIR}/TorchML)
endif()
install(FILES DataQuality/GoodRunsList.txt DataQuality/GoodRunsList_SSD_phase1c.txt
  DESTINATION ${EMPH_RUNTIME_DATA_DIR}/DataQuality)
install(FILES Digitization/Hits_totADC_RMS_3D.root Digitization/dEtoADCmap.root
  Digitization/arich_noise_map_absolute_xy.root
  DESTINATION ${EMPH_RUNTIME_DATA_DIR}/Digitization)
install(FILES G4EMPH/g4emph.mac DESTINATION ${EMPH_RUNTIME_DATA_DIR}/G4EMPH)
install(DIRECTORY OnlineMonitoring/util/ DESTINATION ${EMPH_RUNTIME_DATA_DIR}/OnlineMonitoring/util
  FILES_MATCHING PATTERN "*.csv")
install(DIRECTORY OnlineMonitoring/viewer/ DESTINATION ${EMPH_RUNTIME_DATA_DIR}/OnlineMonitoring/viewer
  FILES_MATCHING PATTERN "*.png")

# ups - table and config files
cet_cmake_config()
