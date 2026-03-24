########################################################################
# Unit tests for KalmanReco — pure-math layer (no art services required)
########################################################################

# Propagator + State: only need the KalmanReco library (pulls in ROOT::Matrix)
cet_test(test_propagator
  SOURCES test_propagator.cc
  USE_BOOST_UNIT
  LIBRARIES PRIVATE
    KalmanReco
    ROOT::Core
    ROOT::Matrix
)
