////////////////////////////////////////////////////////////////////////
/// \file    RawDrawingOptions.h
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
/// \version $Id: RawDrawingOptions.h,v 1.8 2012-07-19 04:21:45 ksachdev Exp $
///
#ifndef EVD_RAWDRAWINGOPTIONS_H
#define EVD_RAWDRAWINGOPTIONS_H
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
  class RawDrawingOptions : public evdb::Reconfigurable
  {
  public:
    
    explicit RawDrawingOptions(fhicl::ParameterSet const& pset,
			       art::ActivityRegistry& reg);
    ~RawDrawingOptions();
    
    void reconfigure(fhicl::ParameterSet const& pset) override;

    int fColor;
    static const int kCOLOR_BY_CHARGE = 0;
    static const int kCOLOR_BY_TIME   = 1;

    int fWhichHits;
    static const int kRAW_HITS = 0;
    static const int kCAL_HITS = 1;

    int fWhichQ;
    static const int kPE    = 0;
    static const int kPECOR = 1;

    int fRawDrawingOpt;
    static const int kMASK_BAD_CHANNELS     = 1<<0;
    static const int kSCALE_HITS_BY_CHARGE  = 1<<1;
    static const int kSUPPRESS_GHOSTED_HITS = 1<<2;
    static const int kSUPPRESS_DIMMED_HITS  = 1<<3;

    float              fScaleFactor;
    std::vector<float> fTimeRange;
    float              fTimeBinSize;
    int                fTimeAutoZoomTruth;

    int                fTHistogram;

    // Modules and instance labels
    std::vector<std::string> fRawDigitsModules;
    std::vector<std::string> fRawDigitsInstances;
    std::vector<std::string> fCellHitsModules;
    std::vector<std::string> fCellHitsInstances;
    
    std::vector<float> fADCRange;
    float              fADCBinSize;

    int fHit3DStyle;
    static const int kHIT3D_BOXES     = 1<<0;
    static const int kHIT3D_TOWERS    = 1<<1;
    static const int kHIT3D_CROSSINGS = 1<<2;
  protected:

    /// \brief Parse apart a "module_instance" string. Input the
    /// joined string as mod. If there is no underscore, mod is
    /// unaltered and inst is set to the empty string (""). If
    /// there is an underscore, mod is set to the part before
    /// the underscore, and inst to the part after.
    void ParseModuleInstanceString(std::string& mod,
                                   std::string& inst) const;

    void ModuleList(fhicl::ParameterSet const& pset,
		    std::vector<std::string>& modules,
		    std::vector<std::string>& instances,
		    const char* object);
    
  };
}
#endif // __CINT__
DECLARE_ART_SERVICE(evd::RawDrawingOptions, LEGACY)
#endif
////////////////////////////////////////////////////////////////////////
