////////////////////////////////////////////////////////////////////////
/// \file    DetChanPos.h
/// \brief   Class that converts channel information into a position
/// \version 
/// \author  jpaley@fnal.gov 
////////////////////////////////////////////////////////////////////////

#ifndef DETCHAN_POS_H
#define DETCHAN_POS_H

namespace emph {
  namespace geo {
    
    class DetChanPos {
    public:
      DetChanPos();
      ~DetChanPos() {};

      void SetRun(int run);

      bool T0Pos(int chan, double* x) const;
      bool SSDPos(int chan, double* x) const;
      bool ARICHPos(int, double* ) const { return false;}
      bool RPCPos(int chan, double* x) const;
      bool LGCaloPos(int chan, double* x) const;

    private:
      
      int fRun;
      
    };
    
  }  // end namespace geo
} // end namespace emph


#endif
