cet_make_library ( SOURCE ADCUtils.cxx
  LIBRARY_NAME ADCUtils
  LIBRARIES
	RawData
	RecoBase
  BASENAME_ONLY )

install_source()
install_headers()
