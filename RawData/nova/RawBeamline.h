////////////////////////////////////////////////////////////////////////////
/// \file    RawBeamline.h
/// \brief   Raw data definitions for beamline data used in NOvA
///          test beam experiment.
/// \author  Mike Wallbank (University of Cincinnati) <wallbank@fnal.gov>
/// \date    October 2018
////////////////////////////////////////////////////////////////////////////

#ifndef RAWBEAMLINE_H
#define RAWBEAMLINE_H

// framework
#include "cetlib_except/exception.h"

// nova
#include "BeamlineUtils/BeamlineGeometry.h"

// stl
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <bitset>

// root

// -----------------------------------------------------------------------
namespace rawdata {

  // -----------------------------------------------------------------------
  // RawBeamlineConfig
  // -----------------------------------------------------------------------
  class RawBeamlineConfig {

  public:

    RawBeamlineConfig();
    virtual ~RawBeamlineConfig() {};

    void SetBeamlineRun(unsigned int run);
    void SetBField(float b_field);

    unsigned int BeamlineRun() const;
    float BField() const;

  private:

    unsigned int fBeamlineRun;
    float fBField;

  };

  // -----------------------------------------------------------------------
  // RawBeamlineTrigger
  // -----------------------------------------------------------------------
  class RawBeamlineTrigger {

  public:

    RawBeamlineTrigger();
    RawBeamlineTrigger(beamlinegeo::DetectorID detector, unsigned int number);
    virtual ~RawBeamlineTrigger() {};

    void SetDetectorID(beamlinegeo::DetectorID detector);
    void SetTimestamp(uint64_t timestamp);
    void SetPattern(uint32_t pattern);
    void SetTDUTime(uint64_t tduTime);
    void SetTriggerCondition(std::string trig_condition);

    unsigned int TriggerNumber() const;
    beamlinegeo::DetectorID DetectorID() const;
    uint64_t Timestamp() const;
    uint32_t Pattern() const;
    uint64_t TDUTime() const;
    std::string TriggerCondition() const;

  private:

    beamlinegeo::DetectorID fDetectorID;
    unsigned int fNumber;

    uint64_t fTimestamp;
    uint32_t fPattern;
    uint64_t fTDUTime;
    std::string fTriggerCondition;

  };

  // -----------------------------------------------------------------------
  // RawBeamlineDigit
  // -----------------------------------------------------------------------
  class RawBeamlineDigit {

  public:

    RawBeamlineDigit();
    RawBeamlineDigit(beamlinegeo::ChannelID channel);
    virtual ~RawBeamlineDigit() {};

    beamlinegeo::ChannelID ChannelID() const;
    uint32_t Timestamp() const;
    unsigned int NADC() const;
    uint16_t ADC(unsigned int sample) const;
    std::vector<uint16_t> ADCs() const;

    void SetChannelID(beamlinegeo::ChannelID channel);
    void SetTimestamp(uint32_t timestamp);
    void SetWaveform(std::vector<uint16_t> waveform);

  private:

    beamlinegeo::ChannelID fChannelID;
    uint32_t fTimestamp;
    std::vector<uint16_t> fADCs;

  };

  // -----------------------------------------------------------------------
  // RawBeamlineWC
  // -----------------------------------------------------------------------
  class RawBeamlineWC {

  public:

    struct WCPulse {
      WCPulse() {}
      WCPulse(int channel, int time) { Channel = channel; Time = time; }
      int Channel;
      int Time;
    };

    RawBeamlineWC();
    RawBeamlineWC(beamlinegeo::DetectorID detector);
    virtual ~RawBeamlineWC() {}

    beamlinegeo::DetectorID DetectorID() const;
    uint64_t Timestamp() const;
    std::vector<WCPulse> const& XPulses() const { return fXPulses; }
    std::vector<WCPulse> const& YPulses() const { return fYPulses; }

    void AddXPulse(WCPulse pulse);
    void AddYPulse(WCPulse pulse);
    void SetDetectorID(beamlinegeo::DetectorID detector);
    void SetTimestamp(uint64_t timestamp);

  private:

    beamlinegeo::DetectorID fDetectorID;
    uint64_t fTimestamp;

    std::vector<WCPulse> fXPulses;
    std::vector<WCPulse> fYPulses;

  };

} // namespace

#endif
