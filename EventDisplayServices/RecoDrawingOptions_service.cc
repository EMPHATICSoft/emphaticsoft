///
/// \file    RecoDrawingOptions.cxx
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
///
#include "EventDisplayServices/RecoDrawingOptions.h"
#include <algorithm>

namespace evd
{
  RecoDrawingOptions::RecoDrawingOptions(fhicl::ParameterSet const& pset, 
					 art::ActivityRegistry& ) // reg)
    : evdb::Reconfigurable{pset}
  {
    this->reconfigure(pset);
  }

  //....................................................................
  RecoDrawingOptions::~RecoDrawingOptions()
  {
  }

  //....................................................................
  static bool is_empty(const std::string& s)
  {
    return (s=="" || s==" "); 
  }

  //....................................................................
  void RecoDrawingOptions::ModuleList(fhicl::ParameterSet const& pset,
				      std::vector<std::string>& modules,
				      std::vector<std::string>& instances,
				      const char* object) 
  {
    unsigned int i;

    std::string A(object);
    std::string B(object);
    
    A += ".val";
    B += "Add.val";
      
    std::vector<std::string> a=pset.get<std::vector<std::string> >(A.c_str());
    std::vector<std::string> b=pset.get<std::vector<std::string> >(B.c_str());

    modules.clear();
    modules.insert(modules.end(), a.begin(), a.end());
    modules.insert(modules.end(), b.begin(), b.end());

    auto newEnd = std::remove_if(modules.begin(), modules.end(), is_empty);
    modules.erase(newEnd, modules.end());

    instances.resize(modules.size());
    for(i=0; i<modules.size(); ++i) {
      ParseModuleInstanceString(modules[i], instances[i]);
    }
  }
  
  //....................................................................  
  void RecoDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
  {
    fClusterStyle     = pset.get<int>("ClusterStyle.val");
    fProngOpt         = pset.get<int>("ProngOpt.val");
    fTrackOpt         = pset.get<int>("TrackOpt.val");
    fVertexOpt        = pset.get<int>("VertexOpt.val");
    fOfflineChanOpt   = pset.get<int>("OfflineChanOpt.val");
    fShowerOpt        = pset.get<int>("ShowerOpt.val");
    fHoughOpt         = pset.get<int>("HoughOpt.val");

    fClusterIndex = pset.get< std::vector< std::vector<int> > >("ClusterIndex.val");
    fTrackIndex   = pset.get< std::vector< std::vector<int> > >("TrackIndex.val");
    fVertexIndex  = pset.get< std::vector< std::vector<int> > >("VertexIndex.val");
    fProngIndex   = pset.get< std::vector< std::vector<int> > >("ProngIndex.val");

    this->ModuleList(pset, fCellHitModules,     fCellHitInstances,     "CellHits");
    this->ModuleList(pset, fHoughResultModules, fHoughResultInstances, "Hough");
    this->ModuleList(pset, fClusterModules,     fClusterInstances,     "Clusters");
    this->ModuleList(pset, fProngModules,       fProngInstances,       "Prongs");
    this->ModuleList(pset, fVertexModules,      fVertexInstances,      "Vertices");
    this->ModuleList(pset, fTrackModules,       fTrackInstances,       "Tracks");
    this->ModuleList(pset, fShowerModules,      fShowerInstances,      "Showers");
    this->ModuleList(pset, fOfflineChanModules, fOfflineChanInstances, "OfflineChans");
  }
  
  //......................................................................
  void RecoDrawingOptions::ParseModuleInstanceString(std::string& mod,
                                                     std::string& inst) const
  {
    const size_t splitPos = mod.find_first_of('_');
    if(splitPos == std::string::npos){
      inst = "";
      return;
    }
    inst = mod.substr(splitPos+1);
    mod.resize(splitPos);
  }

  //......................................................................
  
  DEFINE_ART_SERVICE(RecoDrawingOptions)

} // end namespace evd
////////////////////////////////////////////////////////////////////////
