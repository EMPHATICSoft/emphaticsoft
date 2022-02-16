///
/// \file   EventDisplayBase/ServiceTable.h
/// \brief  Interface to services and their configurations
/// \author messier@indiana.edu
///
#include "EventDisplayBase/ServiceTable.h"

#include "fhiclcpp/intermediate_table.h"
//#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "EventDisplayBase/ParameterSetEdit.h"
#include "EventDisplayBase/ParameterSetEditDialog.h"

#include <iostream>

using namespace evdb;

std::map<std::string, int> ServiceTable::fgCategoryOverrides;

//......................................................................

bool ServiceTable::IsDrawingService(const std::string& s)
{
  if(fgCategoryOverrides.count(s))
    return fgCategoryOverrides[s] == kDRAWING_SERVICE;

  return (s.find("DrawingOptions")!=std::string::npos);
}

//......................................................................

void ServiceTable::RegisterService(fhicl::ParameterSet const& ps,
                                   cet::exempt_ptr<Reconfigurable> s)
{
  ServiceTableEntry entry;
  entry.fName     = ps.get<std::string>("service_type");
  entry.fCurrentParamSet = ps;
  entry.fParamSet = "";
  entry.fCategory = this->IsDrawingService(entry.fName) ? kDRAWING_SERVICE : kEXPERIMENT_SERVICE;
  entry.fService  = s;

  fServices.emplace_back(std::move(entry));
}

//......................................................................

ServiceTable& ServiceTable::Instance()
{
  static ServiceTable s;
  return s;
}

//......................................................................

void ServiceTable::Edit(unsigned int i)
{
  assert(i < fServices.size());
  new ParameterSetEditDialog(i);
}

//......................................................................

void ServiceTable::ApplyEdits()
{
  // Look to see if we have any new service configurations to apply
  for (auto& s : fServices) {
    if (s.fParamSet.empty()) continue;

    MF_LOG_DEBUG("ServiceTable") << "Applying edits for "
                              << s.fName
                              << "\n"
                              << s.fParamSet;

    try {
      fhicl::ParameterSet pset;
      //
      // Each of the next 2 lines may throw on error: should check.
      //
      //fhicl::make_ParameterSet(s.fParamSet, pset);
      pset = fhicl::ParameterSet::make(s.fParamSet);
      s.fParamSet.clear();
      s.fService->do_reconfigure(pset);
      s.fCurrentParamSet = pset;
    }
    catch (fhicl::exception const& e) {
      MF_LOG_ERROR("ServiceTable") << "Error parsing the new configuration:\n"
                                << e
                                << "\nRe-configuration has been ignored for service: "
                                << s.fName;
    }
  }
}

//......................................................................

void ServiceTable::OverrideCategory(std::string const& s, int const cat)
{
  fgCategoryOverrides[s] = cat;
}

//......................................................................

fhicl::ParameterSet const& ServiceTable::GetParameterSet(unsigned int id) const
{
  assert(id < fServices.size());
  return fServices[id].fCurrentParamSet;
}

//......................................................................

ServiceTable::ServiceTable() {}

////////////////////////////////////////////////////////////////////////
