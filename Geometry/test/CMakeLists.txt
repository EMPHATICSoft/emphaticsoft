include(ArtDictionary)
include(ArtMake)

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

cet_make_exec( NAME UnitTest
					SOURCE test.cxx
					LIBRARIES Geometry
					ROOT::MathCore
					ROOT::MathMore
			    )

install_headers()
install_source()
