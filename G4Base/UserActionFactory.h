////////////////////////////////////////////////////////////////////////
/// \file  UserActionFactory.h
/// \brief A class for generating concrete UserAction derived classes
///        based on the factory pattern.  This code supplies a CPP
///        macro which allows the classes to self-register and thus
///        no modification of this class is needed in order to expand
///        the list of classes it knows about.
///
///        Implemented as a singleton holding a map between names and
///        pointers-to-functions (that call a class default constructor).
///        The functions pointers must return UserAction*.
///
/// \version 
/// \author  Robert Hatcher <rhatcher \at fnal.gov>
///          Fermi National Accelerator Laboratory
///
////////////////////////////////////////////////////////////////////////
#ifndef USERACTIONFACTORY_H
#define USERACTIONFACTORY_H

#include <string>
#include <vector>
#include <map>

#include "G4Base/UserAction.h"

namespace g4b {

// define a type for the pointer to a function that returns a 
//    g4b::UserAction* 
// i.e. calls the (typically default) ctor for the class.
typedef g4b::UserAction* (*UserActionCtorFuncPtr_t)();

class UserActionFactory
{
public:
  static UserActionFactory& Instance();
  // no public ctor for singleton, all user access is through Instance()

  g4b::UserAction* GetUserAction(const std::string&);
  // instantiate a PhysProc by name

  bool IsKnownUserAction(const std::string&);
  // check if the name is in the list of names

  const std::vector<std::string>& AvailableUserActions() const;
  // return a list of available names

  bool RegisterCreator(std::string name, 
                       UserActionCtorFuncPtr_t ctorptr, bool* ptr);
  // register a new UserAction type by passing pointer to creator function

private:
  static UserActionFactory* fgTheInstance;
  // the one and only instance

  std::map<std::string, UserActionCtorFuncPtr_t> fFunctionMap;
  // mapping between known class names and a registered ctor function

  std::map<std::string, bool*> fBoolPtrMap;

  mutable std::vector<std::string> listnames;
  // copy of list of names, used solely due to AvailableUserActions() 
  // method returning a const reference rather than a vector object.
  // mutable because AvailableUserActions() is const, but list might 
  // need recreation if new entries have been registered.

private:
  UserActionFactory();
  // private ctor, users access class via Instance()

  virtual ~UserActionFactory();

  UserActionFactory(const UserActionFactory&);
  // method private and not implement, declared to prevent copying

  void operator=(const UserActionFactory&);
  // method private and not implement, declared to prevent assignment

  // sub-class Cleaner struct is used to clean up singleton at the end of job.
  struct Cleaner {
     void UseMe() { }  // Dummy method to quiet compiler
    ~Cleaner() {
       if (UserActionFactory::fgTheInstance != 0) {
         delete UserActionFactory::fgTheInstance;
         UserActionFactory::fgTheInstance = 0;
  } } };
  friend struct Cleaner; 

};

} // namespace g4b

// Define macro to create a function to call the class' ctor
// and then registers this function with the factory instance for later use
// Users should have in their  myPhyList.cc two lines that look like:
//     #include "UserActionFactory.h"
//     USERACTIONREG(MyUserActionClass)  // no semicolon
// where "MyMixerClass" is the name of the class (assuming no special namespace)
// If the class is defined in a namespace (or two) use:
//     #include "UserActionFactory.h"
//     USERACTIONREG3(myspace,myAltAction,myspace::myAltAction) // no semicolon
//     USERACTIONREG4(alt,nspace,YAAction,alt::nspace::YAAction) // no semicolon
// and either can then be retrieved from the factory using:
//     UserActionFactory& factory =
//         UserActionFactory::Instance();
//     g4::UserAction* p = 0;
//     p = factory.GetUserAction("MyUserActionClass");
//     p = factory.GetUserAction("myspace::myAltAction");
//     p = factory.GetUserAction("alt::nspace::YAAction");
//
#define USERACTIONREG( _name ) \
  g4b::UserAction* _name ## _ctor_function () { return new _name; } \
  static bool _name ## _creator_registered =                            \
    g4b::UserActionFactory::Instance().RegisterCreator(# _name, \
                                        & _name ## _ctor_function,        \
                                        & _name ## _creator_registered ); 

#define USERACTIONREG3( _ns, _name, _fqname ) \
namespace _ns { \
  g4b::UserAction* _name ## _ctor_function () { return new _fqname; }   \
  static bool _name ## _creator_registered =                                \
    g4b::UserActionFactory::Instance().RegisterCreator(# _fqname, \
                                        & _fqname ## _ctor_function,          \
                                        & _fqname ## _creator_registered );}

#define USERACTIONREG4( _nsa, _nsb, _name, _fqname )  \
namespace _nsa { \
 namespace _nsb { \
  g4b::UserAction* _name ## _ctor_function () { return new _fqname; }   \
  static bool _name ## _creator_registered =                                \
    g4b::UserActionFactory::Instance().RegisterCreator(# _fqname, \
                                        & _fqname ## _ctor_function,          \
                                        & _fqname ## _creator_registered );}}
#endif
