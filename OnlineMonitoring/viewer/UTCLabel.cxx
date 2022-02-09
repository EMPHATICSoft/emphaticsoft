#include "OnlineMonitoring/viewer/UTCLabel.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include <cstdio>
#include <iostream>
#include "TText.h"
#include "TLine.h"
using namespace emph::onmon;

UTCLabel::UTCLabel()
{ }

//......................................................................

void UTCLabel::Clear()
{
  std::list<TText*>::const_iterator text   (fText.begin());
  std::list<TText*>::const_iterator textEnd(fText.end());
  for (;text!=textEnd; ++text) delete *text;
  fText.clear();

  std::list<TLine*>::const_iterator line   (fLine.begin());
  std::list<TLine*>::const_iterator lineEnd(fLine.end());
  for (;line!=lineEnd; ++line) delete *line;
  fLine.clear();
}

//......................................................................

UTCLabel::~UTCLabel() {
  this->Clear();
}

//......................................................................

void UTCLabel::Draw()
{
  std::list<TText*>::const_iterator text   (fText.begin());
  std::list<TText*>::const_iterator textEnd(fText.end());
  for (;text!=textEnd; ++text) (*text)->Draw();

  std::list<TLine*>::const_iterator line   (fLine.begin());
  std::list<TLine*>::const_iterator lineEnd(fLine.end());
  for (;line!=lineEnd; ++line) (*line)->Draw();
}

////////////////////////////////////////////////////////////////////////
