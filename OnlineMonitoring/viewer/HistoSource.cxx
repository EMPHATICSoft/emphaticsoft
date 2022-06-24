#include "OnlineMonitoring/viewer/HistoSource.h"
#include <iostream>
#include "TFile.h"
#include "OnlineMonitoring/util/IPC.h"
#include "OnlineMonitoring/util/IPCBlock.h"
#include "OnlineMonitoring/util/RegexMatch.h"
#include "OnlineMonitoring/viewer/OStream.h"
using namespace emph::onmon;

void HistoSourceBase::FindAllMatches(const char* include_pattern,
				     const char* exclude_pattern,
				     std::list<std::string>& objs) 
{
  std::string include_p(include_pattern);
  std::string exclude_p(exclude_pattern);
  bool in, ex;

  std::list<std::string> h;
  this->GetHistogramList(h);

  std::list<std::string>::iterator i(h.begin());
  std::list<std::string>::iterator iend(h.end());
  for (; i!=iend; ++i) {
    in = (regex_match(*i, include_p)==0);
    ex = (regex_match(*i, exclude_p)==0);
      if (in && !ex) objs.push_back(*i);
  }
}

//......................................................................

HistoFile::HistoFile(const char* n) : fFile(0)
{
  if (n) this->SetName(n);
}

//......................................................................

HistoFile::~HistoFile() 
{
  if (fFile) { delete fFile; fFile = 0; }
}

//......................................................................

void HistoFile::SetName(const char* nm) 
{
  if (fFile) { delete fFile; fFile = 0; }
  fFile = new TFile(nm, "READ");
  if (fFile==0) {
    emph::onmon::cout << "Unable to open root file " << nm << " for read." << "";
  }
}

//......................................................................

TH1F* HistoFile::GetTH1FCopy(const char* n) 
{
  TObject* obj = fFile->FindObjectAny(n);
  if (obj==0) return 0;
  return (TH1F*)obj;
}

//......................................................................

TH2F* HistoFile::GetTH2FCopy(const char* n) 
{
  TObject* obj = fFile->FindObjectAny(n);
  if (obj==0) return 0;
  return (TH2F*)obj;
}

//......................................................................

void HistoFile::GetHistogramList(std::list<std::string>& h)
{
  TIter next(fFile->GetListOfKeys());
  TObject* obj = 0;
  while ((obj = next())) {
    h.push_back(obj->GetName());
  }
}

//......................................................................

HistoSHM::HistoSHM(const char* n) : fIPC(0) 
{
  this->SetName(n);
}

//......................................................................

HistoSHM::~HistoSHM() 
{
  if (fIPC) { delete fIPC; fIPC = 0; }
}

//......................................................................

void HistoSHM::GetStatus(std::string& nm,
			 unsigned int* run,
			 unsigned int* subrun,
			 unsigned int* event,
			 pid_t*        pid,
			 long*         stime,
			 long*         utime,
			 long*         cpu,
			 long*         rsize,
			 time_t*       t)
{
  nm = fIPC->fShmHdl;
  if (run)    *run    = fIPC->fIPC->fProcessResources.fRun;
  if (subrun) *subrun = fIPC->fIPC->fProcessResources.fSubrun;
  if (event)  *event  = fIPC->fIPC->fProcessResources.fEvent;
  if (pid)    *pid    = fIPC->fIPC->fProcessResources.fPID;
  if (stime)  *stime  = fIPC->fIPC->fProcessResources.fSTIME;
  if (utime)  *utime  = fIPC->fIPC->fProcessResources.fUTIME;
  if (cpu)    *cpu    = fIPC->fIPC->fProcessResources.fCPU;
  if (rsize)  *rsize  = fIPC->fIPC->fProcessResources.fRSIZE;
  if (t)      *t      = fIPC->fIPC->fProcessResources.fCLOCK;
}

//......................................................................

void  HistoSHM::SetName(const char* nm) 
{
  //if (fIPC) { delete fIPC; fIPC=0; }
  fIPC = new IPC(kIPC_CLIENT, nm);
}

//......................................................................

TH1F* HistoSHM::GetTH1FCopy(const char* n) 
{
  return fIPC->RequestTH1F(n);
}

//......................................................................

TH2F* HistoSHM::GetTH2FCopy(const char* n) 
{
  return fIPC->RequestTH2F(n);
}

//......................................................................

void HistoSHM::GetHistogramList(std::list<std::string>& h) 
{
  fIPC->RequestHistoList(h);
}

//......................................................................

HistoSource::HistoSource(const char* n) : fSource(0)
{
  this->SetName(n);
}

//......................................................................

HistoSource::~HistoSource() 
{
  if (fSource) {
    delete fSource;
    fSource = 0;
  }
}

//......................................................................

void HistoSource::SetName(const char* nm) 
{
  std::string s(nm);
  bool is_root = s.find(".root")<s.length();
  bool is_shm  = s.find(".shm")< s.length();
  if (is_root&&is_shm) abort();

  if (is_root) fSource = new HistoFile(nm);
  if (is_shm)  fSource = new HistoSHM(nm);
}

//......................................................................

TH1F* HistoSource::GetTH1FCopy(const char* n) 
{
  if (fSource) return fSource->GetTH1FCopy(n);
  return 0;
}

//......................................................................

TH2F* HistoSource::GetTH2FCopy(const char* n) 
{
  if (fSource) return fSource->GetTH2FCopy(n);
  return 0;
}

//......................................................................

void HistoSource::GetHistogramList(std::list<std::string>& h) 
{
  if (fSource) fSource->GetHistogramList(h);
}

//......................................................................

void HistoSource::GetStatus(std::string& nm,
			    unsigned int* run,
			    unsigned int* subrun,
			    unsigned int* event,
			    pid_t*        pid,
			    long*         stime,
			    long*         utime,
			    long*         cpu,
			    long*         rsize,
			    time_t*       t) 
{
  if (fSource) {
    fSource->GetStatus(nm, run, subrun, event, 
		       pid, stime, utime, cpu, rsize, t);
  }
}

////////////////////////////////////////////////////////////////////////
