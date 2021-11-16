add_subdirectory(GPS)
add_subdirectory(Bern)

include_directories( ./GPS )

add_definitions(-DLINUX)
link_directories($ENV{CAENCOMM_LIB})
link_directories($ENV{CAENVME_LIB})
link_directories($ENV{CAENDIGITIZER_LIB})


cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_Utilities
  SOURCE
    workerThread.cpp
    CircularBuffer.cc
    PoolBuffer.cc
  LIBRARIES
    pthread
    artdaq_DAQdata
    ${MF_UTILITIES}
    MF_MessageLogger
    ${Boost_SYSTEM_LIBRARY}
)

list(APPEND LIBRARY_CORE_LIB_LIST sbndaq-artdaq_Common_Utilities )

cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_CAENV1730_GeneratorBase
  SOURCE
    CAENDecoder.cc
    CAENConfiguration.cc
    CAENException.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
    CAENComm
    CAENVME
    CAENDigitizer
)

cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_SpectraTime_GeneratorBase
  SOURCE
    SpectraTimeReadout_generator.cc
    GPS/GPSInfo.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
)


cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_WhiteRabbit_GeneratorBase
  SOURCE
    WhiteRabbitReadout_generator.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
)

cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_BernCRTFEBConfiguration
  SOURCE
    BernCRTFEBConfiguration.cc
  LIBRARIES
    fhiclcpp
    cetlib_except
)

cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_BernCRTZMQ_GeneratorBase
  SOURCE
     BernCRTZMQ_GeneratorBase.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
    sbndaq_artdaq_core::sbndaq-artdaq-core_Overlays_Common
    sbndaq-artdaq_Common_BernCRTFEBConfiguration
)

cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq_Common_BernCRT_GeneratorBase
  SOURCE
    BernCRT_GeneratorBase.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
    sbndaq_artdaq_core::sbndaq-artdaq-core_Overlays_Common
    sbndaq-artdaq_Common_BernCRTFEBConfiguration
)

cet_make_library(
  LIBRARY_NAME
    febdrv
  SOURCE
    febdrv.cc
  LIBRARIES
    sbndaq_artdaq_core::sbndaq-artdaq-core_Overlays_Common
    fhiclcpp
    cetlib_except
)


basic_plugin(CAENV1730Readout "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
    sbndaq-artdaq_Common_CAENV1730_GeneratorBase
    CAENComm
    CAENVME
    CAENDigitizer
  )

basic_plugin(SpectraTimeReadout "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
    sbndaq-artdaq_Common_SpectraTime_GeneratorBase
)

basic_plugin(WhiteRabbitReadout "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
    sbndaq-artdaq_Common_WhiteRabbit_GeneratorBase
)

basic_plugin(BernCRTZMQData "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
    sbndaq-artdaq_Common_BernCRTZMQ_GeneratorBase
    zmq
    febdrv
)

basic_plugin(BernCRTData "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
    sbndaq-artdaq_Common_BernCRT_GeneratorBase
    zmq
    febdrv
)

basic_plugin(DummyGenerator "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
)

cet_make_exec( ConvertCRTConfiguration
  SOURCE
    ConvertCRTConfiguration.cc
  LIBRARIES
    sbndaq-artdaq_Common_BernCRTFEBConfiguration
    fhiclcpp
    cetlib
    cetlib_except
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


install_source()
install_headers()
