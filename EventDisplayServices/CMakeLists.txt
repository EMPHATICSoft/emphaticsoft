include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)
find_package(art REQUIRED)
find_package(messagefacility REQUIRED)

cet_build_plugin( GeometryDrawingOptions art::service
  LIBRARIES #PRIVATE
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  art::Utilities
  messagefacility::MF_MessageLogger
  canvas::canvas
  ${MF_UTILITIES}
  ${FHICLCPP}
  EventDisplayBase
  cetlib::cetlib
  cetlib_except::cetlib_except
  BASENAME_ONLY
)

cet_build_plugin(PlotDrawingOptions art::service
  LIBRARIES #PRIVATE
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  art::Utilities
  messagefacility::MF_MessageLogger
  canvas::canvas
  ${MF_UTILITIES}
  ${FHICLCPP}
  EventDisplayBase
  cetlib::cetlib
  cetlib_except::cetlib_except
  BASENAME_ONLY
)

cet_build_plugin(RecoDrawingOptions art::service
  LIBRARIES #PRIVATE
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  art::Utilities
  messagefacility::MF_MessageLogger
  canvas::canvas
  ${MF_UTILITIES}
  ${FHICLCPP}
  EventDisplayBase
  cetlib::cetlib
  cetlib_except::cetlib_except
  BASENAME_ONLY
)

cet_build_plugin(RawDrawingOptions art::service
  LIBRARIES #PRIVATE
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  art::Utilities
  messagefacility::MF_MessageLogger
  canvas::canvas
  ${MF_UTILITIES}
  ${FHICLCPP}
  EventDisplayBase
  cetlib::cetlib
  cetlib_except::cetlib_except
  BASENAME_ONLY
)

cet_build_plugin(SimulationDrawingOptions art::service
  LIBRARIES #PRIVATE
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  art::Utilities
  messagefacility::MF_MessageLogger
  canvas::canvas
  ${MF_UTILITIES}
  ${FHICLCPP}
  EventDisplayBase
  cetlib::cetlib
  cetlib_except::cetlib_except
  BASENAME_ONLY
)

install_headers()
install_source()
