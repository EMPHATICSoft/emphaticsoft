///
/// \file    RawDrawingOptions.cxx
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
///
#include "EventDisplayServices/RawDrawingOptions.h"


namespace evd
{
  //....................................................................  
  RawDrawingOptions::RawDrawingOptions(fhicl::ParameterSet const& pset, 
				       art::ActivityRegistry& ) //reg)
    : evdb::Reconfigurable{pset}
  {
    this->reconfigure(pset);
  }

  //....................................................................
  RawDrawingOptions::~RawDrawingOptions()
  {
  }

  //....................................................................
  static bool is_empty(const std::string& s)
  {
    return (s=="" || s==" "); 
  }

  //....................................................................
  void RawDrawingOptions::ModuleList(fhicl::ParameterSet const& pset,
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
  void RawDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
  {
    fColor               = pset.get<int>("Color.val");
    fWhichHits           = pset.get<int>("WhichHits.val");
    fWhichQ              = pset.get<int>("WhichQ.val");
    fRawDrawingOpt       = pset.get<int>("RawDrawingOpt.val");
    fScaleFactor         = pset.get<float>("ScaleFactor.val");
    fTimeRange           = pset.get< std::vector<float> >("TimeRange.val");
    fTimeBinSize         = pset.get<float>("TimeBinSize.val");
    fTimeAutoZoomTruth   = pset.get<int>("TimeAutoZoomTruth.val");
    fADCRange            = pset.get< std::vector<float> >("ADCRange.val");
    fADCBinSize          = pset.get<float>("ADCBinSize.val");
    fHit3DStyle          = pset.get<int>("Hit3DStyle.val");
    fTHistogram          = pset.get<int>("THistogram.val");

    // Ensure sensical ranges and bin size.
    if ( fTimeBinSize <= 0 ) fTimeBinSize = 0.01; //10 ns 
    if ( fTimeRange[0] >= (fTimeRange[1]-fTimeBinSize)) fTimeRange[1] = fTimeRange[0] + 10*fTimeBinSize;
    if ( (fTimeRange[1]-fTimeRange[0]) < fTimeBinSize ) fTimeBinSize = fTimeRange[1]-fTimeRange[2];

    if ( fADCBinSize < 1) fADCBinSize = 1;
    if ( fADCRange[0] >= (fADCRange[1]-fADCBinSize)) fADCRange[1] = fADCRange[0] + 10*fADCBinSize;
    if ( (fADCRange[1]-fADCRange[0]) < fADCBinSize ) fADCBinSize = fADCRange[1]-fADCRange[2];

    this->ModuleList(pset, fRawDigitsModules,     fRawDigitsInstances,     "RawDigitsModules");
    this->ModuleList(pset, fCellHitsModules, fCellHitsInstances, "CellHitsModules");
  }
  
  //....................................................................
  void RawDrawingOptions::ParseModuleInstanceString(std::string& mod,
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

  DEFINE_ART_SERVICE(RawDrawingOptions)

} // end namespace evd
////////////////////////////////////////////////////////////////////////
