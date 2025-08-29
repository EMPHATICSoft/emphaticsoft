art_dictionary(DICTIONARY_LIBRARIES PRIVATE TorchML)

FILE( GLOB srcs *.cc )

cet_make_library( SOURCE ${srcs}
		  LIBRARY_NAME TorchML
		  LIBRARIES 
		  art::Framework_Core
		  art::Persistency_Common
		  messagefacility::MF_MessageLogger
		   ${TORCH_LIBRARIES}
)

install_headers()
install_source()
