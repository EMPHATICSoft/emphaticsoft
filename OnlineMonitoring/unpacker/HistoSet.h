///
/// \file    HistoSet.h
/// \brief   Hold the collection of histograms created by the producer
/// \version $Id: HistoSet.h,v 1.6 2012-11-02 04:33:42 messier Exp $
/// \author  messier@indiana.edu, mbaird42@fnal.gov
///
#ifndef OM_HISTOSET_H
#define OM_HISTOSET_H
#include <string>
#include <map>
#include <list>
#include "OnlineMonitoring/util/TickerSubscriber.h"
class TH1;
class TH1F;
class TH2F;
class TFile;

namespace emph{
  namespace onmon {
    /// Hold and manage the collection of histograms created by the
    /// producer
    class HistoSet : public TickerSubscriber {
    public:
      static HistoSet& Instance();
      ~HistoSet();

      TH1F* GetTH1F(const char* nm);
      TH2F* GetTH2F(const char* nm);

      void  GetNames(std::list<std::string>& h);

      void WriteToRootFile(TFile* f);

      /// Complete the TickerSubscriber interface
      void RunTicker();
      void SubrunTicker();
      void ThirtySecTicker();
      void OneMinTicker();
      void FiveMinTicker();
      void TenMinTicker();
      void ThirtyMinTicker();
      void HourTicker();
      void TwentyFourHrTicker();

      void DeleteTH1F(TH1F* h);
      void DeleteTH2F(TH2F* h);

      TH1*  FindTH1 (const char* nm);
      TH1F* FindTH1F(const char* nm);
      TH2F* FindTH2F(const char* nm);

      void UTCReset(int UTCHour);
      void UTCResetTH1F(int UTCHour, TH1F* h);
      void UTCResetTH2F(int UTCHour, TH2F* h);

    private:
      HistoSet();
      void  MakeTH1FCopies(const char* base,
        const char* tag,
        unsigned int lookback);
      void  MakeTH2FCopies(const char* base,
        const char* tag,
        unsigned int lookback);
      void CopyAndResetAll(unsigned int which);
      void CopyAndResetOne(const std::string& nm,
        unsigned int       which);
      void UTCReset();
    private:
      std::map<std::string,TH1F*> fTH1F; ///< The collection of 1D histos
      std::map<std::string,TH2F*> fTH2F; ///< The collection of 2D histos
    };
  } // end namespace onmon
} // end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
