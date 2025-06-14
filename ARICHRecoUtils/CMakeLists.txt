art_dictionary(DICTIONARY_LIBRARIES PRIVATE ARICHRecoUtils)

FILE( GLOB srcs *.cc )

cet_make_library( SOURCE ${srcs}
		  LIBRARY_NAME ARICHRecoUtils
		  LIBRARIES 
		  Geometry
		  Geometry_service
		  Simulation
		  ChannelMap
		  ChannelMapService_service
		  art::Framework_Core
		  art::Persistency_Common
		  ROOT::Core
		  ROOT::Physics
		  ROOT::Hist
)

install_headers()
install_source()
