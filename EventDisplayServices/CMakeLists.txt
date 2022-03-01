include(ArtDictionary)
include(ArtMake)

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)
cet_find_package(art REQUIRED)
cet_find_package(messagefacility REQUIRED)

simple_plugin( GeometryDrawingOptions service
  LIBRARIES PRIVATE
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

simple_plugin(PlotDrawingOptions service
  LIBRARIES PRIVATE
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

simple_plugin(RecoDrawingOptions service
  LIBRARIES PRIVATE
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

simple_plugin(RawDrawingOptions service
  LIBRARIES PRIVATE
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

simple_plugin(SimulationDrawingOptions service
  LIBRARIES PRIVATE
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
