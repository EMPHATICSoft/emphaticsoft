include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
                   LIBRARY_NAME  GasCherenkov
                   LIBRARIES
                   RawData
                   RecoBase
                   ChannelMap
                   art_root_io::TFileService_service
                   messagefacility::MF_MessageLogger
                   ROOT::Hist
		   ROOT::Tree
)

build_plugin(GasCkovAna "module"
  LIBRARIES PRIVATE
    RawData
    RecoBase
    ChannelMap
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
