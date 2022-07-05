# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (Merger)


# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
#find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
#include(${ROOT_USE_FILE})

# Define other includes
include_directories(${PROJECT_SOURCE_DIR})

# Add executable called "helloDemo" that is built from the source files
add_executable (Splitter Splitter.cpp FilesSplitter.cpp Hit.cpp Event.cpp DataDecoder.cpp Run.cpp PxEvent.cpp stib.cpp)
add_executable (Merger Merger.cpp FilesMerger.cpp Hit.cpp Event.cpp DataDecoder.cpp Run.cpp PxEvent.cpp stib.cpp)
#target_link_libraries(Splitter ${ROOT_LIBRARIES})
#target_link_libraries(Merger ${ROOT_LIBRARIES})
