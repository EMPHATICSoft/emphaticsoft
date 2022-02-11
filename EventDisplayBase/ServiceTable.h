///
/// \file   EventDisplayBase/ServiceTable.h
/// \brief  Interface to services and their configurations
/// \author messier@indiana.edu
///
#ifndef EVDB_SERVICETABLE_H
#define EVDB_SERVICETABLE_H
#include "cetlib/exempt_ptr.h"
#include "fhiclcpp/ParameterSet.h"
#include "EventDisplayBase/Reconfigurable.h"

#include <vector>
#include <map>
#include <string>
#include <tuple>

namespace evdb {

  static constexpr int kDRAWING_SERVICE    = 1;
  static constexpr int kEXPERIMENT_SERVICE = 2;

  ///
  /// \brief Information about a service required by the event display
  ///
  struct ServiceTableEntry {
    std::string fName;
    fhicl::ParameterSet fCurrentParamSet;
    std::string fParamSet;
    int fCategory;
    cet::exempt_ptr<Reconfigurable> fService;
  };

  ///
  /// \brief Collection of Services used in the event display
  ///
  class ServiceTable {
  public:
    static ServiceTable& Instance();

    void RegisterService(fhicl::ParameterSet const& ps, cet::exempt_ptr<Reconfigurable> s);
    static bool IsDrawingService(std::string const& s);

    void Edit(unsigned int i);
    void ApplyEdits();

    static void OverrideCategory(std::string const& s, int cat);
    fhicl::ParameterSet const& GetParameterSet(unsigned int i) const;

  public:
    std::vector<ServiceTableEntry> fServices;

  private:
    static std::map<std::string, int> fgCategoryOverrides;
    ServiceTable();
  };
}

#endif
////////////////////////////////////////////////////////////////////////
