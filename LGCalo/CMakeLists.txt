include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ART_ROOT_IO_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

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
