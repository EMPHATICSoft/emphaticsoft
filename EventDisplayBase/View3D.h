////////////////////////////////////////////////////////////////////////
/// \file  View3D.h
/// \brief A collection of 3D drawable objects
/// 
/// \version $Id: View3D.h,v 1.2 2012-03-10 06:57:39 bckhouse Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_VIEW3D_H
#define EVDB_VIEW3D_H
#include <list>

class TMarker3DBox;
class TPolyMarker3D;
class TPolyLine3D;
class TText;

namespace evdb {
  class View3D {
  public:
    View3D();
    ~View3D();
    
    void Draw();
    void Clear();
    
    TMarker3DBox&  AddMarker3DBox(double x,  double y,  double z,
				  double dx, double dy, double dz,
				  double th=0.0, double ph=0.0);
    TPolyMarker3D& AddPolyMarker3D(int n, int c, int st, double sz);
    TPolyLine3D&   AddPolyLine3D(int n, int c, int w, int s);
    TText&         AddText(double x, double y, const char* text);
    
  private:
    // Shared pool of unused objects. Any instance may take one for its own
    // purposes. This is the same scheme as used by View2D. See further
    // description there.
    static std::list<TMarker3DBox*>  fgMarker3DBoxL;
    static std::list<TPolyMarker3D*> fgPolyMarker3DL;
    static std::list<TPolyLine3D*>   fgPolyLine3DL;
    static std::list<TText*>         fgText3DL;

    std::list<TMarker3DBox*>  fMarker3DBoxL;  //!< List of 3D marker boxes
    std::list<TPolyMarker3D*> fPolyMarker3DL; //!< List of poly markers
    std::list<TPolyLine3D*>   fPolyLine3DL;   //!< List of poly lines
    std::list<TText*>         fText3DL;       //!< List of texts
  };
}

#endif // B_VIEW3D_H
////////////////////////////////////////////////////////////////////////
