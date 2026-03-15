
# art_dictionary(DICTIONARY_LIBRARIES PRIVATE Geometry)

cet_make_library(SOURCE Geometry.cxx Persistency/SaveGDML.cxx
		 LIBRARY_NAME Geometry
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

add_subdirectory(demo)
add_subdirectory(Persistency)
add_subdirectory(service)
