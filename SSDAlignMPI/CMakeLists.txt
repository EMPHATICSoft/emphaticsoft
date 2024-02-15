cmake_minimum_required(VERSION 3.2.0)
#
#SET(CMAKE_C_COMPILER "/usr/lib64/openmpi/bin/mpicxx")
SET(CMAKE_CXX_COMPILER "/usr/lib64/openmpi/bin/mpicxx")
SET(CMAKE_CXX_LINKER "/usr/lib64/openmpi/bin/mpicxx")
#
IF(MPI_CXX_FOUND)
        INCLUDE_DIRECTORIES(${MPI_CXX_INCLUDE_PATH})
        LIST(APPEND SCR_EXTERNAL_LIBS ${MPI_CXX_LIBRARIES})
ENDIF(MPI_CXX_FOUND)
#
project(SSDAlignMPI)

#install by default in source directory
#SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# activate c++11 features
SET(CMAKE_CXX_FLAGS "-g -std=c++14")
#set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
#
# Find source files
file(GLOB SOURCES *.cxx)
# Find header files
file(GLOB HEADERS *.h *.hh)

#list(REMOVE_ITEM SOURCES LPCint.cc)
#list(REMOVE_ITEM HEADERS LPCint.h)

# Include header files
#include_directories(./ $ENV{FFTW_INC})

include_directories(../src /usr/include/openmpi-x86_64/ /home/lebrun/Minuit2/Minuit2-master/inc/ )
#include_directories(../src /opt/ohpc/pub/mpi/openmpi3-gnu8/3.1.4/include /work1/next/lebrun/Minuit2/Minuit2-master/inc/)  for wc cluster 

# path to external libs
#link_directories(/opt/ohpc/pub/mpi/openmpi3-gnu8/3.1.4/lib/)
#link_directories(/opt/ohpc/pub/libs/gnu8/hdf5/1.10.5/lib/)
#link_directories(/opt/ohpc/pub/libs/gnu8/gsl/2.6/lib/)
link_directories(/usr/lib64/openmpi/lib/)
link_directories(/usr/lib64/openmpi/lib/openmpi/)
link_directories(/home/lebrun/Minuit2/Minuit2-master/lib)

# Create shared library 
add_library(${PROJECT_NAME} SHARED BeamTrackCluster.cxx BTAlignInput.cxx myMPIUtils.cxx BTAlignGeom.cxx BeamTrackSSDAlignFCN.cxx BTAlignGeom1c.cxx 
                 BeamTrack.cxx BeamTracks.cxx BeamTrack2DFCN.cxx BeamTrack3DFCN.cxx SSDAlignParam.cxx 
		 SSDAlignParams.cxx BTMagneticField.cxx  BTMagneticFieldAdd.cxx )

# link library
target_link_libraries(${PROJECT_NAME}  mpi_cxx mpi Minuit2 Minuit2Math GaussSim)

#add_executable(BTDist1 BTDist1.cc)
#target_link_libraries(BTDist1 ${PROJECT_NAME} mpi mpi_cxx mpi  )
#
add_executable(BTTestGeom BTTestGeom.cc)
target_link_libraries(BTTestGeom ${PROJECT_NAME} mpi mpi_cxx mpi  )

add_executable(BTTestParams BTTestParams.cc)
target_link_libraries(BTTestParams ${PROJECT_NAME} mpi mpi_cxx mpi Minuit2 Minuit2Math GaussSim   )

#add_executable(BTFitA BTFitA.cc)
#target_link_libraries(BTFitA ${PROJECT_NAME} mpi mpi_cxx mpi Minuit2 Minuit2Math GaussSim  )

#add_executable(BTFit2ndOrderMigradOnly BTFit2ndOrderMigradOnly.cc)
#target_link_libraries(BTFit2ndOrderMigradOnly ${PROJECT_NAME} mpi mpi_cxx mpi Minuit2 Minuit2Math GaussSim  )

add_executable(BTFitSimplex BTFitSimplex.cc)
target_link_libraries(BTFitSimplex ${PROJECT_NAME} mpi mpi_cxx mpi Minuit2 Minuit2Math GaussSim  )
#
add_executable(GFitMigrad GFitMigrad.cc)
target_link_libraries(GFitMigrad ${PROJECT_NAME} mpi mpi_cxx mpi Minuit2 Minuit2Math GaussSim  )
#
add_executable(testStatusMPI testStatusMPI.cc)
target_link_libraries(testStatusMPI mpi mpi_cxx mpi)
#
