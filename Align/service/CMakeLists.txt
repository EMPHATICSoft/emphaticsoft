include(ArtDictionary)
include(ArtMake)

find_package(art REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)

cet_build_plugin(Align art::service
  LIBRARIES
  PUBLIC
  Align
  PRIVATE
#  RunHistoryService_service
  art::Framework_Services_Registry
  fhiclcpp::fhiclcpp
  BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
