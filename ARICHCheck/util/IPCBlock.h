///
/// \file    IPCBlock.h
/// \brief
/// \version $Id: IPCBlock.h,v 1.5 2012-11-14 07:37:03 messier Exp $
/// \author  messier@indiana.edu
///
#ifndef OM_IPCBLOCK_H
#define OM_IPCBLOCK_H
extern "C" {
#include <time.h>
}

// Standard values to start and end each data block to check for data
// integrity
static const unsigned int BLOCK_HEADER = 0xe9290001;
static const unsigned int BLOCK_FOOTER = 0xe9290002;

// Size of buffer for histogram names and titles
static const unsigned int HISTO_NAME_SZ=256;

// Largest number of bins in histograms
static const int HISTO_NBINS_SZ=2048*2048;

// Largest number of histograms we expect the server to hold
static const unsigned int NMAX_HISTOS = 16384;

namespace emph { 
  namespace onmon {
    //
    // Summary of resources used by the plotter program. Filled by
    // plotter process. Read-only for viewer process.
    //
    struct PlotterResources
    {
      unsigned int fHeader; ///< Start data block marker
      unsigned int fRun;    ///< Run number of last update
      unsigned int fSubrun; ///< Subrun number of last update
      unsigned int fEvent;  ///< Event number of last update
      pid_t        fPID;    ///< Process ID
      long         fSTIME;  ///< System time
      long         fUTIME;  ///< User time
      long         fCPU;    ///< Process CPU usage
      long         fRSIZE;  ///< Process size
      time_t       fCLOCK;  ///< Time of update
      unsigned int fFooter; ///< End data block marker
    };

    //
    // Block holding the names of all the histograms held by the server
    //
    struct HistogramList
    {
      unsigned int fHeader;
      char         fClientLock;
      char         fServerLock;
      char         fRequestPending;
      unsigned int fN;
      char         fName[NMAX_HISTOS][HISTO_NAME_SZ];
      unsigned int fFooter;
    };

    //
    // Histogram data held by plotter. Filled by plotter
    // process. Read-only for viewer process.
    //
    struct PlotterHistogram
    {
      unsigned int fHeader;
      char         fClientLock;           ///< Is access locked by the server?
      char         fServerLock;           ///< Is access locked by the client?
      char         fRequestPending;       ///< Is there a request pending?
      char         fHistoOK;              ///< Is the published histogram info OK?
      char         fType[HISTO_NAME_SZ];  ///< What type of histogram is this?
      char         fName[HISTO_NAME_SZ];  ///< What is the name of this histogram?
      char         fTitle[HISTO_NAME_SZ]; ///< Histogram title
      unsigned int fN1;                   ///< Number of X bins
      unsigned int fN2;                   ///< Number of Y bins
      float        fX1;                   ///< X range lo side
      float        fX2;                   ///< X range hi side
      float        fY1;                   ///< Y range lo side
      float        fY2;                   ///< Y range hi side
      float        fData[HISTO_NBINS_SZ]; ///< Size of histogram buffer
      unsigned int fFooter;
    };


    //......................................................................

    //
    // The data block used for inter-process communication
    //
    struct IPCBlock
    {
      unsigned int       fHeader;
      unsigned int       fNclient;
      PlotterResources   fProcessResources;
      HistogramList      fHistoList;
      PlotterHistogram   fHistogram;
      unsigned int       fFooter;
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
