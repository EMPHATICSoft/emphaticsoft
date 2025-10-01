cet_make_library( SOURCE NeuralNet.cxx
	LIBRARY_NAME TorchML
	LIBRARIES 
    art::Framework_Core
    art::Persistency_Common
    messagefacility::MF_MessageLogger
    ${TORCH_LIBRARIES}
  BASENAME_ONLY
)

install_headers()
install_source()
