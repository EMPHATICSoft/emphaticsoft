////////////////////////////////////////////////////////////////////////
/// \file  UserActionFactory.cxx
/// \brief factory for generating G4Base/UserAction class objects
///
/// \version 
/// \author  Robert Hatcher <rhatcher \at fnal.gov>
///          Fermi National Accelerator Laboratory
///
/// update  2012-08-17 initial version
////////////////////////////////////////////////////////////////////////

#include "G4Base/UserActionFactory.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

namespace g4b {

// Define static variable which holds the one-and-only instance
UserActionFactory* UserActionFactory::fgTheInstance;

UserActionFactory::UserActionFactory() 
{
  fgTheInstance = this;   // record created self in static pointer
}

UserActionFactory::~UserActionFactory()
{
  fgTheInstance = 0;
}

UserActionFactory& UserActionFactory::Instance()
{
  // Cleaner dtor calls UserActionFactory dtor at job end
  static Cleaner cleaner;

  if ( ! fgTheInstance ) {
    // need to create one
    cleaner.UseMe();   // dummy call to quiet compiler warnings
    fgTheInstance = new UserActionFactory();
  }
  
  return *fgTheInstance;
}

UserAction* 
UserActionFactory::GetUserAction(const std::string& name)
{
  UserAction* p = 0;
  
  // we don't want map creating an entry if it doesn't exist
  // so use map::find() not map::operator[]
  std::map<std::string, UserActionCtorFuncPtr_t>::iterator itr
    = fFunctionMap.find(name);
  if ( fFunctionMap.end() != itr ) { 
    // found an appropriate entry in the list
    UserActionCtorFuncPtr_t foo = itr->second;  // this is the function
    p = (*foo)();  // use function to create the UserAction
    p->SetName(name); // let object know its name
  }
  if ( ! p ) {
    mf::LogWarning("UserAction") << "### UserActionFactory WARNING: "
                                 << "UserAction " << name << " is not known";
  }
  return p;
}
  
bool UserActionFactory::IsKnownUserAction(const std::string& name)
{
  //  check if we know the name
  bool res = false;
  std::map<std::string, UserActionCtorFuncPtr_t>::iterator itr
    = fFunctionMap.find(name);
  if ( fFunctionMap.end() != itr ) res = true;
  return res;
}

const std::vector<std::string>& 
UserActionFactory::AvailableUserActions() const
{
  // list of names might be out of date due to new registrations
  // rescan the std::map on each call (which won't be frequent)
  listnames.clear();

  // scan map for registered names
  std::map<std::string, UserActionCtorFuncPtr_t>::const_iterator itr;
  for ( itr = fFunctionMap.begin(); itr != fFunctionMap.end(); ++itr )
    listnames.push_back(itr->first);

  return listnames;
}

bool UserActionFactory::RegisterCreator(std::string name, 
                                          UserActionCtorFuncPtr_t foo,
                                          bool* boolptr)
{
  // record new functions for creating processes
  fFunctionMap[name] = foo;
  fBoolPtrMap[name]  = boolptr;
  return true;
}

} // namespace g4b
