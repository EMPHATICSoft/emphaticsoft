#include "OnlineMonitoring/viewer/CurrentHistogram.h"
#include <iostream>
using namespace emph::onmon;

CurrentHistogram::CurrentHistogram() :
  fHistory(0),
  fPlace(fHistory.end())
{ }

//......................................................................

int CurrentHistogram::Set(const char* nm) 
{

  //
  // Check if we're already there...
  //
  if (fPlace != fHistory.end() && *fPlace == nm) return 0;

  //
  // If we're at the front of the deque, add the new histo to the front.
  //
  if(fPlace == fHistory.begin()) fHistory.push_front(std::string(nm));

  // If we're in the middle of the deque, erase everything forward of this spot
  // and add the new histo.
  if(fPlace != fHistory.begin()) {

    // Erase entries between the [beginning and current place).
    std::deque<std::string>::iterator i1(fHistory.begin());
    std::deque<std::string>::iterator i2(fPlace);
    fHistory.erase(i1, i2);

    // Add histo to the new beginning.
    fHistory.push_front(std::string(nm));

  }

  //
  // If we've grown too big, truncate by dropping oldest entries at
  // beginning of list.
  //
  if (fHistory.size() > kMaxHistory) fHistory.resize(kMaxHistory);

  //
  // Set our place in the history to the new end of the list
  //
  fPlace = fHistory.begin();

  return 1;

}

//......................................................................

bool CurrentHistogram::ForwardOK() const 
{
  if (fPlace == fHistory.begin()) return false;
  return true;
}

//......................................................................

bool CurrentHistogram::BackwardOK() const
{
  std::deque<std::string>::iterator i(fPlace+1);
  if (i >= fHistory.end()) return false;
  return true;
}

//......................................................................

bool CurrentHistogram::ReloadOK() const
{
  if (fPlace == fHistory.end() || *fPlace == "") return false;
  return true;
}

//......................................................................

int CurrentHistogram::Forward() 
{
  if (this->ForwardOK()) { --fPlace; return 1; }
  return 0;
}

//......................................................................

int CurrentHistogram::Backward() 
{
  if (this->BackwardOK()) { ++fPlace; return 1; }
  return 0;
}

//......................................................................

const char* CurrentHistogram::Current() const
{
  if (fPlace == fHistory.end()) return "";
  return fPlace->c_str();
}

////////////////////////////////////////////////////////////////////////
