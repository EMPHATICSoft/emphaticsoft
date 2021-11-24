set ( LIBRARY_CORE_LIB_LIST
      artdaq_DAQdata
      artdaq_Generators
      artdaq-utilities_Plugins
      artdaq-core_Utilities
      artdaq-core_Data
      ${Boost_SYSTEM_LIBRARY}
      art::Utilities
      fhiclcpp::fhiclcpp
      messagefacility::MF_MessageLogger
      cetlib::cetlib
      cetlib_except
)

set(BUILD_PLUGIN_CORE_LIB_LIST
  ${LIBRARY_CORE_LIB_LIST}
  artdaq_Application
  emphatic-artdaq_Overlays
)

include(ArtDictionary)
include(ArtMake)

add_definitions(-DLINUX)
link_directories($ENV{CAENCOMM_LIB})
link_directories($ENV{CAENVME_LIB})
link_directories($ENV{CAENDIGITIZER_LIB})
link_directories($ENV{ARTDAQ_LIB})
link_directories($ENV{ARTDAQ_CORE_LIB})
link_directories($ENV{ARTDAQ_UTILITIES_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})

include_directories($ENV{ARTDAQ_INC})
include_directories($ENV{ARTDAQ_UTILITIES_INC})
include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{CAENCOMM_INC})
include_directories($ENV{CAENDIGITIZER_INC})
include_directories($ENV{CAENVME_INC})
include_directories($ENV{FHICLCPP_INC})
include_directories($ENV{TRACE_INC})

cet_make_library(
  LIBRARY_NAME
    emphatic-artdaq_Utilities
  SOURCE
    workerThread.cpp
    #CircularBuffer.cc
    PoolBuffer.cc
  LIBRARIES
    pthread
    artdaq_DAQdata
    ${MF_UTILITIES}
    messagefacility::MF_MessageLogger
    ${Boost_SYSTEM_LIBRARY}
)

list(APPEND LIBRARY_CORE_LIB_LIST emphatic-artdaq_Utilities )

cet_make_library(
  LIBRARY_NAME
    emphatic-artdaq_CAENV1730_GeneratorBase
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


basic_plugin(CAENV1730Readout "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
    emphatic-artdaq_CAENV1730_GeneratorBase
    CAENComm
    CAENVME
    CAENDigitizer
  )


install_source()
install_headers()
