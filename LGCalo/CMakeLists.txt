include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

#art_make_library ( SOURCE ${src_files}
#                   LIBRARY_NAME  TrackReco
#                   LIBRARIES
#		   ToFUtil
#		   RawData
#		   RecoBase
#		   ChannelMap
#			RunHistory
#		   art_root_io::TFileService_service
#		   messagefacility::MF_MessageLogger
#		   ROOT::Core
#                   ROOT::Hist
#	           ROOT::Spectrum
#		   ROOT::Tree
#)

#cet_build_plugin(calotest art::module
#  LIBRARIES PRIVATE
#    ToFUtil
#    RawData
#    RecoBase
#    ChannelMap
#         RunHistory
#    art_root_io::TFileService_service
#    messagefacility::MF_MessageLogger
#    ROOT::Core
#    ROOT::Hist
#    ROOT::Spectrum
#    ROOT::Tree
#)

cet_build_plugin(CaloProd art::module
  LIBRARIES PRIVATE
    ToFUtil
    RawData
    RecoBase
    ChannelMap
	 RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
