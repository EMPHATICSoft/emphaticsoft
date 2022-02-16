////////////////////////////////////////////////////////////////////////
/// \file  View2D.h
/// \brief A collection of drawable 2-D objects.
///
/// \version $Id: View2D.h,v 1.2 2012-03-10 06:40:29 bckhouse Exp $
/// \author  messier@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef EVDB_VIEW2D_H
#define EVDB_VIEW2D_H
#include <list>

class TMarker;
class TPolyMarker;
class TLine;
class TPolyLine;
class TArc;
class TBox;
class TText;
class TLatex;

namespace evdb {
  class View2D {
  public:
    View2D();
    ~View2D();
    
    void Draw();
    void Clear();
    
    // The list of object which make up the view
    TMarker&     AddMarker(double x, double y, int c, int st, double sz);
    TPolyMarker& AddPolyMarker(int n, int c, int st, double sz);
    TLine&       AddLine(double x1, double y1, double x2, double y2);
    TPolyLine&   AddPolyLine(int n, int c, int w, int s);
    TArc&        AddArc(double x,double t,double r,double a=0.,double b=360.);
    TBox&        AddBox(double x1, double y1, double x2, double y2);
    TText&       AddText(double x, double y, const char* text);
    TLatex&      AddLatex(double x, double y, const char* text);
    
  private:
    // Lists of cached drawing objects shared between all instances. Allows us
    // to avoid costly news and deletes.
    static std::list<TMarker*>     fgMarkerL;
    static std::list<TPolyMarker*> fgPolyMarkerL;
    static std::list<TLine*>       fgLineL;
    static std::list<TPolyLine*>   fgPolyLineL;
    static std::list<TArc*>        fgArcL;
    static std::list<TBox*>        fgBoxL;
    static std::list<TText*>       fgTextL;
    static std::list<TLatex*>      fgLatexL;

    // Lists of drawing objects currently being used by this view. Will be
    // returned to the shared lists when done with them.
    std::list<TMarker*>     fMarkerL;
    std::list<TPolyMarker*> fPolyMarkerL;
    std::list<TLine*>       fLineL;
    std::list<TPolyLine*>   fPolyLineL;
    std::list<TArc*>        fArcL;
    std::list<TBox*>        fBoxL;
    std::list<TText*>       fTextL;
    std::list<TLatex*>      fLatexL;
  };
}

#endif // EVDB_VIEW2D_H
////////////////////////////////////////////////////////////////////////
