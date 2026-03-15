cet_build_plugin(GeometryService art::service
		 REG_SOURCE GeometryService_service.cc ../Persistency/SaveGDML.cxx
  		 LIBRARIES PUBLIC Geometry
		 	   PRIVATE
		 GeoPersistency
		 Geometry_Persistency_dict 	   
  		 messagefacility::MF_MessageLogger
  		 RunHistory
  		 RunHistoryService_service
	 	 Utilities
  		 art::Framework_Services_Registry
  		 cetlib_except::cetlib_except
  		 art::Framework_Principal
  		 ROOT::Core
  		 ROOT::Physics
  		 ROOT::RIO
  		 BASENAME_ONLY
)


install_headers()
install_source()
install_fhicl()
