cet_make_library( SOURCE Align.cxx
		  LIBRARY_NAME Align
		  LIBRARIES
		  Geometry
		  messagefacility::MF_MessageLogger
		  cetlib::cetlib cetlib_except::cetlib_except
		  ROOT::Geom
		  ROOT::Physics
		)

install_headers()
install_source()

add_subdirectory(service)
