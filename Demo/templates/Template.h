////////////////////////////////////////////////////////////////////////
// \file    MODULENAME.h
// \brief   TODO
// \version $Id: Template.h,v 1.1 2012-07-20 00:45:39 bckhouse Exp $
// \author  AUTHOR
////////////////////////////////////////////////////////////////////////
#ifndef INCLUDEGUARD
#define INCLUDEGUARD

// ART includes
#include "art/Framework/Core/EDProducer.h"

namespace NAMESPACE
{
  class MODULENAME : public art::EDProducer
  {
  public:
    explicit MODULENAME(const fhicl::ParameterSet& pset);
    ~MODULENAME();

    void produce(art::Event& evt);

    void beginJob();

  protected:
  };
}

#endif // INCLUDEGUARD
////////////////////////////////////////////////////////////////////////
