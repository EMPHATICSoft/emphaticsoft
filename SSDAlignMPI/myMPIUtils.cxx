#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cmath>
#include <complex.h>
#include <mpi.h>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <float.h>
#include <sys/time.h>

#include "BTAlignInput.h"
#include "BeamTracks.h"

namespace emph{ 
  namespace rbal {
    

    void synchronizeFCNs(int aTag) { // not sure at all I need this! 
      struct timeval tvEnd;
      char tmbuf2[64];
      int tagPhase = 99900 + aTag;
      int endPhase= 9999; 
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      int world_size;
      MPI_Comm_size(MPI_COMM_WORLD, &world_size);
      if (myRank == 0) {
        for (int kSlave=1; kSlave != world_size; kSlave++) {
          MPI_Send((void*) &endPhase, 1, MPI_INT, kSlave, tagPhase, MPI_COMM_WORLD );
        } 
      } else { 
        endPhase= -9999;
        MPI_Status aStatus;
        MPI_Recv((void*) &endPhase, 1, MPI_INT, 0, tagPhase, MPI_COMM_WORLD, &aStatus);
        gettimeofday(&tvEnd,NULL);
        time_t nowTimeEnd = tvEnd.tv_sec;
        struct tm *nowtmEnd = localtime(&nowTimeEnd);
        strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
        std::string dateNowEnd(tmbuf2);
        if (myRank == 1) std::cerr << " On rank " << myRank << " received signal " 
	                           << endPhase << " at time " << dateNowEnd <<  " end of sync   " <<  std::endl;
      }  
    }

    void broadcastFCNParams(std::vector<double> &pVals) {
      int world_size;
      MPI_Comm_size(MPI_COMM_WORLD, &world_size);
      if (world_size == 1) return;
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      int count=static_cast<int>(pVals.size());
      int aTag = myRank*1000;
      if (myRank == 0) {
        for (int kSlave=1; kSlave != world_size; kSlave++) {
          MPI_Send((void*) &pVals[0], count, MPI_DOUBLE, kSlave, aTag, MPI_COMM_WORLD );
        } 
      } else { 
        MPI_Status aStatus;
        MPI_Recv((void*) &pVals[0], count, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &aStatus);
      }
//      synchronizeFCNs(aTag);  
    }
    
    void broadcastIntsFromRank0(int aTag, std::vector<int> &dbData) {
      int tagPhase = 1459900 + aTag;
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      int count=static_cast<int>(dbData.size());
      int world_size;
      MPI_Comm_size(MPI_COMM_WORLD, &world_size);
      if (myRank == 0) {
      for (int kSlave=1; kSlave != world_size; kSlave++) {
        MPI_Send((void*) &dbData[0], count, MPI_INT, kSlave, tagPhase, MPI_COMM_WORLD );
       } 
      } else { 
        MPI_Status aStatus;
        MPI_Recv((void*) &dbData[0], count, MPI_INT, 0, tagPhase, MPI_COMM_WORLD, &aStatus);
     }
     synchronizeFCNs(aTag);  
   }
   void getEventsCountOnRank0(BTAlignInput &myLL, std::vector<int> &evtsCnts) {
     int tagTally = 9998989;
     int myRank;
     MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     int world_size;
     MPI_Comm_size(MPI_COMM_WORLD, &world_size);
     if (myRank != 0) {
       int nEvt = myLL.GetNumEvts();
       MPI_Send((void*) &nEvt, 1, MPI_INT, 0, tagTally, MPI_COMM_WORLD );
     } else {
       evtsCnts[0] = myLL.GetNumEvts();
       for (int kSlave=1; kSlave != world_size; kSlave++) {
         int nEvt = 0;
         MPI_Status aStatus;
         MPI_Recv((void*) &nEvt, 1, MPI_INT, kSlave, tagTally, MPI_COMM_WORLD, &aStatus );
         evtsCnts[kSlave] = nEvt;
        } 
      }
      broadcastIntsFromRank0(1999800, evtsCnts);
    }
    void distributeEvtsSlow(BTAlignInput &myDat) {

      struct timeval tvEnd, tvStart;
      char tmbuf2[64];
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      int np;
      MPI_Comm_size(MPI_COMM_WORLD, &np);
      if (np == 1) return;
      int numPerProc;
      int tagFirst = 1; 
      int numTotal = myDat.GetNumEvts();
      int leftOver = numTotal % np;
      numPerProc = numTotal/np;
      std::vector<int> numPerProcs(np, numPerProc);
      if (leftOver != 0) {
        for (size_t kp=1; kp != np; kp++)  numPerProcs[kp] = numPerProc + 1;
	numPerProcs[0] = numTotal - (np-1)*numPerProcs[1];
      }
      //
      if (myRank == 0) {
        for (int kP=1; kP != np; kP++) { 
          MPI_Send((void*) &numPerProcs[kP], 1, MPI_INT, kP, tagFirst, MPI_COMM_WORLD );
	}
      } else {
        MPI_Status aStatus;
        MPI_Recv((void*) &numPerProcs[myRank], 1, MPI_INT, 0, tagFirst, MPI_COMM_WORLD, &aStatus);
      } 
      std::cerr << "  distributeEvts, from rank " << myRank << " expecting " << numPerProcs[myRank] << " evts " << std::endl;
//      sleep(10);
      //
      if (myRank == 0) {      
         gettimeofday(&tvStart,NULL);
         time_t nowTimeStart = tvStart.tv_sec;
         struct tm *nowtmStart = localtime(&nowTimeStart);
         strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmStart);
         std::string dateNowStart(tmbuf2);
         std::cout << " distribute evts , start at " << dateNowStart << std::endl;
      } 
      //
      // Testing how long it take to remove events, one by one .. 
      //
      if (myRank == 122223) { 
          std::cerr << " testing how long it take to remove events one by one ... " << std::endl;
	  BTAlignInput myDat2;
	  for (size_t k=0; k !=  myDat.GetNumEvts()/3; k++) {     
	    std::vector<BeamTrackCluster>::const_iterator itF = myDat.cbegin();
	    int aSpill = itF->Spill(); int aEvtNum = itF->EvtNum(); 
            std::vector<double> tmpDat;
	    size_t nW = myDat.FillForMPITransfer(itF, tmpDat);
	    myDat.Remove(aSpill, aEvtNum); 
	    myDat2.AddFromMPIData(&tmpDat[0]);
	  }    
          gettimeofday(&tvEnd,NULL);
          time_t nowTimeEnd = tvStart.tv_sec;
          struct tm *nowtmEnd = localtime(&nowTimeEnd);
          strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
          std::string dateNowEnd(tmbuf2);
          std::cerr << " distributeEvts, ending test remove 1 by 1, and add .. at  " << dateNowEnd << std::endl;
		MPI_Finalize();
		exit(2);
      }
          
      int kP = 1;
      int tagTally = 0; int nTransfer = 0;
      while (true ) {
	if (myRank == 0) {
	  if (myDat.GetNumEvts() == numPerProcs[0]) break;
	  std::vector<BeamTrackCluster>::const_iterator itF = myDat.cbegin();
	  int aSpill = itF->Spill(); int aEvtNum = itF->EvtNum(); 
          std::vector<double> tmpDat;
	  size_t nW = myDat.FillForMPITransfer(itF, tmpDat);	    
	  itF->DoNotUse();
//
//	  myDat.Remove(aSpill, aEvtNum);  That was the slow part, I am pretty sure!!!  
	  tagTally = 100000*kP + nTransfer;
          MPI_Send((void*) &tmpDat[0], nW, MPI_DOUBLE, kP, tagTally, MPI_COMM_WORLD );
	  if ((nTransfer < 3) || (nTransfer%1000 == 0))
	     std::cerr << "  .... from rank 0, sending " << nW << "  doubles to rank " << kP 
	               << " nTransfers " << nTransfer << " tag Tally " << tagTally << std::endl;
	  nTransfer++;  
//	  tagTally++;
	  kP++;
	  if (kP == np) kP = 1;   
//	  sleep(5);
	} else {
	   if (myDat.GetNumEvts() == numPerProcs[myRank]) break;
//           MPI_Status statusProbe;
//	   MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &statusProbe);
	   int nD = 46; // I know this.. Hopefully I won't change my mind.. 
//	   MPI_Get_count(&statusProbe, MPI_DOUBLE, &nD);
	   if ((nTransfer < 3) || (nTransfer%1000 == 0)) std::cerr << "  .... from rank " 
	      << myRank << " after probing , expecting " << nD << "  doubles .. " << std::endl;
           MPI_Status statusRec;
	   double dataTmp[static_cast<size_t>(nD)];
           MPI_Recv((void*) &dataTmp[0], nD, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec );
	   myDat.AddFromMPIData(dataTmp);
	   if ((nTransfer < 3) || (nTransfer%1000 == 0)) std::cerr << "  .... from rank " << myRank << " received buffer " 
	          << " tag " << statusRec.MPI_TAG << " evt " << myDat.rbegin()->EvtNum() 
		  << " from spill " << myDat.rbegin()->Spill() << " and added to worker node " << std::endl; 
//	   if ((nTransfer < 3) || (nTransfer%1000 == 0)) std::cerr << "  .... from rank " << myRank << " received transfer " << nTransfer << std::endl;
	   nTransfer++;  
//	   tagTally++;
	 }
      } // going on untill done 
      if (myRank == 0) {      
        gettimeofday(&tvEnd,NULL);
        time_t nowTimeEnd = tvStart.tv_sec;
        struct tm *nowtmEnd = localtime(&nowTimeEnd);
        strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
        std::string dateNowEnd(tmbuf2);
        std::cout << " distributeEvts, ending at  " << dateNowEnd << " after " << nTransfer <<  " transfers " << std::endl;
      }
       std::cerr << "  distributeEvts, done, from rank " << myRank << " evts, accepted " << myDat.GetNumEvts() << std::endl;

    } // distributeEvtsSlow
    
    void distributeEvts(BTAlignInput &myDat, double sleepFact) {
      struct timeval tvEnd, tvStart;
      char tmbuf2[64];
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      int np;
      MPI_Comm_size(MPI_COMM_WORLD, &np);
      if (np == 1) return;
      std::cerr << " distributeEvts, sleepFact " << sleepFact << std::endl;
      int numPerProc;
      int tagFirst = 1; 
      int numTotal = myDat.GetNumEvts();
      int leftOver = numTotal % np;
      numPerProc = numTotal/np;
      std::vector<int> numPerProcs(np, numPerProc);
      if (leftOver != 0) {
        for (size_t kp=1; kp != np; kp++)  numPerProcs[kp] = numPerProc + 1;
	numPerProcs[0] = numTotal - (np-1)*numPerProcs[1];
      }
      //
      int sleepBest = static_cast<int>(sleepFact*numPerProcs[1]);
      if (myRank == 0) {
        for (int kP=1; kP != np; kP++) { 
          MPI_Send((void*) &numPerProcs[kP], 1, MPI_INT, kP, tagFirst, MPI_COMM_WORLD );
	}
      } else {
        MPI_Status aStatus;
        MPI_Recv((void*) &numPerProcs[myRank], 1, MPI_INT, 0, tagFirst, MPI_COMM_WORLD, &aStatus);
      } 
      std::cerr << "  distributeEvts, from rank " << myRank << " expecting " << numPerProcs[myRank] << " evts " << std::endl;
//      sleep(10);
      //
      if (myRank == 0) {      
         gettimeofday(&tvStart,NULL);
         time_t nowTimeStart = tvStart.tv_sec;
         struct tm *nowtmStart = localtime(&nowTimeStart);
         strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmStart);
         std::string dateNowStart(tmbuf2);
         std::cerr << " distribute evts , from rank 0, start at " << dateNowStart << std::endl;
      }
//      std::cerr << " No! quit here and now " << std::endl; MPI_Finalize(); exit(2); 
      size_t nD = 46; // I know this.. Hopefully I won't change my mind.. 
      std::vector<double> tmpBuffer(nD, 0.); // factor 10 is for debugging.. 
      size_t nDb = nD*sizeof(double);
      double buffer[nD*(numPerProcs[myRank]+ np + 1)]; // to gaurantee enough space ??? flimsy.. 
      double *bufferPtr = &buffer[0];
      int tagCurrent = 1000*myRank;
      if (myRank == 0) {
        std::vector<BeamTrackCluster>::const_iterator itC = myDat.cbegin();
        for (int kp=1; kp != np; kp++) {
	  bufferPtr = &buffer[0];
	  for (int evtCnt = 0; evtCnt != numPerProcs[kp]; evtCnt++) {
	    if ((kp == 1) && ((evtCnt < 3) || (evtCnt == numPerProcs[kp]-1))) std::cerr << " At evtCnt  " << evtCnt 
	           << " evt Num " << itC->EvtNum() << " spill " << itC->Spill() << std::endl;
	    std::vector<double> tmpBuffer(nD, 0.); // factor 10 is for debugging.. 
	    size_t nDCheck = myDat.FillForMPITransfer(itC, tmpBuffer);
	    memcpy((void*) bufferPtr, (void*) &tmpBuffer[0], nDb); 
	    bufferPtr += nD;
	    itC->DoNotUse();
	    itC++;
	  }
	  tagCurrent = kp*1000;
           MPI_Send((void*) &buffer[0], nD*numPerProcs[kp], MPI_DOUBLE, kp, tagCurrent, MPI_COMM_WORLD );
	} // on worker nodes 
      } else {
	 MPI_Status statusProbe;
	 MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &statusProbe);
	 int nWRec = 0; // I know this.. Hopefully I won't change my mind.. 
	 MPI_Get_count(&statusProbe, MPI_DOUBLE, &nWRec);
         MPI_Status statusRec;
	 if (sleepBest > 0 && (myRank > 1)) {
	    std::cerr << " .... Rank " << myRank << " Sleeping ... " << sleepBest*(myRank-1) << std::endl;
	    sleep(sleepBest*(myRank-1));
	 }
	 struct timeval tvCurr;
	 std::cerr << " ... About to received the long buffer on node " << myRank <<  " length " << nWRec << std::endl;
         MPI_Recv((void*) &buffer[0], nWRec, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec);
	 if (nWRec != nD*numPerProcs[myRank]) {
	   std::cerr << " distributeEvts.. problem, inconsistent buffering for rank " << myRank << " Recieved " 
	             << nWRec << " expected " << nD*numPerProcs[myRank] << std::endl;
	 }
	 // 
	 bufferPtr = &buffer[0];
         for (int iEvt=0; iEvt != numPerProcs[myRank]; iEvt++) {
	   myDat.AddFromMPIData(bufferPtr);
	   if ((iEvt < 3) || (iEvt > (numPerProcs[myRank] - 4)))
	      std::cerr << " ... from Rank " << myRank << " Last event stored iEvt " << iEvt << " evtNum " 
	                           << myDat.rbegin()->EvtNum() << " spill " << myDat.rbegin()->Spill() << std::endl;
	   bufferPtr += nD; 
	    
         }
	 
      }
      if (myRank == 0) {      
         gettimeofday(&tvEnd,NULL);
         time_t nowTimeEnd = tvEnd.tv_sec;
         struct tm *nowtmEnd = localtime(&nowTimeEnd);
         strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
         std::string dateNowEnd(tmbuf2);
         std::cout << " distribute evts , from rank 0, ending at " << dateNowEnd << std::endl;
      } 
      std::cerr << "  distributeEvts, done, from rank " << myRank << " evts, accepted " << myDat.GetNumEvts() << std::endl;

    } // distributeEvts
    
    void collectBeamTracks(BeamTracks &myDat, bool removeLocal) {  // on rank 0 

      struct timeval tvEnd, tvStart;
      char tmbuf2[64];
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      int np;
      MPI_Comm_size(MPI_COMM_WORLD, &np);
      if (np == 1) return;
      int tagFirst = 1; 
      int numHere =  myDat.size();
      std::cerr << "  collectBeamTracks, from rank " << myRank << " Number of tracks here " << numHere << std::endl;
//      sleep(10);
      //
      if (myRank == 0) {      
         gettimeofday(&tvStart,NULL);
         time_t nowTimeStart = tvStart.tv_sec;
         struct tm *nowtmStart = localtime(&nowTimeStart);
         strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmStart);
         std::string dateNowStart(tmbuf2);
         std::cout << " collectBeamTracks evts , start at " << dateNowStart << std::endl;
      } 
      int tagTally = 0; int nTransfer = 0;
      int nTotalExpected = 0; 
      int nn = static_cast<int>(myDat.size());
      std::vector<int> nExpected(np, nn);
       
      if (myRank != 0) {
        tagTally = myRank*1000;
        MPI_Send((void*) &nn, 1, MPI_INT, 0, tagTally, MPI_COMM_WORLD );
      } else {
        nTotalExpected += static_cast<int>(myDat.size()); 
        for (int kp=1; kp != np; kp++ ) {
	  int nn = 0;
          MPI_Status statusRec;
          MPI_Recv((void*) &nn, 1, MPI_INT, kp, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec );
	  nTotalExpected += nn; nExpected[0] += nn;
        }
	std::cerr << " .....  Got an event tally, on rank 0, expecting " << nTotalExpected << " beam tracks " << std::endl;
      }
      std::vector<BeamTrack>::const_iterator itF = myDat.cbegin();
      while (true) { // This assumes we had events to fit on rank 0..
	if (myRank != 0) {
	  if (nTransfer == nExpected[myRank]) break;
	  int aSpill = itF->Spill(); int aEvtNum = itF->EvtNum(); 
          std::vector<double> tmpDat;
	  size_t nW = itF->Serialize(tmpDat);
	  itF++;
	  tagTally = 100000*myRank + nTransfer; // arbitrary.. 
          MPI_Send((void*) &tmpDat[0], nW, MPI_DOUBLE, 0, tagTally, MPI_COMM_WORLD );
	  if ((nTransfer < 3) || (nTransfer%1000 == 0))
	     std::cerr << "  .... from rank " << myRank << ", sending " << nW << "  doubles to rank 0 nTransfers " 
	               << nTransfer << " tag Tally " << tagTally << std::endl;
	  nTransfer++; 
//	  sleep(5);
	} else {
	   if (nExpected[0] == static_cast<int>(myDat.size())) break;
	   for (int kp=1; kp != np; kp++ ) {
             MPI_Status statusProbe;
	     MPI_Probe(kp, MPI_ANY_TAG, MPI_COMM_WORLD, &statusProbe);
	     int nD;
	     MPI_Get_count(&statusProbe, MPI_DOUBLE, &nD);
	     if ((nTransfer < 3) || (nTransfer%1000 == 0)) std::cerr << "  .... from rank " 
	      << kp << " after probing , expecting " << nD << "  doubles from rank 0 " << std::endl;
             MPI_Status statusRec;
	     std::vector<double> dataTmp(static_cast<size_t>(nD), DBL_MAX);
             MPI_Recv((void*) &dataTmp[0], nD, MPI_DOUBLE, kp, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec );
	     BeamTrack aTrack; 
	     aTrack.DeSerialize(dataTmp);
	     myDat.AddBT(aTrack);
	     
	     if ((nTransfer < 3) || (nTransfer%1000 == 0)) {
	        std::cerr << "  .... from rank 0, received buffer,  tag is " 
	          << statusRec.MPI_TAG << " from rank " << kp << " Number of track so far " << myDat.size() <<  std::endl;
		std::cerr << " ....................Spill  " << aTrack.Spill() << " Event " 
		          << aTrack.EvtNum() << " chiSq " << aTrack.ChiSq() << " Resid 1 " << aTrack.Resid(1) << std::endl;
	     } 
	     nTransfer++; 
	   } 
	 }
      } // going on untill done 
      if (myRank == 0) {      
        gettimeofday(&tvEnd,NULL);
        time_t nowTimeEnd = tvStart.tv_sec;
        struct tm *nowtmEnd = localtime(&nowTimeEnd);
        strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
        std::string dateNowEnd(tmbuf2);
        std::cout << " collectBeamTracks, ending at  " << dateNowEnd << " after " 
	          << nTransfer <<  " transfers " << " Number of tracks " << myDat.size() << std::endl;
      }
      std::cerr << "  collectBeamTracks, done, from rank " << myRank << " Number of tracks "  << myDat.size() << std::endl;
      if (removeLocal && (myRank != 0)) {
         std::cerr << " ................... On Rank " << myRank << " Clearing the Beam Track container " << std::endl;
         myDat.clear();
      }
    } // collectBeamTracks
    
    double MeanChiSqFromBTracks(BeamTracks &myDat, double upLimit, double chiAddSum) { 
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
//      std::cerr << " MeanChiSqFromBTracks, Start, myRank " << myRank << " UpLimit  " << upLimit << std::endl;
      int np;
      MPI_Comm_size(MPI_COMM_WORLD, &np);
      if (np == 1) {
//         std::cerr << " .... MeanChiSqFromBTracks, one process, meanChi from tracking " 
//	           << myDat.MeanChiSq(upLimit) << " Survey + beam   " << chiAddSum << std::endl;
         return ( myDat.MeanChiSq(upLimit) + chiAddSum) ;
      }
      double mean = 0.;
      // The summation node is rank 0 ;
      if (myRank != 0) {
        double meanLocal = myDat.MeanChiSq(upLimit) + chiAddSum;
//        std::cerr << " MeanChiSqFromBTracks, myRank " << myRank << " meanLocal " <<  meanLocal << std::endl;
	int tagTally = 1000*myRank + 1; // arbitrary.. 
        MPI_Send((void*) &meanLocal, 1, MPI_DOUBLE, 0, tagTally, MPI_COMM_WORLD );
      } else {
        mean += myDat.MeanChiSq(upLimit) + chiAddSum;
//        std::cerr << " MeanChiSqFromBTracks, myRank " << myRank << " meanLocal " <<  mean << std::endl;
        for (int kp=1; kp != np; kp++ ) { 
          double meanWorker = 0;
          MPI_Status statusRec;
          MPI_Recv((void*) &meanWorker, 1, MPI_DOUBLE, kp, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec );
          mean += meanWorker;
        }
      }
//      std::cerr << " After collecting, MeanChiSqFromBTracks, myRank " << myRank << " mean " <<  mean/np << std::endl;
      // 
      // broadcast this info. 
      //
      int tagTally = 2000*myRank + 1;
      double meanGlobal = 1.0e10;
      if (myRank == 0) {
        meanGlobal = mean/np;
	int tagTally = 100000*myRank + 1; // arbitrary.. 
	for (size_t kp=1; kp != np; kp++) 
           MPI_Send((void*) &meanGlobal, 1, MPI_DOUBLE, kp, tagTally, MPI_COMM_WORLD );
      } else {
        MPI_Status statusRec;
        MPI_Recv((void*) &meanGlobal, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec );
      }
//      std::cerr << " MeanChiSqFromBTracks, end game on myRank " << myRank << " mean " <<  meanGlobal << std::endl;
      return meanGlobal;
    }
  } // name rbal 
} // name space emph 
