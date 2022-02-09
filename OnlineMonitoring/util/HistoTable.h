/// \file    HistoTable.h
/// \brief   Class to read, hold, and deliver histogram data
/// \author  messier@indiana.edu, mbaird42@fnal.gov
/// \version $Id: HistoTable.h,v 1.5 2012-06-11 17:41:58 messier Exp $
#ifndef OM_HISTOTABLE_H
#define OM_HISTOTABLE_H
#include <string>
#include <map>
#include "OnlineMonitoring/util/HistoData.h"
class TH1F;
class TH2F;

namespace emph { 
  namespace onmon {
    class HistoTable {
    public:
      static HistoTable& Instance(const char* f=0, Detector_t d=kALLDET);
      
      /// Load the histogram data from a .csv file
      /// \param file - name of file to read
      /// \param d    - which detector context to apply (emph/ssd?)
      void ReadFile(const char* file, Detector_t d);

      /// Look up histogram data given name of histogram
      /// \param nm - "C++" name of histogram
      const HistoData* LookUp    (const char* nm) const;
      const HistoData& operator[](const char* nm) const;

    private:
      HistoTable();
      char NewLineChar(const char* f);
      void Unquote(std::string& s);

    public:
      std::map<std::string,HistoData> fTable; ///< Histogram data by C++ name
    };
  }//end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
