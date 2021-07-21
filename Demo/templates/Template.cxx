////////////////////////////////////////////////////////////////////////
// \file    MODULENAME.cxx
// \brief   TODO
// \version $Id: Template.cxx,v 1.1 2012-07-20 00:45:39 bckhouse Exp $
// \author  AUTHOR
////////////////////////////////////////////////////////////////////////

#include "MODULENAME.h"

// Framework includes
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace NAMESPACE
{
  //.......................................................................
  MODULENAME::MODULENAME(const fhicl::ParameterSet& pset)
  {
    reconfigure(pset);

    //    produces< std::vector<int> >();
  }

  //......................................................................
  MODULENAME::~MODULENAME()
  {
  }

  //......................................................................
  void MODULENAME::reconfigure(const fhicl::ParameterSet& pset)
  {
  }

  //......................................................................
  void MODULENAME::beginJob()
  {
  }

  //......................................................................
  void MODULENAME::produce(art::Event& evt)
  {
  }

} // end namespace NAMESPACE
////////////////////////////////////////////////////////////////////////

