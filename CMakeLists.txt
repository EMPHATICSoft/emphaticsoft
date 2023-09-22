cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(emphaticsoft VERSION 2.00.00 LANGUAGES CXX C)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  #WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-parameter -Wno-unused-variable -Wno-dev
)

#cet_enable_asserts()

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

set(CMAKE_MODULE_PATH $ENV{NUFINDER_DIR}/Modules
		      ${CMAKE_CURRENT_SOURCE_DIR}/Modules
		      ${CMAKE_MODULE_PATH}
)

MESSAGE(VERBOSE "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")

add_definitions( -D_REENTRANT ) # needed for stan_math v4

# packages pulled in by cetmodules
find_package( libwda         2.20.1 REQUIRED )
find_package( art REQUIRED)
find_package( artdaq REQUIRED)
find_package( canvas_root_io REQUIRED)
find_package( art_root_io REQUIRED)
find_package( ifdh_art       2.15.00 REQUIRED)

# macros for artdaq_dictionary and cet_build_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(ARICHReco)
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
add_subdirectory(MagneticField)
add_subdirectory(MCValidation)
add_subdirectory(OnlineMonitoring)
add_subdirectory(RawData)
add_subdirectory(RawDataMerger)
add_subdirectory(RawDataUnpacker)
add_subdirectory(RecoBase)
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
add_subdirectory(TOF)
add_subdirectory(Utilities)

# ups - table and config files
#add_subdirectory(ups)

cet_cmake_config()
