////////////////////////////////////////////////////////////////////////
#include "EventDisplayBase/evdb.h"
#include <cstdlib>
#include <string>
// ROOT includes
#include "TGClient.h"
#include "TGFrame.h"
#include "TGPicture.h"

//......................................................................

const TGWindow* evdb::TopWindow() { return gClient->GetRoot(); }

//......................................................................

TGPicturePool* evdb::PicturePool()
{
  static TGPicturePool* pp = 0;
  if (pp==0) {
    std::string path;
    
    const char* private_ctx = getenv("SRT_PRIVATE_CONTEXT");
    const char* public_ctx  = getenv("SRT_PUBLIC_CONTEXT");
    const char* root_ctx    = getenv("ROOTSYS");
    
    // Not every experiment uses SRT, so check that the SRT environmental 
    // variables are legit before adding them to the path
    if(private_ctx) { path += private_ctx; path += "/EventDisplay/icons:"; }
    if(public_ctx)  { path += public_ctx;  path += "/EventDisplay/icons:"; }
    path += root_ctx;    path += "/icons";
    
    pp = new TGPicturePool(gClient, path.c_str());
  }
  return pp;
}
////////////////////////////////////////////////////////////////////////
