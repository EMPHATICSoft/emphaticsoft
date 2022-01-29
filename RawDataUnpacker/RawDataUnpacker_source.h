#ifndef RAWDATA_UNPACKER
#define RAWDATA_UNPACKER

#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "artdaq-core/Data/RawEvent.hh"
#include "artdaq-core/Utilities/TimeUtils.hh"
#include "artdaq-core/Data/Fragment.hh"
#include "fhiclcpp/types/Atom.h"

#include "TTree.h"
#include "TH1I.h"
#include <string>
#include <vector>

namespace emph {
  namespace rawdata {

    class Unpacker
    {
    public:
      Unpacker(Unpacker const&) = delete;
      Unpacker& operator=(Unpacker const&) = delete;

      explicit Unpacker(fhicl::ParameterSet const& ps,
			art::ProductRegistryHelper& help,
			art::SourceHelper const& pm);
      
      virtual ~Unpacker() {};
      void closeCurrentFile();
      void readFile(std::string const& name, art::FileBlock*& fb);
      
      //      bool hasMoreData() const { return !shutdownMsgReceived_; }
      
      bool readNext(art::RunPrincipal* const& inR,
		    art::SubRunPrincipal* const& inSR,
		    art::RunPrincipal*& outR,
		    art::SubRunPrincipal*& outSR,
		    art::EventPrincipal*& outE);
      
    private:
      bool   createDigitsFromArtdaqEvent();
      
      bool   fIsFirst;
      bool   fMoreData;
      int64_t fTimeOffset;
      
      art::SourceHelper const& fSourceHelper;
      std::string   fDAQDataLabel;
      std::string   fCurrentFilename;

      std::unordered_map<artdaq::Fragment::fragment_id_t,size_t> fFragCounter;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::WaveForm> > > fWaveForms;
      
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::TRB3RawDigit> > > fTRB3RawDigits;

      TTree* fTRB3Tree;
      std::vector<uint32_t> fTRB3_HeaderWord;
      std::vector<uint32_t> fTRB3_Measurement;
      std::vector<uint32_t> fTRB3_Channel;
      std::vector<uint32_t> fTRB3_FineTime;
      std::vector<uint32_t> fTRB3_EpochTime;
      std::vector<uint32_t> fTRB3_CoarseTime;
      
      std::unordered_map<int, TH1I*> fC1720_WaveForm;
      
    };
  }
}

#endif
