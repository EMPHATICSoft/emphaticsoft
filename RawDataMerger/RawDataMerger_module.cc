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

#include "RawData/CAENV1720Fragment.h"
#include "RawData/FragmentType.h"
#include "artdaq-core/Data/Fragment.hh"

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
    ssd_events = 0;
}


void emphatic::RawDataMerger::beginJob() {
}


void emphatic::RawDataMerger::endJob() {
    ssd_file.close();
}


void emphatic::RawDataMerger::produce(art::Event& evt) {
    // TODO process events from artdaq
    // auto const& digit = evt.getProduct<std::vector<rawdata::RawDigit>>(fInputModuleLabel);

    // make the collection of ints we want to store
    int run = evt.run();
    std::cout << run << "\n";
    // TODO check run number, here or in begin job. If event header is needed,
    // probably have to do it here

    // unpack the event into timestamp and hit data
    // there are usually empty triggers before the first SSD event so we 
    // need to read in to find the first actual hits
    std::vector<rawdata::SSDRawDigit> hits;
    uint64_t bco = 0;
    do {
        auto tmp_ssd_hits = rawdata::readSSDHitsFromFileStream(ssd_file);
        bco = tmp_ssd_hits.first;
        hits = tmp_ssd_hits.second;
        // std::cout << bco << "\t" << hits.size() << "\n";
        ssd_events++;
    } while (hits.size() == 0);
    std::cout << ssd_events << "\n";

    // compute the timestamp offset with the artdaq file
    // mostly copied from emphaticsoft/otsdaq-demo
    // modified to get the timestamp from the first 1720
    uint64_t artdaq_timestamp = 0;
    std::vector<art::Handle<artdaq::Fragments>>fragmentHandles = evt.getMany<artdaq::Fragments>();
    for (const auto& handle : fragmentHandles) {
        if (!handle.isValid() || handle->size() == 0) {
            continue;
        }

        if(handle->front().type() != ots::detail::FragmentType::CAENV1720) {
            continue;
        }
        TLOG(TLVL_INFO)<<"This is CAENV1720 fragment";

        bool found = false;
        for(const auto& frag : *handle) { //loop over fragments
                                          //--use this fragment as a reference to the same data
            emphaticdaq::CAENV1720Fragment bb(frag);
            auto const* md = bb.Metadata();
            // emphaticdaq::CAENV1720Event const* event_ptr = bb.Event();

            // emphaticdaq::CAENV1720EventHeader header = event_ptr->Header;

            //convert fragment access time to human-readable format
            struct timespec ts = frag.atime();
            char buff[100];
            strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
            std::cout << "\n\tFrom metadata, timeStampSec      "  << md->timeStampSec
                << "\n\tFrom metadata, timeStampNSec     "  << md->timeStampNSec << "\n";

            artdaq_timestamp = frag.timestamp();
            found = true;
            // break;
        }
        if (found) {
            break;
        }
    }

    int64_t timestamp_diff = bco - artdaq_timestamp;
    std::cout << bco << "\t" << artdaq_timestamp << "\t" << timestamp_diff << "\n";
}


DEFINE_ART_MODULE(emphatic::RawDataMerger)
