///
/// \file  EventDisplay.h
/// \brief The interactive event display
///
/// \version $Id: EventDisplay.h,v 1.15 2012-03-05 14:37:34 messier Exp $
/// \author  messier@indiana.edu
///
#ifndef EVDB_EVENTDISPLAY_H
#define EVDB_EVENTDISPLAY_H

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Persistency/Provenance/ScheduleContext.h"

namespace fhicl { class ParameterSet; }
namespace art   { class ActivityRegistry; }
namespace art   { class Worker; }
namespace art   { class InputSource; }
namespace art   { class EventID; }
namespace art   { class Event; }

namespace evdb
{
  ///
  /// \brief ART event display service
  ///
  class EventDisplay
  {
  public:

    EventDisplay(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    void reconfigure(fhicl::ParameterSet const& pset);

  private:

    void postBeginJob();
    void postBeginJobWorkers(art::InputSource* inputs,
                             std::vector<art::Worker*> const& workers);
    void preProcessEvent(art::Event const&, art::ScheduleContext);
    void postProcessEvent(art::Event const&, art::ScheduleContext);

  private:
    art::InputSource* fInputSource; ///< Input source of events

  public:
    unsigned int fAutoAdvanceInterval; ///< Wait time in milliseconds
    int          fAutoPrintCount;      ///< Number of events printed so far
    int          fAutoPrintMax;        ///< How many events to print (zero = disable printing).
    std::string  fAutoPrintPattern;    ///< Pattern for printing output filenames.  Must contain "%s" and "%d", in that order.
    bool         fEchoPrint;           ///< Copy what you see in X to a .gif for each event
    std::string  fEchoPrintFile;       ///< The file to dump that .gif to.  Only one file, if you want a different file for each event, use AutoPrint instead.
    std::string  fEchoPrintTempFile;   ///< a temporary file to enable atomic writes
  };
}

DECLARE_ART_SERVICE(evdb::EventDisplay, LEGACY)
#endif // EVDB_EVENTDISPLAY_H

////////////////////////////////////////////////////////////////////////
