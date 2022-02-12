////////////////////////////////////////////////////////////////////////
/// \file    RecoDrawingOptions.h
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
///
#ifndef EVD_RECODRAWINGOPTIONS_H
#define EVD_RECODRAWINGOPTIONS_H
#ifndef __CINT__

#include <vector>
#include <string>

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "EventDisplayBase/Reconfigurable.h"


namespace evd {
  /// \brief Global drawing options that apply to all displays
  class RecoDrawingOptions : public evdb::Reconfigurable
  {
  public:
    
    explicit RecoDrawingOptions(fhicl::ParameterSet const& pset,
				art::ActivityRegistry& reg);
    ~RecoDrawingOptions();

    void reconfigure(fhicl::ParameterSet const& pset) override;
    
    // RecoBase Drawing Options
    int fClusterStyle;
    static const int kCLUSTERS_AS_BOXES   = 1<<0;
    static const int kCLUSTERS_AS_MARKERS = 1<<1;
    static const int kCLUSTERS_AS_HULLS   = 1<<2;

    int fProngOpt;
    int fTrackOpt;
    static const int kAS_CLUSTER = 1<<0;
    static const int kAS_PRONG   = 1<<1;
    static const int kAS_TRACK   = 1<<2;

    static const int kORTHO_HITS = 1<<3;

    int fVertexOpt;
    int fOfflineChanOpt;
    int fShowerOpt;
    int fHoughOpt;
    static const int kOFF = 0;
    static const int kON = 1;

    static const int kHOUGH_COLORS = 2;

    std::vector<std::string> fCellHitModules;     ///< Module labels for CellHits
    std::vector<std::string> fCellHitInstances;   ///< productInstance labels for CellHits

    std::vector<std::string> fClusterModules;     ///< Module labels for Clusters
    std::vector<std::string> fClusterInstances;   ///< productInstance labels for Clusters

    std::vector<std::string> fOfflineChanModules;   ///< Module labels for OfflineChans
    std::vector<std::string> fOfflineChanInstances; ///< productInstance labels for OfflineChans

    std::vector<std::string> fHoughResultModules; ///< Module labels for HoughResults
    std::vector<std::string> fHoughResultInstances; ///< productInstance labels for HoughResults

    std::vector<std::string> fVertexModules;      ///< Module labels for Vertices
    std::vector<std::string> fVertexInstances;    ///< productInstance labels for Vertexes

    std::vector<std::string> fProngModules;       ///< Module labels for Prongs
    std::vector<std::string> fProngInstances;     ///< productInstance labels for Prongs

    std::vector<std::string> fTrackModules;       ///< Module labels for Tracks
    std::vector<std::string> fTrackInstances;     ///< productInstance labels for Tracks

    std::vector<std::string> fShowerModules;      ///< Module labels for Showers
    std::vector<std::string> fShowerInstances;    ///< productInstance labels for Showers

    std::vector<std::vector<int> > fClusterIndex;
    std::vector<std::vector<int> > fTrackIndex;
    std::vector<std::vector<int> > fVertexIndex;
    std::vector<std::vector<int> > fProngIndex;

  protected:
    /// \brief Parse apart a "module_instance" string
    ///
    /// Input the joined string as \a mod. If there is no underscore, \a mod is
    /// unaltered and \a inst is set to the empty string (""). If there is an
    /// underscore, \a mod is set to the part before the underscore, and \a inst
    /// to the part after.
    void ParseModuleInstanceString(std::string& mod,
                                   std::string& inst) const;
    
    void ModuleList(fhicl::ParameterSet const& pset,
		    std::vector<std::string>& modules,
		    std::vector<std::string>& instances,
		    const char* object);
      };
}
#endif // __CINT__
DECLARE_ART_SERVICE(evd::RecoDrawingOptions, LEGACY)
#endif
////////////////////////////////////////////////////////////////////////
