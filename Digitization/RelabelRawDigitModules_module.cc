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
    if (arichHandle.isValid()) {
      evt.getByLabel(fRawDigitLabelARICH, arichHandle);
      for (unsigned int i = 0; i<arichHandle->size() ; ++i) {
	rawdata::TRB3RawDigit digit ( (*arichHandle)[i]);
	arichDigs->push_back(digit);
      }
    } // valid arich handle

    art::Handle< std::vector<rawdata::SSDRawDigit> > ssdHandle;
    if (ssdHandle.isValid()) {
      evt.getByLabel(fRawDigitLabelSSD, ssdHandle);

      for (unsigned int i = 0; i<ssdHandle->size(); ++i) {
	rawdata::SSDRawDigit digit ( (*ssdHandle)[i]);
	ssdDigs->push_back(digit);
      }
    } // valid ssd handle

    evt.put(std::move(arichDigs), fInstanceLabelARICH);
    evt.put(std::move(ssdDigs), fInstanceLabelSSD);    

  } // produce

} // emph namespace

DEFINE_ART_MODULE(RelabelRawDigitModules)
