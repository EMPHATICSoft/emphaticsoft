cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(emphaticsoft VERSION 6.00.00 LANGUAGES CXX C)

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

# packages pulled in by cetmodules
find_package( libwda          2.20.1      REQUIRED )
find_package( art 	     	       	  REQUIRED EXPORT)
find_package( artdaq 		       	  REQUIRED EXPORT)
find_package( artdaq_core 		  REQUIRED EXPORT)
find_package( canvas_root_io 	       	  REQUIRED EXPORT)
find_package( art_root_io 	       	  REQUIRED EXPORT)
find_package( fhiclcpp 	     	       	  REQUIRED EXPORT)
find_package( cetlib                      REQUIRED EXPORT)
find_package( cetlib_except               REQUIRED EXPORT)
find_package( TRACE                       REQUIRED )
find_package( nlohmann_json		  REQUIRED )
find_package( millepede_ii      REQUIRED )
find_package( ifdh_art        2.15.00  	  REQUIRED )
find_package( messagefacility 2.08.04  	  REQUIRED )
find_package( ROOT            6.22.08d    REQUIRED COMPONENTS Core EG Eve Geom Gdml Gpad Graf Graf3d Gui Hist Matrix MathCore MathMore Minuit Minuit2 Net Physics RIO Spectrum TMVA Thread Tree TreePlayer X3d XMLIO )
cmake_policy(PUSH)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE BOOL "Suppress dev warnings")
find_package( Geant4          4.11.0.p03b REQUIRED )
cmake_policy(POP)
find_package( python         3.9.15    	  REQUIRED )
set( PYTHON_EXECUTABLE $ENV{PYTHONHOME}/bin/python )

# macros for artdaq_dictionary and cet_build_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(Align)
add_subdirectory(Alignment)
add_subdirectory(ADCReco)
add_subdirectory(ADCUtils)
add_subdirectory(ARICHReco)
add_subdirectory(ARICHML)
add_subdirectory(ARICHRecoUtils)
add_subdirectory(BACkovHitReco)
add_subdirectory(CAFMaker)
add_subdirectory(ChannelMap)
add_subdirectory(ChannelState)
add_subdirectory(DataQuality)
add_subdirectory(Demo)
add_subdirectory(DetGeoMap)
add_subdirectory(Digitization)
add_subdirectory(emphatic-artdaq)
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
add_subdirectory(MomReco)
add_subdirectory(OnlineMonitoring)
add_subdirectory(RawData)
add_subdirectory(RawDataMerger)
add_subdirectory(RawDataUnpacker)
add_subdirectory(RecoBase)
add_subdirectory(RecoUtils)
add_subdirectory(ReadoutWaveform)
add_subdirectory(RunHistory)
add_subdirectory(SignalTime)
add_subdirectory(Simulation)
add_subdirectory(SimulationBase)
add_subdirectory(SpillInfo)
add_subdirectory(StandardRecord)
add_subdirectory(SSDCalibration)
add_subdirectory(SSDAlignment)
add_subdirectory(SSDReco)
add_subdirectory(SSD_FERMerger)
add_subdirectory(TrackReco)
add_subdirectory(TOF)
add_subdirectory(Utilities)

# ups - table and config files

cet_cmake_config()
