########################################################################
# Unit tests for RecoUtils — geometry math (no art event loop required)
#
# Note: RecoUtils links against messagefacility PRIVATELY; the test
# itself never triggers error-logging paths,
# so no MessageFacility initialisation is needed at runtime.
########################################################################

cet_test(test_recoutils
  SOURCES test_recoutils.cc
  USE_BOOST_UNIT
  LIBRARIES PRIVATE
    RecoUtils
    ROOT::Core
    ROOT::Physics   # TVector3
    ROOT::GenVector # ROOT::Math::XYZVector
    ROOT::Matrix    # TMatrixD, TVectorD
)
