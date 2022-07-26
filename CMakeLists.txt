cmake_minimum_required(VERSION 3.18.2 FATAL_ERROR)

find_package(cetmodules 2.08.00 REQUIRED)
project(emphaticsoft VERSION 0.01.00 LANGUAGES CXX C)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

cet_find_package(art REQUIRED)
cet_find_package(artdaq REQUIRED)
cet_find_package(canvas_root_io REQUIRED)
cet_find_package(art_root_io REQUIRED)

# source
add_subdirectory(Geometry)
add_subdirectory(ChannelMap)
add_subdirectory(RawData)
add_subdirectory(RecoBase)
add_subdirectory(Demo)
add_subdirectory(RunHistory)
add_subdirectory(SimulationBase)
add_subdirectory(emphatic-artdaq)
add_subdirectory(OnlineMonitoring)
add_subdirectory(RawDataMerger)
add_subdirectory(RawDataUnpacker)
add_subdirectory(MagneticField)
add_subdirectory(EventDisplayBase)
add_subdirectory(EventDisplayServices)
add_subdirectory(EventDisplay)
add_subdirectory(G4Base)
add_subdirectory(Simulation)
add_subdirectory(G4EMPH)
add_subdirectory(MCValidation)
