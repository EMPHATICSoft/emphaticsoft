////////////////////////////////////////////////////////////////////////
/// \brief  Some MPI high level utilities in the context of a simple Mastr <-> np-1 workers node. 
///          Used by main SSD Aligner Algo1 package.   
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef MYMPIUTILS_H
#define MYMPIUTILS_H
#include <mpi.h>
#include <vector>
#include "BTAlignInput.h"
#include "BeamTracks.h"
namespace emph{ 
  namespace rbal {
    
    void synchronizeFCNs(int aTag); // not needed!!! 
    void broadcastFCNParams(std::vector<double> &pVals);
    void getEventsCountOnRank0(BTAlignInput &myLL, std::vector<int> &evtsCnts); 
    void distributeEvtsSlow(BTAlignInput &myDat); // Thousands of MPI_Send/ MPI_Recv of ~500 bytes.. 
    void distributeEvts(BTAlignInput &myDat, double sleepFact=1.0e-10); // buffering.. optimization... not needed! 
    void collectBeamTracks(BeamTracks &myDat, bool removeOnWorkerRank = true);
    double MeanChiSqFromBTracks(BeamTracks &myDat, double upLimit); 
  }
}
#endif
