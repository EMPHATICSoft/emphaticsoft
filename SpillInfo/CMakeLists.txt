include(ArtDictionary)
include(ArtMake)

FILE( GLOB src_files *.cc )

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)
cet_find_package(art REQUIRED)

include_directories($ENV{ART_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories( $ENV{IFBEAM_FQ_DIR}/include )
include_directories( $ENV{LIBWDA_FQ_DIR}/include )
include_directories( $ENV{IFDHC_FQ_DIR}/inc )
include_directories( $ENV{IFDH_ART_INC} )

link_directories($ENV{CETLIB_EXCEPT_LIB})
link_directories($ENV{IFDH_ART_LIB})
link_directories($ENV{IFBEAM_LIB})

art_make_library ( SOURCE ${src_files}
                   LIBRARY_NAME  SpillInfo
                   LIBRARIES
		   art::Framework_Core
		   art::Persistency_Common
		   art::Framework_Principal
		   art::Framework_Services_Registry
		   RecoBase
		   ifbeam
		   IFBeam_service
)

build_plugin(SpillInfo "module"
  LIBRARIES PRIVATE
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  RecoBase
  ifbeam
  IFBeam_service
)


install_headers()
install_fhicl()
install_source()
