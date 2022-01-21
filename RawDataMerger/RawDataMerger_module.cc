////////////////////////////////////////////////////////////////////////
// Class:       RawDataMerger
// Plugin Type: producer
// File:        RawDataMerger_module.cc
//
// Code to read SSD raw data files is supplied by Lorenzo
// Module status: Not working! Testing components while DAQ file format is finalized
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RawData/SSDRawDigit.h"

#include <string>
#include <fstream>
#include <memory>

namespace emphatic {
    class RawDataMerger;
}


class emphatic::RawDataMerger : public art::EDProducer {
    public:
        explicit RawDataMerger(fhicl::ParameterSet const& p);

        // Plugins should not be copied or assigned.
        RawDataMerger(RawDataMerger const&) = delete;
        RawDataMerger(RawDataMerger&&) = delete;
        RawDataMerger& operator=(RawDataMerger const&) = delete;
        RawDataMerger& operator=(RawDataMerger&&) = delete;

        void produce(art::Event& e) override;
        void beginJob() override;
        void endJob() override;

    private:
        std::ifstream ssd_file;
        uint32_t ssd_events;
};


emphatic::RawDataMerger::RawDataMerger(fhicl::ParameterSet const& p)
    : EDProducer{p}
{
    // TODO not final names for fcl configuration
    std::string ssd_filename = p.get<std::string>("ssd_filename");
    std::cout << ssd_filename << "\n";
    ssd_file = std::ifstream(ssd_filename.c_str());
    if (!ssd_file.is_open()) {
        printf("Can't open file %s\n", ssd_filename.c_str());
        // TODO more art-like way of handling fatal error?
        exit(1);
    }
}


void emphatic::RawDataMerger::beginJob() {
}


void emphatic::RawDataMerger::endJob() {
    ssd_file.close();
}


void emphatic::RawDataMerger::produce(art::Event& e) {
    // TODO process events from artdaq
    // auto const& digit = evt.getProduct<std::vector<rawdata::RawDigit>>(fInputModuleLabel);

    // make the collection of ints we want to store
    int run = e.run();
    std::cout << run << "\n";
    // TODO check run number, here or in begin job. If event header is needed,
    // probably have to do it here

    // unpack the event into timestamp and hit data
    auto tmp_ssd_hits = rawdata::readSSDHitsFromFileStream(ssd_file);
    uint64_t bco = tmp_ssd_hits.first;
    std::vector<rawdata::SSDRawDigit> hits = tmp_ssd_hits.second;
    std::cout << bco << "\t" << hits.size() << "\n";
}


DEFINE_ART_MODULE(emphatic::RawDataMerger)
