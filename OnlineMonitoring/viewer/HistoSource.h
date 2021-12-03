#ifndef OM_HISTOFILE_H
#define OM_HISTOFILE_H
#include <list>
#include <string>
class TObject;
class TH1F;
class TH2F;
class TFile;
namespace om { class IPC; }

namespace om {
  ///
  /// Define the base class for any source (root files, shared memory,
  /// etc...) of histograms to be displayed by the viewer.
  ///
  class HistoSourceBase
  {
  public:
    virtual ~HistoSourceBase() { }
  public:
    /// Set the source name.
    virtual void SetName(const char* nm) = 0;

    /// Read a copy of a named histogram into memory. User accepts
    /// ownership and responsibility for deletetion
    virtual TH1F* GetTH1FCopy(const char* n) = 0;

    /// Read a copy of a named histogram into memory. User accepts
    /// ownership and responsibility for deletion.
    virtual TH2F* GetTH2FCopy(const char* n) = 0;

    /// Get a list of all histograms held by this source
    virtual void GetHistogramList(std::list<std::string>& h) = 0;

    /// Get the histogram server status
    virtual void GetStatus(std::string& nm __attribute__((unused)),
			   unsigned int* run    __attribute__((unused)),
			   unsigned int* subrun __attribute__((unused)),
			   unsigned int* event  __attribute__((unused)),
			   pid_t*        pid    __attribute__((unused)),
			   long*         stime  __attribute__((unused)),
			   long*         utime  __attribute__((unused)),
			   long*         cpu    __attribute__((unused)),
			   long*         rsize  __attribute__((unused)),
			   time_t*       t      __attribute__((unused))) { }

  public:
    /// Return the names of all objects in the file who's names match
    /// a particular set of patterns
    ///
    /// \param include_pattern - Names must match this expression
    /// \param exclude_pattern - Names must not match this expression
    /// \param objs - List of object names that satisfy the above
    ///
    void FindAllMatches(const char* include_pattern,
			const char* exclude_pattern,
			std::list<std::string>& objs);
  };

  ///
  /// Histograms from a root file
  ///
  class HistoFile : public HistoSourceBase
  {
  public:
    HistoFile(const char* n);
    ~HistoFile();
  public:
    void  SetName(const char* nm);
    TH1F* GetTH1FCopy(const char* n);
    TH2F* GetTH2FCopy(const char* n);
    void  GetHistogramList(std::list<std::string>& h);
  private:
    TFile* fFile; ///< The root histogram file
  };

  ///
  /// Histograms from a shared memory segment
  ///
  class HistoSHM  : public HistoSourceBase
  {
  public:
    HistoSHM(const char* n);
    ~HistoSHM();
  public:
    void  SetName(const char* nm);
    TH1F* GetTH1FCopy(const char* n);
    TH2F* GetTH2FCopy(const char* n);
    void  GetHistogramList(std::list<std::string>& h);
    void  GetStatus(std::string& nm,
		    unsigned int* run,
		    unsigned int* subrun,
		    unsigned int* event,
		    pid_t*        pid,
		    long*         stime,
		    long*         utime,
		    long*         cpu,
		    long*         rsize,
		    time_t*       t);
  private:
    IPC* fIPC; ///< Shared memory handler
  };

  ///
  /// The generic source. Figures out which of the above to use based
  /// on the . extension
  ///
  class HistoSource : public HistoSourceBase
  {
  public:
    HistoSource(const char* n);
    ~HistoSource();
  public:
    void  SetName(const char* nm);
    TH1F* GetTH1FCopy(const char* n);
    TH2F* GetTH2FCopy(const char* n);
    void  GetHistogramList(std::list<std::string>& h);
    void  GetStatus(std::string& nm,
		    unsigned int* run,
		    unsigned int* subrun,
		    unsigned int* event,
		    pid_t*        pid,
		    long*         stime,
		    long*         utime,
		    long*         cpu,
		    long*         rsize,
		    time_t*       t);
  private:
    HistoSourceBase* fSource;
  };
}

#endif
////////////////////////////////////////////////////////////////////////
