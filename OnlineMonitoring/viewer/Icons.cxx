#include "OnlineMonitoring/viewer/Icons.h"
#include <unistd.h>
#include <cstdlib>
#include <string>
#include "TGClient.h"

namespace om {
  namespace Icons {

    //....................................................................

    const char* Find(const char* fname)
    {
      unsigned int itry;
      static std::string path;
      for (itry=0; itry<2; ++itry) {
	switch (itry) {
	case 0: path = "./"; break;
	case 1: path = getenv("CETPKG_SOURCE"); break;
	}
	if (itry==1 || itry==2) path += "/OnlineMonitoring/viewer/";
	path += fname;
	if (access(path.c_str(), F_OK) != -1) return path.c_str();
      }
      return "";
    }

    //....................................................................

    const TGPicture* Rewind()
    {
      static const std::string s = Find("control_rewind_blue.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
      return r;
    }

    //....................................................................

    const TGPicture* FastForward()
    {
      static const std::string s = Find("control_fastforward_blue.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
      return r;
    }

    //....................................................................

    const TGPicture* Pause()
    {
      static const std::string s = Find("control_pause_blue.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
    return r;
    }

    //....................................................................

    const TGPicture* Play()
    {
    static const std::string s = Find("control_play_blue.png");
    static const TGPicture* r = 0;
    if (r==0) r = gClient->GetPicture(s.c_str());
    return r;
    }

    //....................................................................

    const TGPicture* Repeat()
    {
      static const std::string s = Find("control_repeat_blue.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
      return r;
    }

    //....................................................................

    const TGPicture* FolderExplore()
    {
      static const std::string s = Find("folder_explore.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
      return r;
    }

    //......................................................................

    const TGPicture* Print()
    {
      static const std::string s = Find("control_print_blue.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
      return r;
    }

    //......................................................................

    const TGPicture* Histo()
    {
      static const std::string s = Find("control_histogram_blue.png");
      static const TGPicture* r = 0;
      if (r==0) r = gClient->GetPicture(s.c_str());
      return r;
    }
  }
}
////////////////////////////////////////////////////////////////////////
