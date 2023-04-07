#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/IPCBlock.h"
#include <sys/time.h>
#include <sys/resource.h>
#ifdef DARWINBUILD
#include <unistd.h>
#endif
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
using namespace emph::onmon;

//
// Number of microseconds to wait between request attempts and the
// number of requests to make. Units are microseconds
//
static const unsigned int kSecond = 1000000;
static const unsigned int kSleep  = kSecond/1000;
static const unsigned int kWait   = 5*kSecond;

//......................................................................

IPC::IPC(int mode, const char* shm_key) : fMode(mode)
{
  int shmflg = 0666;
  if (fMode == kIPC_SERVER) shmflg |= IPC_CREAT;

  memset(fShmHdl, 0, kHdlSz);
  strncpy(fShmHdl, shm_key, kHdlSz-1);
  fKey = (key_t*)fShmHdl;

  //
  // Try to open the SHM area. Try repeatedly eventually timing out.
  //
  unsigned int i, imax = 30*(kSecond/kSleep);
  for (i=0; i<imax; ++i) {
    fShmId = shmget(*fKey, sizeof(emph::onmon::IPCBlock), shmflg);
    if (fShmId>0) break;
    if (i%100==0) {
      std::cerr << __FILE__ << ":" << __LINE__
		<< " Looking for shared memory segment..." << std::endl;
    }
    usleep(kSleep);
  }
  if (fShmId<0) {
    std::cerr << __FILE__ << ":" << __LINE__ << " shmget failed" << std::endl;
    exit(1);
  }

  fShm = shmat(fShmId, NULL, 0);
  if (fShm==(char*)-1) {
    std::cerr << __FILE__ << ":" << __LINE__ << " shmat failed" << std::endl;
    exit(1);
  }

  fIPC = (emph::onmon::IPCBlock*)fShm;

  if (fMode == kIPC_SERVER) {
    //
    // Zero the data blocks and set the headers and footers to their
    // standard values
    //
    memset(fIPC, 0, sizeof(emph::onmon::IPCBlock));

    fIPC->fHeader = BLOCK_HEADER;
    fIPC->fFooter = BLOCK_FOOTER;

    fIPC->fProcessResources.fHeader = BLOCK_HEADER;
    fIPC->fProcessResources.fFooter = BLOCK_FOOTER;

    fIPC->fHistoList.fHeader = BLOCK_HEADER;
    fIPC->fHistoList.fFooter = BLOCK_FOOTER;

    fIPC->fHistogram.fHeader = BLOCK_HEADER;
    fIPC->fHistogram.fFooter = BLOCK_FOOTER;
  }

  if (fMode == kIPC_CLIENT) {
    ++fIPC->fNclient;
  }
}

//......................................................................

IPC::~IPC()
{
  if (fMode == kIPC_CLIENT) {
    --fIPC->fNclient;
    shmdt(fShm);
  }
  if (fMode == kIPC_SERVER) {
    shmdt(fShm);
    shmid_ds buf;
    shmctl(fShmId, IPC_RMID, &buf);
  }
}

//......................................................................

TH1F* IPC::RequestTH1F(const char* n)
{
  if (fMode==kIPC_SERVER) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Server used as client." << std::endl;
    abort();
  }

  //
  // Wait for any locks to clear then set our own client lock
  //
  unsigned int i;
  for (i=0; i<kWait; ++i) {
    if ( (fIPC->fHistogram.fClientLock    ==0) &&
	 (fIPC->fHistogram.fServerLock    ==0) &&
	 (fIPC->fHistogram.fRequestPending==0)) break;
    usleep(kSleep);
  }
  if (i==kWait) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Time out waiting for client lock, server lock, "
	      << "and pending requests. " << std::endl;
    return 0;
  }

  fIPC->fHistogram.fClientLock = 1;

  //
  // Submit a request to the server and wait for a response
  //
  this->Request(n, "TH1F");
  for (i=0; i<kWait; ++i) {
    if (fIPC->fHistogram.fRequestPending==0) break;
    usleep(kSleep);
  }
  if (i==kWait) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Time out waiting for request to be satisfied."
	      << std::endl;
    fIPC->fHistogram.fClientLock = 0;
    return 0;
  }

  //
  // With the request satisfied by the server, make a TH1F for
  // return. Now that we are done with the data in the shared memory
  // segment, release the lock we have on it.
  //
  TH1F* h = this->UnpackTH1F();
  fIPC->fHistogram.fClientLock = 0;

  return h;
}

//......................................................................

TH2F* IPC::RequestTH2F(const char* n)
{
  if (fMode==kIPC_SERVER) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Server used as client." << std::endl;
    abort();
  }

  //
  // Wait for any locks to clear then set our own client lock
  //
  unsigned int i;
  for (i=0; i<kWait; ++i) {
    if ( (fIPC->fHistogram.fClientLock    ==0) &&
	 (fIPC->fHistogram.fServerLock    ==0) &&
	 (fIPC->fHistogram.fRequestPending==0)) break;
    usleep(kSleep);
  }
  if (i==kWait) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Time out waiting for client lock, server lock, "
	      << "and pending requests. "
	      << std::endl;
    return 0;
  }

  fIPC->fHistogram.fClientLock = 1;

  //
  // Submit a request to the server and wait for a response
  //
  this->Request(n, "TH2F");
  for (i=0; i<kWait; ++i) {
    if (fIPC->fHistogram.fRequestPending==0) break;
    usleep(kSleep);
  }
  if (i==kWait) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Time out waiting for request to be satisfied."
	      << std::endl;
    fIPC->fHistogram.fClientLock = 0;
    return 0;
  }

  //
  // With the request satisfied by the server, make a TH1F for
  // return. Now that we are done with the data in the shared memory
  // segment, release the lock we have on it.
  //
  TH2F* h = this->UnpackTH2F();
  fIPC->fHistogram.fClientLock = 0;

  return h;
}

//......................................................................

///
/// Client call to request the histogram list
///
int IPC::RequestHistoList(std::list<std::string>& hlist)
{
  if (fMode==kIPC_SERVER) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Server used as client." << std::endl;
    abort();
  }

  //
  // Wait for any locks to clear then set our own client lock
  //
  unsigned int i;
  for (i=0; i<kWait; ++i) {
    if ( (fIPC->fHistoList.fClientLock    ==0) &&
	 (fIPC->fHistoList.fServerLock    ==0) &&
	 (fIPC->fHistoList.fRequestPending==0)) break;
    usleep(kSleep);
  }
  if (i==kWait) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Time out waiting for client lock, server "
	      << "lock, and pending requests."
	      << std::endl;
    return 0;
  }

  fIPC->fHistoList.fClientLock     = 1;
  fIPC->fHistoList.fRequestPending = 1;
  fIPC->fHistoList.fClientLock     = 0;

  for (i=0; i<kWait; ++i) {
    if (fIPC->fHistoList.fRequestPending==0) break;
    usleep(kSleep);
  }
  if (i==kWait) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Time out waiting for request to be satisfied."
	      << std::endl;
    fIPC->fHistoList.fRequestPending = 0;
    return 0;
  }

  //
  // With the request satisfied by the server, fill the list
  //
  fIPC->fHistoList.fClientLock = 1;
  for (i=0; i<fIPC->fHistoList.fN; ++i) {
    hlist.push_back(fIPC->fHistoList.fName[i]);
  }
  fIPC->fHistoList.fClientLock = 0;

  return 1;
}

//......................................................................

TH1F* IPC::UnpackTH1F()
{
  static const std::string s("TH1F");
  if (s != fIPC->fHistogram.fType)  return 0;
  if (fIPC->fHistogram.fHistoOK==0) return 0;

  TH1F* h = new TH1F(fIPC->fHistogram.fName,
		     fIPC->fHistogram.fTitle,
		     fIPC->fHistogram.fN1,
		     fIPC->fHistogram.fX1,
		     fIPC->fHistogram.fX2);

  for (int i=0; i<h->fN; ++i) {
    h->SetBinContent(i, fIPC->fHistogram.fData[i]);
  }

  return h;
}

//......................................................................

TH2F* IPC::UnpackTH2F()
{
  static const std::string s("TH2F");
  if (s != fIPC->fHistogram.fType)  { return 0; }
  if (fIPC->fHistogram.fHistoOK==0) { return 0; }

  TH2F* h = new TH2F(fIPC->fHistogram.fName,
		     fIPC->fHistogram.fTitle,
		     fIPC->fHistogram.fN1,
		     fIPC->fHistogram.fX1,
		     fIPC->fHistogram.fX2,
		     fIPC->fHistogram.fN2,
		     fIPC->fHistogram.fY1,
		     fIPC->fHistogram.fY2);
  for (int i=0; i<h->fN; ++i) {
    h->SetBinContent(i, fIPC->fHistogram.fData[i]);
  }

  return h;
}

//......................................................................

///
/// Submit a request to the server. It is assumed that the caller has
/// already checked that the server and client locks are clear
///
int IPC::Request(const char* n, const char* t)
{
  fIPC->fHistogram.fRequestPending = 1;
  strcpy(fIPC->fHistogram.fName, n);
  strcpy(fIPC->fHistogram.fType, t);
  return 1;
}

//......................................................................

int IPC::HandleRequests()
{
  int ok1 = this->HandleHistogramRequests();
  int ok2 = this->HandleHistoListRequests();
  if (ok1==1 && ok2==1) return 1;
  return 0;
}

//......................................................................

void IPC::PostResources(unsigned int run,
			unsigned int subrun,
			unsigned int event)
{
  fIPC->fProcessResources.fRun    = run;
  fIPC->fProcessResources.fSubrun = subrun;
  fIPC->fProcessResources.fEvent  = event;

  struct rusage r;
  getrusage(RUSAGE_SELF, &r);

  fIPC->fProcessResources.fPID    = getpid();
  fIPC->fProcessResources.fSTIME  = r.ru_stime.tv_sec;
  fIPC->fProcessResources.fUTIME  = r.ru_utime.tv_sec;
  fIPC->fProcessResources.fCPU    = 0;
  fIPC->fProcessResources.fRSIZE  = r.ru_maxrss;
  fIPC->fProcessResources.fCLOCK  = time(NULL);

}

//......................................................................

int IPC::HandleHistogramRequests()
{
  if (fMode == kIPC_CLIENT) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Client used as server." << std::endl;
    abort();
  }

  //
  // Only proceed if there is a request pending
  //
  if (fIPC->fHistogram.fRequestPending==0) return 0;

  fIPC->fHistogram.fServerLock = 1;
  static const std::string TH1Fs("TH1F");
  if (TH1Fs==fIPC->fHistogram.fType) {
    TH1F* h1 = this->FindTH1F(fIPC->fHistogram.fName);

    this->Publish1DHistogram(h1);

    fIPC->fHistogram.fServerLock     = 0;
    fIPC->fHistogram.fRequestPending = 0;

    if (h1) return 1;
  }

  static const std::string TH2Fs("TH2F");
  if (TH2Fs==fIPC->fHistogram.fType) {
    TH2F* h2 = this->FindTH2F(fIPC->fHistogram.fName);

    this->Publish2DHistogram(h2);

    fIPC->fHistogram.fServerLock     = 0;
    fIPC->fHistogram.fRequestPending = 0;

    if (h2) return 1;
  }

  // If we reach here, we have not sucessfully exited by any of the
  // above pathways, so return 0.
  return 0;

}

//......................................................................

int IPC::HandleHistoListRequests()
{
  if (fMode == kIPC_CLIENT) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Client used as server." << std::endl;
    abort();
  }

  //
  // Only proceed if there is a request pending
  //
  if (fIPC->fHistoList.fRequestPending==0) return 0;

  fIPC->fHistoList.fServerLock = 1;

  std::list<std::string> hlist;
  this->HistoList(hlist);
  std::list<std::string>::iterator itr(hlist.begin());
  std::list<std::string>::iterator itrEnd(hlist.end());
  unsigned int n=0;
  for (; itr!=itrEnd; ++itr) {
    strcpy(fIPC->fHistoList.fName[n], itr->c_str());
    ++n;
  }
  if (n>=NMAX_HISTOS) {
    std::cerr << __FILE__ << " : " << __LINE__
	      << " Request for " << n << " histograms with limit of " << NMAX_HISTOS
	      << ". Time to update IPC.h and recompile."
	      << std::endl;
    abort();
  }
  fIPC->fHistoList.fN = n;

  fIPC->fHistoList.fServerLock     = 0;
  fIPC->fHistoList.fRequestPending = 0;

  return 1;
}

//......................................................................

int IPC::Publish1DHistogram(const TH1F* h)
{
  fIPC->fHistogram.fHistoOK = 0;
  if (h==0) return 0;

  //
  // Check the histogram name size is within limits
  //
  if (strlen(h->GetName()) >HISTO_NAME_SZ ) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Histogram name size " << strlen(h->GetName())
	      << " exceeds maximum " << HISTO_NAME_SZ << std::endl;
    abort();
  }

  //
  // Test that the title will be within limits and construct it from
  // the global title and the axis labels
  //
  unsigned int tsz =
    4 +                                 // 3 semi colons and \0
    strlen(h->GetTitle()) +             // global title
    strlen(h->GetXaxis()->GetTitle()) + // x-axis title
    strlen(h->GetYaxis()->GetTitle()) + // y-axis title
    strlen(h->GetZaxis()->GetTitle());  // z-axis title
  if (tsz>HISTO_NAME_SZ ) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Histogram title size " << strlen(h->GetTitle())
	      << " exceeds maximum " << HISTO_NAME_SZ << std::endl;
    abort();
  }
  char title[HISTO_NAME_SZ];
  sprintf(title,
	  "%s;%s;%s",
	  h->GetTitle(),
	  h->GetXaxis()->GetTitle(),
	  h->GetYaxis()->GetTitle());

  //
  // Check that the number of bins will not exceed limits
  //
  if (h->fN>HISTO_NBINS_SZ) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Histogram #bins " << h->fN
	      << " exceeds maximum " << HISTO_NBINS_SZ << std::endl;
    abort();
  }

  strcpy(fIPC->fHistogram.fType, "TH1F");
  strcpy(fIPC->fHistogram.fName,  h->GetName());
  strcpy(fIPC->fHistogram.fTitle, title);
  fIPC->fHistogram.fN1 = h->GetNbinsX();
  fIPC->fHistogram.fX1 = h->GetXaxis()->GetXmin();
  fIPC->fHistogram.fX2 = h->GetXaxis()->GetXmax();
  fIPC->fHistogram.fN2 = 0;
  fIPC->fHistogram.fY1 = 0;
  fIPC->fHistogram.fY2 = 0;
  memcpy(fIPC->fHistogram.fData, h->fArray, h->fN*sizeof(float));
  fIPC->fHistogram.fHistoOK = 1;

  return 1;
}

//......................................................................

int IPC::Publish2DHistogram(const TH2F* h)
{
  fIPC->fHistogram.fHistoOK = 0;
  if (h==0) return 0;

  //
  // Check the histogram name size is within limits
  //
  if (strlen(h->GetName())>=HISTO_NAME_SZ ) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Histogram name size " << strlen(h->GetName())
	      << " exceeds maximum " << HISTO_NAME_SZ << std::endl;
    abort();
  }

  //
  // Test that the title will be within limits and construct it from
  // the global title and the axis labels
  //
  unsigned int tsz =
    4 +                                 // 3 semi colons and \0
    strlen(h->GetTitle()) +             // global title
    strlen(h->GetXaxis()->GetTitle()) + // x-axis title
    strlen(h->GetYaxis()->GetTitle()) + // y-axis title
    strlen(h->GetZaxis()->GetTitle());  // z-axis title
  if (tsz>=HISTO_NAME_SZ ) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Histogram title size " << strlen(h->GetTitle())
	      << " exceeds maximum " << HISTO_NAME_SZ << std::endl;
    abort();
  }
  char title[HISTO_NAME_SZ];
  sprintf(title,
	  "%s;%s;%s;%s",
	  h->GetTitle(),
	  h->GetXaxis()->GetTitle(),
	  h->GetYaxis()->GetTitle(),
	  h->GetZaxis()->GetTitle());

  //
  // Check that the number of bins will not exceed limits
  //
  if (h->fN>HISTO_NBINS_SZ) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Histogram #bins " << h->fN
	      << " exceeds maximum " << HISTO_NBINS_SZ << std::endl;
    abort();
  }

  strcpy(fIPC->fHistogram.fType, "TH2F");
  strcpy(fIPC->fHistogram.fName,  h->GetName());
  strcpy(fIPC->fHistogram.fTitle, title);
  fIPC->fHistogram.fN1 = h->GetNbinsX();
  fIPC->fHistogram.fX1 = h->GetXaxis()->GetXmin();
  fIPC->fHistogram.fX2 = h->GetXaxis()->GetXmax();
  fIPC->fHistogram.fN2 = h->GetNbinsY();
  fIPC->fHistogram.fY1 = h->GetYaxis()->GetXmin();
  fIPC->fHistogram.fY2 = h->GetYaxis()->GetXmax();
  memcpy(fIPC->fHistogram.fData, h->fArray, h->fN*sizeof(float));
  fIPC->fHistogram.fHistoOK = 1;

  return 1;
}

//......................................................................

void IPC::ResetAllLocks()
{
  fIPC->fHistoList.fClientLock     = 0;
  fIPC->fHistoList.fServerLock     = 0;
  fIPC->fHistoList.fRequestPending = 0;

  fIPC->fHistogram.fClientLock     = 0;
  fIPC->fHistogram.fServerLock     = 0;
  fIPC->fHistogram.fRequestPending = 0;
  fIPC->fHistogram.fHistoOK        = 0;

}

////////////////////////////////////////////////////////////////////////
