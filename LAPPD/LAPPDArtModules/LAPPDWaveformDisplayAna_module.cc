////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to dump LAPPD waveform data
/// \author  fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"

namespace emph {
  
  namespace lappdana {
  
    class LAPPDWaveformDisplayAna : public art::EDAnalyzer {

    public:
      explicit LAPPDWaveformDisplayAna(fhicl::ParameterSet const& pset);
      
      void analyze(const art::Event& evt);
      void beginJob();
      void endJob();
      
    private:
      // Input parameters
      std::string fInputLabel; ///< Input label for LAPPD raw digits
      int fVerbosity; ///< Verbosity level for logging

      // Event information
      int fRunId;
      int fSubRunId;
      int fEventId;

      // TTree for storing event information
      TTree* fTree; ///< Tree to store event information
      std::vector<int> fChannelNumbers; ///< Vector to store channel numbers
      std::vector< std::vector<unsigned int> > fWaveforms; ///< Vector to store waveforms for each channel
    };

  } // end namespace lappdana
} // end namespace emph



//.......................................................................  
emph::lappdana::LAPPDWaveformDisplayAna::LAPPDWaveformDisplayAna(fhicl::ParameterSet const& pset):
  EDAnalyzer(pset)
  , fInputLabel(pset.get<std::string>("InputLabel"))
  , fVerbosity(pset.get<int>("Verbosity", 0))
{
}

    
//......................................................................
void emph::lappdana::LAPPDWaveformDisplayAna::endJob()
{ 
}

//......................................................................
void emph::lappdana::LAPPDWaveformDisplayAna::beginJob()
{
  // Initialize event information
  fRunId = -1;
  fSubRunId = -1;
  fEventId = -1;

  // Create a TTree to store event information
  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("LAPPDWaveformDisplayAnaTree", "LAPPD Waveform Display Analysis Tree");
  fTree->Branch("RunId", &fRunId, "RunId/I");
  fTree->Branch("SubRunId", &fSubRunId, "SubRunId/I");
  fTree->Branch("EventId", &fEventId, "EventId/I");
  fTree->Branch("ChannelNumbers", &fChannelNumbers);
  fTree->Branch("Waveforms", &fWaveforms);
}


//......................................................................  
void emph::lappdana::LAPPDWaveformDisplayAna::analyze(const art::Event& evt)
{ 

  fRunId = evt.run();
  fSubRunId = evt.subRun();
  fEventId = evt.id().event();
  mf::LogInfo("LAPPDWaveformDisplayAna") << "Processing event: "
                                           << "Run: " << fRunId
                                           << ", SubRun: " << fSubRunId
                                           << ", Event: " << fEventId;
  
  

  // Create directory for this run-subrun-event
  art::ServiceHandle<art::TFileService> tfs;
  std::string dirName = "Run_" + std::to_string(fRunId) + "_SubRun_" + std::to_string(fSubRunId) + "_Event_" + std::to_string(fEventId);
  art::TFileDirectory dir = tfs->mkdir(dirName);

  // Reset channel numbers and waveforms for this event
  fChannelNumbers.clear();
  fWaveforms.clear();
  
  // Get the LAPPD raw digits from the event
  art::Handle< std::vector<lappd::LAPPDRawDigit> > lappdHandle;
  evt.getByLabel(fInputLabel, lappdHandle);
  if (!lappdHandle.isValid()) {
    mf::LogError("LAPPDWaveformDisplayAna") << "No LAPPD raw digits found in event " << fEventId
                                             << " in Run: " << fRunId
                                             << ", SubRun: " << fSubRunId;
    return;
  }
  else{
    // Printout with number of LAPPD raw digits
    mf::LogInfo("LAPPDWaveformDisplayAna") << "Found " << lappdHandle->size() << " LAPPD raw digits in event "
                                           << fEventId << " in Run: " << fRunId
                                           << ", SubRun: " << fSubRunId;

    int lappdCounter = 0;
    for( const auto& lappdDigit : *lappdHandle) {
      mf::LogInfo("LAPPDWaveformDisplayAna") << "LAPPD raw digit with ACDC number: " 
                                             << lappdDigit.GetACDCNumber() 
                                             << ", event number: " << lappdDigit.GetEventNumber()
                                             << ", timestamp: " << lappdDigit.GetTimeStamp();
      lappdCounter++;
      // Loop over channels
      for (int channel = 0; channel < lappdDigit.GetNChannels(); ++channel) {
        const lappd::LAPPDReadoutChannelRawDigit& channelData = lappdDigit.GetChannel(channel);
        mf::LogInfo("LAPPDWaveformDisplayAna") << "  Channel " << channel;
        
        // Create a histogram for this channel
        TH1F* hWaveform = dir.make<TH1F>( ( "histLAPPD-"+std::to_string(lappdCounter) +
                                            "_ADCCh-" + std::to_string(channel)).c_str(), 
                                          ( "LAPPD " + std::to_string(lappdCounter) +
                                            "Channel " + std::to_string(channel) + " ADC Waveform; Tick; ADC Value").c_str(),
                                          channelData.NADCs(), 0, channelData.NADCs() );
        hWaveform->SetStats(0); // Disable stats box for cleaner display
      
        fWaveforms.push_back(std::vector<unsigned int>(channelData.NADCs()));
        fChannelNumbers.push_back(channel);

        // Print ADC values for this channel
        if (fVerbosity > 1) {
          // Fill the histogram with ADC values
          std::cout << "  ADC values: ";
          for (int tick = 0; tick < channelData.NADCs(); ++tick) {
            std::cout << channelData.ADC(tick) << " ";
          }
          std::cout << std::endl;
        }

        // Fill the histogram with ADC values
        for (int tick = 0; tick < channelData.NADCs(); ++tick) {
          hWaveform->SetBinContent(tick, channelData.ADC(tick));
          fWaveforms.back()[tick] = channelData.ADC(tick); // Store ADC value in waveforms vector
        }

      }
    }                                       
  }
  

  // Fill the TTree with event information
  fTree->Fill();
}


DEFINE_ART_MODULE(emph::lappdana::LAPPDWaveformDisplayAna)