///////////////////////////////////////////////////////////////////////
// Class:       LAPPDEventGen
// Module Type: input source
// Author:      fnicolas@fnal.gov
// Description: start with a txt LAPPD binary file, and create art::Events
//              with decoded LAPPD raw digits objects
////////////////////////////////////////////////////////////////////////


#include "LAPPD/LAPPDEventGen_source.hh"


namespace emph {

  namespace rawdata {

    /***************************************************************************/
    LAPPDEventGen::LAPPDEventGen(fhicl::ParameterSet const& ps, art::ProductRegistryHelper& help, art::SourceHelper const& pm) :
      fIsFirst(true)
      , fVerbosity(ps.get<int>("Verbosity", 0))
      , fRun(1)
      , fSubrun(1)
      , fNEvents(ps.get<int64_t>("NEvents", -1))
      , fEvtCount(0)
      , fSourceHelper(pm)

    {
      if(fVerbosity > 0) {
        mf::LogInfo("LAPPDEventGen") << "  LAPPDEventGen constructor called with Verbosityy = " << fVerbosity << " and NEvents = " << fNEvents;
      }

      // Hist file
      art::ServiceHandle<art::TFileService> tfs;
      fTree = tfs->make<TTree>("LAPPDEventGenTree", "LAPPD Event Generation Tree");
      fTree->Branch("Run", &fRun, "Run/I");
      fTree->Branch("Subrun", &fSubrun, "Subrun/I");

      // Define art output
      help.reconstitutes<std::vector<lappd::LAPPDRawDigit>, art::InEvent>("raw", "LAPPD");
    }

    /***************************************************************************/
    void LAPPDEventGen::closeCurrentFile()
    {

    }

    /***************************************************************************/
    void LAPPDEventGen::readFile(std::string const &name, art::FileBlock* &fb)
    {

      fCurrentFilename = name;
      fb = new art::FileBlock(art::FileFormatVersion{1, "LAPPDRawEventACC"}, fCurrentFilename);

      std::string fileName = name;
      if (name.find_last_of('/') != std::string::npos) {
        fileName = name.substr(name.find_last_of('/')+1);
      }

      sscanf(fileName.c_str(),"r%d_sr%d.dat",&fRun,&fSubrun);
      if (fVerbosity > 0) {
        mf::LogInfo("LAPPDEventGen") << "  LAPPDEventGen::readFile called for run " << fRun
                                     << ", subrun " << fSubrun
                                     << ", file: " << fCurrentFilename;
      }

      // Initialize the unpacker with the file handle
      fUnpacker.setFileHandle(fCurrentFilename);
      fUnpackerEvtIx = 0; // Reset event index for unpacker
      if (fVerbosity > 0) {
        mf::LogInfo("LAPPDEventGen") << "  LAPPDEventGen::readFile initialized unpacker for file: " << fCurrentFilename 
                                     << " with NEvents = " << fUnpacker.getNEvents();
      }
    }

    bool LAPPDEventGen::readNext(art::RunPrincipal* const& ,//inR,
			  art::SubRunPrincipal* const& ,//inSR,
			  art::RunPrincipal* & outR,
			  art::SubRunPrincipal* & outSR,
			  art::EventPrincipal* &outE)
    {

      // Create the LAPPD raw digit data to be filled in the event
      std::unique_ptr< std::vector<lappd::LAPPDRawDigit> > lappdDigitsPtr(new std::vector<lappd::LAPPDRawDigit>);
      std::vector<lappd::LAPPDRawDigit> lappdRawDigits;

      // Check if we have reached the end of the event generation
      if (fNEvents > 0)
        if (fEvtCount == fNEvents)
	        return false;

      // Printouts
      if (fVerbosity > 1) {
        mf::LogInfo("LAPPDEventGen") << "  LAPPDEventGen::readNext called for eventcount " << fEvtCount
                                     << " in run " << fRun
                                     << ", subrun " << fSubrun;
      }
      if ((fEvtCount%10) == 0 && fVerbosity > 0){
        mf::LogInfo("LAPPDEventGen") << "  LAPPDEventGen::readNext called for eventcount " << fEvtCount
                                       << " in run " << fRun
                                       << ", subrun " << fSubrun;
      }

      // First time we are called, we need to create the run and subrun principals
      if (fIsFirst) {
        outR = fSourceHelper.makeRunPrincipal(fRun,0); //runAux.beginTime());
        outSR = fSourceHelper.makeSubRunPrincipal(fRun, fSubrun, fSubrun*60); //subrunAux.beginTime());
        fIsFirst = false;
      }

      // Create the event principal
      art::Timestamp evtTime(0); // + earliestTimestamp);
      outE = fSourceHelper.makeEventPrincipal(fRun, fSubrun, fEvtCount++, evtTime);

      if (fVerbosity > 1) {
        mf::LogInfo("LAPPDEventGen") << "  Creating event " << fEvtCount
                                     << " in run " << fRun
                                     << ", subrun " << fSubrun;
      }

      lappd::LAPPDRawDigit lappdRawDigit = fUnpacker.readNextEvent();
      lappdRawDigits.push_back(lappdRawDigit);
      lappdDigitsPtr->emplace_back( std::move(lappdRawDigit) );
      fUnpackerEvtIx++;

      if (fVerbosity > 1) {
        std::cout << "  LAPPD raw digit created for event " << fEvtCount
                                     << " with ACDC number: " << (int) lappdRawDigit.GetACDCNumber()
                                     << ", event number: " << lappdRawDigit.GetEventNumber()
                                     << ", timestamp: " << lappdRawDigit.GetTimeStamp()
                                     << ", with internal event index: " << lappdRawDigit.GetEventNumber();

        for (int i = 0; i < lappdRawDigit.GetNChannels(); ++i) {
          lappd::LAPPDReadoutChannelRawDigit channel = lappdRawDigit.GetChannel(i);
          std::cout << "  Channel " << i << ": ADC values: ";
          for (int tick = 0; tick < channel.NADCs(); ++tick) {
            std::cout << channel.ADC(tick) << " ";
          }
          std::cout << std::endl;
        }

      }

    

      if( !lappdDigitsPtr->empty() ) {
        mf::LogInfo("LAPPDEventGen") << "  LAPPD raw digits successfully put in event " << lappdDigitsPtr->size()
                                     << " with ACDC number: " << (int) lappdDigitsPtr->at(0).GetACDCNumber()
                                     << ", event number: " << lappdDigitsPtr->at(0).GetEventNumber()
                                     << ", timestamp: " << lappdDigitsPtr->at(0).GetTimeStamp();

        put_product_in_principal(std::move( lappdDigitsPtr), *outE, "raw", "LAPPD");	
      } 
      else {
        mf::LogWarning("LAPPDEventGen") << "  No LAPPD raw digits created for event " << fEvtCount;
      }


      return true;
    }
  
  }
}


DEFINE_ART_INPUT_SOURCE(art::Source<emph::rawdata::LAPPDEventGen>)

