///////////////////////////////////////////////////////////////////////////
/// \brief Simple service to provide SSD Efficiencies
/// \author vo17@illinois.edu
///////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "SSDEfficiency/service/SSDEfficiencyService.h"
#include "RunHistory/service/RunHistoryService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/ParameterSet.h"


namespace emph {
  SSDEfficiencyService::SSDEfficiencyService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg) {
    art::ServiceHandle<runhist::RunHistoryService> rhs;

    fSSDEfficiency = new SSDEfficiency();
    if (pset.get<bool>("IsDisabled"))
      fSSDEfficiency->Disable();
    else
      fSSDEfficiency->Enable();

    reg.sPreBeginRun.watch(this, &SSDEfficiencyService::preBeginRun);
  }

  //----------------------------------------------------------

  void SSDEfficiencyService::preBeginRun(const art::Run&) {
    art::ServiceHandle<runhist::RunHistoryService> rhs;

    std::string effFile = rhs->RunHist()->SSDEfficiencyFile();
    if (fSSDEfficiency->IsDisabled()) {
      // All sensors stay 100% efficient (the SSDEfficiency default); the file is not needed,
      // e.g. runs < 2280 have none
      std::cout << "SSDEfficiencyService: efficiency is disabled, not applying constants from "
                << effFile << std::endl;
      return;
    }
    std::cout << "SSDEfficiencyService: using " << effFile
              << " for SSD efficiency." << std::endl;

    fSSDEfficiency->LoadSSDEffConsts(effFile);
  }
} // end namespace emph
