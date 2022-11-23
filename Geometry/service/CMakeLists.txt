include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

cet_build_plugin( Geometry art::service
#  IMPL_SOURCE Geometry.cxx DetChanPos.cxx
  LIBRARIES #PUBLIC
  Geometry
  messagefacility::MF_MessageLogger
  RunHistory
  RunHistory_service
#  art::Framework_Core
  art::Framework_Services_Registry
  cetlib_except::cetlib_except
#  PRIVATE
  art::Framework_Principal
  ROOT::Geom
  ROOT::Physics
  BASENAME_ONLY
  )


install_headers()
install_source()
install_fhicl()
