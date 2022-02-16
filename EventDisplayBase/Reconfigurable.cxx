#include "EventDisplayBase/Reconfigurable.h"
#include "EventDisplayBase/ServiceTable.h"

#include <iostream>

evdb::Reconfigurable::Reconfigurable(fhicl::ParameterSet const& ps)
{
  ServiceTable::Instance().RegisterService(ps, cet::exempt_ptr<Reconfigurable>{this});
}
