#include "emphatic-artdaq/Overlays/FragmentType.hh"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <map>

namespace {
  static std::map<emphaticdaq::detail::FragmentType,std::string> const
  names{
    {emphaticdaq::detail::FragmentType::MISSED, "MISSED"},

     //Common
    {emphaticdaq::detail::FragmentType::CAENV1730, "CAENV1730"},
    {emphaticdaq::detail::FragmentType::SpectratimeEvent, "SPECTRATIMEVENT"},
    {emphaticdaq::detail::FragmentType::BERNCRT, "BERNCRT"},
    {emphaticdaq::detail::FragmentType::BERNCRTV2, "BERNCRTV2"},
    {emphaticdaq::detail::FragmentType::BERNCRTZMQ,  "BERNCRTZMQ"},
    {emphaticdaq::detail::FragmentType::WhiteRabbit, "WhiteRabbit"},

    //ICARUS
    {emphaticdaq::detail::FragmentType::PHYSCRATEDATA,  "PHYSCRATEDATA"},
    {emphaticdaq::detail::FragmentType::PHYSCRATESTAT, "PHYSCRATESTAT"},
    {emphaticdaq::detail::FragmentType::ICARUSTriggerUDP,  "ICARUSTriggerUDP"},
    {emphaticdaq::detail::FragmentType::ICARUSPMTGate, "ICARUSPMTGate"},

    //SBND
    {emphaticdaq::detail::FragmentType::NevisTPC, "NEVISTPC"},
    {emphaticdaq::detail::FragmentType::PTB,      "PTB"},
    {emphaticdaq::detail::FragmentType::DAPHNE,   "DAPHNE"},

      //Simulators
    {emphaticdaq::detail::FragmentType::DummyGenerator, "DUMMYGENERATOR"},

    {emphaticdaq::detail::FragmentType::INVALID,  "UNKNOWN"}
  };
}

emphaticdaq::FragmentType
emphaticdaq::toFragmentType(std::string t_string)
{
  std::transform(t_string.begin(),
                 t_string.end(),
                 t_string.begin(),
                 toupper);
  for(auto it = names.begin(); it != names.end(); ++it)
    if(t_string == it->second)
      return static_cast<FragmentType>(it->first);
  return FragmentType::INVALID;
}

std::string
emphaticdaq::fragmentTypeToString(FragmentType val)
{
  if (val < FragmentType::INVALID) {
    return names.at(val);
  }
  else {
    return "INVALID/UNKNOWN"; //AA: why is it INVALID/UNKNOWN rather than UNKNOWN, as defined in the list above?
  }
}

std::map< artdaq::Fragment::type_t, std::string > emphaticdaq::makeFragmentTypeMap()
{
      auto output = artdaq::Fragment::MakeSystemTypeMap();
      for (auto name : names)
      {
	TLOG(TLVL_DEBUG) << "Setting map: " << name.first << " --> " << name.second;
	output[name.first] = name.second;
      }

      for( auto name : output)
      {
	TLOG(TLVL_DEBUG) << "Verifying map: " << name.first << " --> " << name.second;
      }

      return output;
}
