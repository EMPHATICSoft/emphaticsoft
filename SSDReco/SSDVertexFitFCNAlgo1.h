////////////////////////////////////////////////////////////////////////
/// \brief   3D non-linear fit FCN function for Beam Track alignment, downstream of the target. 
///          Depends on the magnetic field map.
///          Input data is a vector of  rb::SSDStationPtAlgo1 
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef SSDVERTEXFITFCNALGO1_H
#define SSDVERTEXFITFCNALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>

#include "Minuit2/FCNBase.h"
#include "RecoBase/BeamTrackAlgo1.h" 
#include "RecoBase/DwnstrTrackAlgo1.h" 

namespace emph{ 
  namespace ssdr {
    
    typedef std::vector<rb::BeamTrackAlgo1>::const_iterator myBeamTrPtr;  
    typedef std::vector<rb::DwnstrTrackAlgo1>::const_iterator myDwnTrPtr;  

    class SSDVertexFitFCNAlgo1 : public ROOT::Minuit2::FCNBase {
    
    public:
      SSDVertexFitFCNAlgo1();
      ~SSDVertexFitFCNAlgo1();
      
    private:
      bool fDebugIsOn;
      double fErrorDef; // for Minuit. 
      double fZ2; // The coordinate of Station2 
      myBeamTrPtr fDataUpstr;
      std::vector<myDwnTrPtr> fDataDwn; // No Deep copy done here.. 
//      mutable std::vector<double> fResids; // Probably 
//      mutable double fLastChi2;
//      mutable std::ofstream fFOutResids; 
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (xv, yv, zv)
     
      // Adding  upstream or Beam track. 
      inline void SetInputUpstrTr(myBeamTrPtr itBeam) { fDataUpstr = itBeam; }
      inline void AddInputDwn(std::vector<rb::DwnstrTrackAlgo1>::const_iterator it) { fDataDwn.push_back(it); }  
      inline void ResetInput() { 
        fDataDwn.clear();
       //   fDataUpstr = nullptr; proper value of dangling iterator..?? 
       }
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline void SetZ2(double v) {fZ2 = v; }
       
      inline void SetDebugOn(bool v = true) { fDebugIsOn = v; }
//      inline double Resid(size_t kSe) const {
//        if (kSe < fResids.size()) return fResids[kSe];
//	return DBL_MAX;
//      }
      //  
      //  Getter.. None needed.. I think.. 
      //
      
//      void OpenOutResids(const std::string &fNameStr); 
//      void SpitOutResids(int spill, int evt); 
      void printInputData() const; 
          
     };
   }
}
#endif
    
