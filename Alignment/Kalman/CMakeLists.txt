cet_make_library(SOURCE KAlignFilter.cxx KAlignParameters.cxx KAlignDerivatives.cxx KAlignConstants.cxx
  LIBRARY_NAME KAlignFilter
)

cet_build_plugin(KalmanAlign art::module
  LIBRARIES REG
    KAlignFilter
    AlignService_service
    Align
    KalmanReco
    RecoBase
    GeometryService_service
    DetGeoMapService_service
    MagneticFieldService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Geom
    ROOT::Tree
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()

cet_test(test_alignment_linear_algebra
  SOURCE test/test_alignment_linear_algebra.cc
  USE_BOOST_UNIT
)

cet_test(test_KAlignFilter
  SOURCE test_KAlignFilter.cc
  USE_BOOST_UNIT
  LIBRARIES PRIVATE
    KAlignFilter
)

cet_test(test_KAlignParameters
  SOURCE test_KAlignParameters.cc
  USE_BOOST_UNIT
  LIBRARIES PRIVATE
    KAlignFilter
)

cet_test(test_KAlignDerivatives
  SOURCE test_KAlignDerivatives.cc
  USE_BOOST_UNIT
  LIBRARIES PRIVATE
    KAlignFilter
)

cet_test(test_KAlignConstants
  SOURCE test_KAlignConstants.cc
  USE_BOOST_UNIT
  LIBRARIES PRIVATE
    KAlignFilter
)
