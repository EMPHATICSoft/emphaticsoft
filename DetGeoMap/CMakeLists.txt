cet_make_library(SOURCE DetGeoMap.cxx
		 LIBRARY_NAME DetGeoMap
		 LIBRARIES
		 Geometry
		 GeometryService_service
		 Align
		 AlignService_service
		 RecoBase
		 RawData
		 messagefacility::MF_MessageLogger
		 BASENAME_ONLY
		)

install_headers()
install_source()

add_subdirectory(service)
