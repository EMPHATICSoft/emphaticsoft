////////////////////////////////////////////////////////////////////////////
/// \file    RawBeamline.cxx
/// \brief   Raw data definitions for beamline data used in NOvA
///          test beam experiment.
/// \author  Mike Wallbank (University of Cincinnati) <wallbank@fnal.gov>
/// \date    October 2018
////////////////////////////////////////////////////////////////////////////

#include "RawData/RawBeamline.h"

// -----------------------------------------------------------------------
// RawBeamlineTrigger
// -----------------------------------------------------------------------
rawdata::RawBeamlineConfig::RawBeamlineConfig() {
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineConfig::SetBeamlineRun(unsigned int run) {
  fBeamlineRun = run;
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineConfig::SetBField(float b_field) {
  fBField = b_field;
  return;
}

// -----------------------------------------------------------------------
unsigned int rawdata::RawBeamlineConfig::BeamlineRun() const {
  return fBeamlineRun;
}

// -----------------------------------------------------------------------
float rawdata::RawBeamlineConfig::BField() const {
  return fBField;
}

// -----------------------------------------------------------------------
// RawBeamlineTrigger
// -----------------------------------------------------------------------
rawdata::RawBeamlineTrigger::RawBeamlineTrigger() {
}

// -----------------------------------------------------------------------
rawdata::RawBeamlineTrigger::RawBeamlineTrigger(beamlinegeo::DetectorID detector,
						unsigned int number) {
  fDetectorID = detector;
  fNumber = number;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineTrigger::SetDetectorID(beamlinegeo::DetectorID detector) {
  fDetectorID = detector;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineTrigger::SetTimestamp(uint64_t timestamp) {
  fTimestamp = timestamp;
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineTrigger::SetPattern(uint32_t pattern) {
  fPattern = pattern;
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineTrigger::SetTDUTime(uint64_t tduTime) {
  fTDUTime = tduTime;
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineTrigger::SetTriggerCondition(std::string trig_condition) {
  fTriggerCondition = trig_condition;
  return;
}

// -----------------------------------------------------------------------
unsigned int rawdata::RawBeamlineTrigger::TriggerNumber() const {
  return fNumber;
}

// -----------------------------------------------------------------------
beamlinegeo::DetectorID rawdata::RawBeamlineTrigger::DetectorID() const {
  return fDetectorID;
}

// -----------------------------------------------------------------------
uint64_t rawdata::RawBeamlineTrigger::Timestamp() const {
  return fTimestamp;
}

// -----------------------------------------------------------------------
uint32_t rawdata::RawBeamlineTrigger::Pattern() const {
  return fPattern;
}

// -----------------------------------------------------------------------
uint64_t rawdata::RawBeamlineTrigger::TDUTime() const {
  return fTDUTime;
}

// -----------------------------------------------------------------------
std::string rawdata::RawBeamlineTrigger::TriggerCondition() const {
  return fTriggerCondition;
}

// -----------------------------------------------------------------------
// RawBeamlineDigit
// -----------------------------------------------------------------------
rawdata::RawBeamlineDigit::RawBeamlineDigit() {
}

// -----------------------------------------------------------------------
rawdata::RawBeamlineDigit::RawBeamlineDigit(beamlinegeo::ChannelID channel) {
  fChannelID = channel;
}

// -----------------------------------------------------------------------
beamlinegeo::ChannelID rawdata::RawBeamlineDigit::ChannelID() const {
  return fChannelID;
}

// -----------------------------------------------------------------------
uint32_t rawdata::RawBeamlineDigit::Timestamp() const {
  return fTimestamp;
}

// -----------------------------------------------------------------------
unsigned int rawdata::RawBeamlineDigit::NADC() const {
  return fADCs.size();
}

// -----------------------------------------------------------------------
uint16_t rawdata::RawBeamlineDigit::ADC(unsigned int sample) const {
  if (sample >= this->NADC()) {
    std::cout << "Sample index " << sample << " out of range (ADCs.size() == " << this->NADC() << ").  "
	      << "Aborting..." << std::endl;
    abort();
  }
  return fADCs.at(sample);
}

// -----------------------------------------------------------------------
std::vector<uint16_t> rawdata::RawBeamlineDigit::ADCs() const {
  return fADCs;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineDigit::SetChannelID(beamlinegeo::ChannelID channel) {
  fChannelID = channel;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineDigit::SetTimestamp(uint32_t timestamp) {
  fTimestamp = timestamp;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineDigit::SetWaveform(std::vector<uint16_t> waveform) {
  fADCs = waveform;
}

// -----------------------------------------------------------------------
// RawBeamlineWC
// -----------------------------------------------------------------------
rawdata::RawBeamlineWC::RawBeamlineWC() {
}

// -----------------------------------------------------------------------
rawdata::RawBeamlineWC::RawBeamlineWC(beamlinegeo::DetectorID detector) {
  fDetectorID = detector;
}

// -----------------------------------------------------------------------
beamlinegeo::DetectorID rawdata::RawBeamlineWC::DetectorID() const {
  return fDetectorID;
}

// -----------------------------------------------------------------------
uint64_t rawdata::RawBeamlineWC::Timestamp() const {
  return fTimestamp;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineWC::AddXPulse(WCPulse pulse) {
  fXPulses.push_back(pulse);
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineWC::AddYPulse(WCPulse pulse) {
  fYPulses.push_back(pulse);
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineWC::SetDetectorID(beamlinegeo::DetectorID detector) {
  fDetectorID = detector;
  return;
}

// -----------------------------------------------------------------------
void rawdata::RawBeamlineWC::SetTimestamp(uint64_t timestamp) {
  fTimestamp = timestamp;
  return;
}
