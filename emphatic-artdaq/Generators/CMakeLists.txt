add_definitions(-DLINUX)
link_directories($ENV{CAENCOMM_LIB})
link_directories($ENV{CAENVME_LIB})
link_directories($ENV{CAENDIGITIZER_LIB})


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


basic_plugin(DummyGenerator "generator"
  LIBRARIES REG
    ${BUILD_PLUGIN_CORE_LIB_LIST}
)

install_source()
install_headers()
