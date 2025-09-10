art_dictionary(DICTIONARY_LIBRARIES PRIVATE RecoBase)

FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME RecoBase
  LIBRARIES
    RawData
    Geometry
    ChannelMap
    ChannelMapService_service
    ROOT::Physics
    ROOT::Geom
    StandardRecord
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
