include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(LIBRARY_NAME DataQuality
		 SOURCE ${src_files}
		 LIBRARIES PRIVATE
		 	   art::Framework_Core
			   art::Persistency_Common
			   canvas::canvas
			   fhiclcpp::fhiclcpp
)

build_plugin(FillDataQuality "module"
  LIBRARIES PRIVATE
  	    RawData
	    DataQuality
)

build_plugin(AnalyzeTOFARICH "module"
  LIBRARIES PRIVATE
 	    RawData
	    DataQuality
	    RunHistory
            ChannelMap
	    Geometry
	    ToFUtil
	    RecoBase
	    art_root_io::TFileService_service
	    messagefacility::MF_MessageLogger
)

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::DataQuality)
