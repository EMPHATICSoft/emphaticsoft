cmake_minimum_required(VERSION 3.18.2 FATAL_ERROR)

find_package(cetmodules 3.20.00 REQUIRED)
project(emphaticsoft VERSION 1.02.00 LANGUAGES CXX C)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

find_package(art REQUIRED)
find_package(artdaq REQUIRED)
find_package(canvas_root_io REQUIRED)
find_package(art_root_io REQUIRED)

# source
add_subdirectory(ARICHReco)
add_subdirectory(BACkovHitReco)
add_subdirectory(ChannelMap)
add_subdirectory(DataQuality)
add_subdirectory(Demo)
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
add_subdirectory(Simulation)
add_subdirectory(SimulationBase)
add_subdirectory(SpillInfo)
add_subdirectory(SSDCalibration)
add_subdirectory(SSDAlignment)
add_subdirectory(SSDReco)
add_subdirectory(SSD_FERMerger)
add_subdirectory(TOF)
