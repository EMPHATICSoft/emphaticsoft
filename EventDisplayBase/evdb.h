////////////////////////////////////////////////////////////////////////
//  $Id: evdb.h,v 1.4 2011-04-05 22:26:55 messier Exp $
//
//! Collection of global resources for the event display
//
//  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_H
#define EVDB_H

class TGMainFrame;
class TGWindow;
class TGPicturePool;

/// Base package for construction of an event display
namespace evdb {
  const TGWindow* TopWindow();
  TGPicturePool*  PicturePool();
}

#endif // EVDB_H
////////////////////////////////////////////////////////////////////////
