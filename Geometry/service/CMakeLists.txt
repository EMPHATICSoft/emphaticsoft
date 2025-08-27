cet_build_plugin(GeometryService art::service
		 REG_SOURCE GeometryService_service.cc
  		 LIBRARIES PUBLIC Geometry
		 	   PRIVATE
  		 messagefacility::MF_MessageLogger
  		 RunHistory
  		 RunHistoryService_service
  		 art::Framework_Services_Registry
  		 cetlib_except::cetlib_except
  		 art::Framework_Principal
  		 ROOT::Geom
  		 ROOT::Physics
  		 BASENAME_ONLY
)


install_headers()
install_source()
install_fhicl()
