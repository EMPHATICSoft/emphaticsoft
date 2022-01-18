include(ArtDictionary)
include(ArtMake)

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

build_plugin(DemoModule "module"
  LIBRARIES PRIVATE
    RawData
    RecoBase
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
)

#art_make( BASENAME_ONLY
#         MODULE_LIBRARIES RawData
#	 		  BackTracker_service
#	  		   nusimdata_SimulationBase
#                           ${ART_FRAMEWORK_CORE}
#                           ${ART_FRAMEWORK_PRINCIPAL}
#                           ${ART_FRAMEWORK_SERVICES_REGISTRY}
#                           ${ART_FRAMEWORK_SERVICES_BASIC}
#                           ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
#                           ${ART_ROOT_IO_TFILESERVICE_SERVICE}
#                           art_Persistency_Common  
#                           art_Utilities canvas
#                           ${MF_MESSAGELOGGER}
#                           ${MF_UTILITIES}
#                           ${FHICLCPP}
#                           cetlib cetlib_except
#	 EXCLUDE           tutCAFMacro.C
#         )

install_headers()
install_fhicl()
install_source()
