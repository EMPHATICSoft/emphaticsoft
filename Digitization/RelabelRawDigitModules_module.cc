///////////////////////////////////////////////////////////////////////////
// \file    RelabelRawDigitModules_module.cc
// \brief   Changes the module label for simulated digits.
// Grabs SSD and ARICH simulated {SSD,TRB3}RawDigits from digitization output to
// change their module label to the same as is used for data. 
// \author  lackey32@fnal.gov
///////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RawData/SSDRawDigit.h"
#include "RawData/TRB3RawDigit.h"

using namespace emph;

namespace emph {
  class RelabelRawDigitModules : public art::EDProducer {
  public:
    explicit RelabelRawDigitModules(fhicl::ParameterSet const &pset);
    virtual ~RelabelRawDigitModules();

    void produce(art::Event& evt);

  protected:
    std::string fRawDigitLabelARICH;
    std::string fRawDigitLabelSSD;
    std::string fInstanceLabelARICH;
    std::string fInstanceLabelSSD;
  };

  //--------------------------------------------------
  RelabelRawDigitModules::RelabelRawDigitModules(fhicl::ParameterSet const &pset) :
    EDProducer(pset),
    fRawDigitLabelARICH   (pset.get< std::string >("RawDigitLabelARICH")),
    fRawDigitLabelSSD     (pset.get< std::string >("RawDigitLabelSSD")),
    fInstanceLabelARICH   (pset.get< std::string >("InstanceLabelARICH")),
    fInstanceLabelSSD     (pset.get< std::string >("InstanceLabelSSD"))
  {
    produces<std::vector<rawdata::TRB3RawDigit> >(fInstanceLabelARICH);
    produces<std::vector<rawdata::SSDRawDigit>  >(fInstanceLabelSSD);
  }

  RelabelRawDigitModules::~RelabelRawDigitModules()
  {
  }

  void RelabelRawDigitModules::produce(art::Event& evt)
  {
    std::unique_ptr< std::vector<rawdata::TRB3RawDigit> > arichDigs (new std::vector<rawdata::TRB3RawDigit>);
    std::unique_ptr< std::vector<rawdata::SSDRawDigit> >   ssdDigs (new std::vector<rawdata::SSDRawDigit>);

    art::Handle< std::vector<rawdata::TRB3RawDigit> > arichHandle;
    evt.getByLabel(fRawDigitLabelARICH, arichHandle);
    art::Handle< std::vector<rawdata::SSDRawDigit> > ssdHandle;
    evt.getByLabel(fRawDigitLabelSSD, ssdHandle);

    // auto ssdHandle = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fRawDigitLabelSSD);
    for (unsigned int i = 0; i<ssdHandle->size(); ++i) {
      rawdata::SSDRawDigit digit ( (*ssdHandle)[i]);
      ssdDigs->push_back(digit);
    }
    //if (!ssdHandle.failedToGet()) ssdDigs = *ssdHandle;

    for (unsigned int i = 0; i<arichHandle->size() ; ++i) {
      rawdata::TRB3RawDigit digit ( (*arichHandle)[i]);
      arichDigs->push_back(digit);
    }

    // for (size_t i = 0; i< ssdHandle->size(); ++i) {
    //   art::Ptr<emph::rawdata::SSDRawDigit> digit(ssdHandle,i);
    //   ssdDigs->push_back(digit);
    // }

    evt.put(std::move(arichDigs), fInstanceLabelARICH);
    evt.put(std::move(ssdDigs), fInstanceLabelSSD);    

  } // produce

} // emph namespace

DEFINE_ART_MODULE(RelabelRawDigitModules)
