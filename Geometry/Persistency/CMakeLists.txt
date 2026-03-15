
art_dictionary(DICTIONARY_LIBRARIES PRIVATE GeoPersistency)

cet_make_library(SOURCE SaveGDML.cxx
		 LIBRARY_NAME GeoPersistency
		 LIBRARIES
		 PRIVATE
    		   art::Framework_Core
    		   art::Persistency_Common
    		   canvas::canvas
    		   fhiclcpp::fhiclcpp
    		   messagefacility::MF_MessageLogger
    		   cetlib::container_algorithms
    		   cetlib::cetlib
	 	   Utilities
    		   ROOT::Core
		   ROOT::MathCore
		   ROOT::MathMore
		 PUBLIC
    		   ROOT::Geom
    		   ROOT::Physics
		 BASENAME_ONLY
)



install_headers()
install_source()

