cet_make_library(SOURCE Geometry.cxx ModAlign/ModGDML.cxx
		 LIBRARY_NAME Geometry
		 LIBRARIES
    		 art::Framework_Core
    		 art::Persistency_Common
    		 canvas::canvas
    		 fhiclcpp::fhiclcpp
    		 ROOT::Geom
    		 ROOT::Physics
    		 messagefacility::MF_MessageLogger
    		 cetlib::container_algorithms
    		 cetlib::cetlib
	 	 Utilities
		 uuid		 
		 BASENAME_ONLY
)

install_headers()
install_source()

add_subdirectory(ModAlign)
add_subdirectory(demo)
add_subdirectory(service)
