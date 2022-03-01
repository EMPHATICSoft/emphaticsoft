////////////////////////////////////////////////////////////////////////
/// \file  Printable.h
/// \brief Base class for printable objects
///
/// \version $Id: Printable.h,v 1.2 2011-01-23 16:08:50 p-nusoftart Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_PRINTABLE_H
#define EVDB_PRINTABLE_H
#include <string>
#include <map>

namespace evdb {
  /// Base class for printable objects
  class Printable {
  public:
    Printable();
    virtual ~Printable();

    // Print the object to a file
    virtual void Print(const char* filename) = 0;
  
    // Give a medium length description of what the view holds to be
    // printed. Examples: "Full view of experiment", "3D View", ...
    virtual const char* Description() const { return "??"; }

    // Give a short tag which can be included in filename to ID object printed
    // Keep to ~4 characters
    virtual const char* PrintTag() const = 0;
    
  public:
    static void AddToListOfPrintables(const char* name, evdb::Printable* p);
    static void RemoveFromListOfPrintables(evdb::Printable* p);
    static std::map<std::string,evdb::Printable*>& GetPrintables();

  };
}

#endif // EVDB_PRINTABLE_H
////////////////////////////////////////////////////////////////////////

