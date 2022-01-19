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

#include <string>
#include <fstream>
#include <memory>

namespace emphatic {
    class RawDataMerger;
    struct SSDHit;
}

struct emphatic::SSDHit {
    int module;
    int chip;
    int set;
    int strip;
    int bco;
    int adc;
    int station;
    int row;
    int trigger;
    void clear() {
        module = -1;
        chip = -1;
        set = -1;
        strip = -1;
        bco = -1;
        adc = -1;
        station = -1;
        row = -1;
        trigger = -1;
    }
};


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
        std::string ssd_filename;
        uint32_t ssd_events;
        unsigned int getSensorRow(int, int, int);
};


emphatic::RawDataMerger::RawDataMerger(fhicl::ParameterSet const& p)
    : EDProducer{p}
{
    // TODO not final names for fcl configuration
    ssd_filename = p.get<std::string>("DigitModuleLabel", "daq");
}


void emphatic::RawDataMerger::beginJob() {
    const uint64_t kOnes = -1;
    const int kDataSize = sizeof(unsigned long long);
    const uint32_t kMaxEvents = 500;

    std::ifstream ssd_file(ssd_filename.c_str(), std::ios::binary);
    if (!ssd_file.is_open()) {
        printf("Can't open file %s\n", ssd_filename.c_str());
        exit(1);
    }

    std::vector<emphatic::SSDHit> ssd_hits;
    ssd_hits.reserve(kMaxEvents);

    uint64_t rawdata_buffer;
    emphatic::SSDHit hit;

    while (!ssd_file.eof()) {
        ssd_file.read((char*)(&rawdata_buffer), kDataSize);
        if (rawdata_buffer == kOnes) {
            // end of event
            ssd_events++;

            // bco clock time is stored next in the stream, so we need an additional read
            ssd_file.read((char*)(&rawdata_buffer), kDataSize);
            hit.bco = (rawdata_buffer >> 32) | (rawdata_buffer & 0xffffffff);
            ssd_hits.push_back(hit);
            hit.clear();
        }
        else {
            // event data
            uint64_t rawdata_tmp = 0;
            uint64_t rawdata_ordered = 0;
            for (int byte = 0; byte < kDataSize; byte++) {
                rawdata_tmp = ((rawdata_buffer >> ((kDataSize - byte - 1) * 8)) & 0xff) << (byte * 8);
                rawdata_ordered += rawdata_tmp;
            }
            rawdata_buffer = rawdata_ordered;
            hit.trigger = (rawdata_buffer >> 32) & 0xfffff;
            hit.module = (rawdata_buffer >> 27) & 0x07;
            hit.chip = (rawdata_buffer >> 24) & 0x07;
            hit.set = (rawdata_buffer >> 12) & 0x1f;
            hit.strip = (rawdata_buffer >> 17) & 0x0f;
            hit.bco = (rawdata_buffer >> 4) & 0xff;
            hit.adc = (rawdata_buffer >> 1) & 0x7;
            hit.station = (rawdata_buffer >> 56) & 0xf;
            hit.row = getSensorRow(hit.chip, hit.set, hit.strip);
        }
    }
    ssd_file.close();
}


void emphatic::RawDataMerger::endJob() {
}


void emphatic::RawDataMerger::produce(art::Event& e) {
    // TODO process events from artdaq
    std:: cout << e.id().event() << std::endl;
    // search ssd hits for closest-time hit
}

unsigned int emphatic::RawDataMerger::getSensorRow(int chip, int set, int strip) {
    // TODO bounds checking
    static const unsigned char set_number[] = {
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255,   0,   1,   4,   5,   3,   2,
        255, 255,  12,  13,   8,   9,  11,  10,
        255, 255,  15,  14,   7,   6, 255, 255
    };
    static const unsigned char strip_number[] = {
        255, 255, 255, 255,
        255,   0,   2,   1,
        255,   6,   4,   5,
        255,   7,   3, 255
    };

    return 128 * (chip - 1) + (set_number[set] * 8) + strip_number[strip];
}

DEFINE_ART_MODULE(emphatic::RawDataMerger)
