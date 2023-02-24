include(ArtDictionary)
include(ArtMake)

find_package(art REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)

cet_build_plugin(ChannelStateService art::service
  LIBRARIES
  PUBLIC
  ChannelState
  PRIVATE
  art::Framework_Core
  art::Framework_Services_Registry
  fhiclcpp::fhiclcpp
  BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
