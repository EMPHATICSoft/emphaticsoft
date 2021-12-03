///
/// \file    IPC.h
/// \brief   Class to manage the shared memory segment
/// \version $Id: IPC.h,v 1.6 2012-11-14 07:37:03 messier Exp $
/// \author  messier@indiana.edu
///
#ifndef OM_IPC_H
#define OM_IPC_H
extern "C" {
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
}
#include <list>
#include <string>
#include <vector>
class TObject;
class TH1F;
class TH2F;
namespace om { struct IPCBlock; }

namespace om {
  static const int kIPC_SERVER = 0x01;
  static const int kIPC_CLIENT = 0x02;

  ///
  /// Class to manage the shared memory segment
  ///
  class IPC {
  public:
    IPC(int mode, const char* shm_handle);
    virtual ~IPC();

    ///
    /// Method for server application check to see if there are any
    /// requests pending
    ///
    int HandleRequests();

    ///
    /// Post information about unpacker resource usage
    ///
    void PostResources(unsigned int run,
		       unsigned int subrun,
		       unsigned int event);

    ///
    /// Client calls to request a histogram from the server.
    /// \param   n - name of histogram to retrieve
    /// \returns A new histogram. Called takes ownership
    ///
    TH1F* RequestTH1F(const char* n);
    TH2F* RequestTH2F(const char* n);

    ///
    /// Client call to request the histogram list
    ///
    int RequestHistoList(std::list<std::string>& hlist);

    ///
    /// For "emergency use" only. Clear all locks in the event they
    /// get stuck.
    ///
    void ResetAllLocks();

  private:
    //
    // Find a ROOT histogram given a name. Derived class must
    // implement this based on its storage strategy for histograms.
    //
    virtual TH1F* FindTH1F(const char* nm __attribute__((unused))) { return 0; }
    virtual TH2F* FindTH2F(const char* nm __attribute__((unused))) { return 0; }

    //
    // Fill a list with the names of all the histograms held by the
    // server. Derived class must implement this based on its storage
    // strategy for histograms
    //
    virtual void HistoList(std::list<std::string>& hlist __attribute__((unused))) { abort(); }

    //
    // Log a request for a histogram named n of type t
    //
    int Request(const char* n, const char* t);

    //
    // Create a new histogram based on the data in the shared memory
    // bank and return it.
    //
    TH1F* UnpackTH1F();
    TH2F* UnpackTH2F();

    //
    // Push data from a ROOT histogram into the shared memory segment
    //
    int Publish1DHistogram(const TH1F* obj);
    int Publish2DHistogram(const TH2F* obj);

    //
    // Process requests
    //
    int HandleHistogramRequests();
    int HandleHistoListRequests();


  public:
    static const int kHdlSz = sizeof(key_t)+1;
    int              fMode;           ///< Client or server?
    int              fShmId;          ///< ID of shared memory segment
    key_t*           fKey;            ///< Shared memory key
    char             fShmHdl[kHdlSz]; ///< Shared memory handle
    void*            fShm;            ///< Pointer to shared segment
    struct IPCBlock* fIPC;            ///< Pointer to struct
  };
}

#endif
////////////////////////////////////////////////////////////////////////
