/////////////////////////////////////////////////////////////////////////
/// \file Canvas.h
/// \brief Base class for define a detector display
///
/// \version $Id: Canvas.h,v 1.2 2011-01-23 16:08:50 p-nusoftart Exp $
/// \author  messier@indiana.edu
/////////////////////////////////////////////////////////////////////////
#ifndef EVDB_CANVAS_H
#define EVDB_CANVAS_H

#include "TQObject.h"
#include "RQ_OBJECT.h"
#include "EventDisplayBase/Printable.h"

class TGMainFrame;
class TGCompositeFrame;
class TGLayoutHints;
class TRootEmbeddedCanvas;
class TCanvas;

namespace evdb {
  class Canvas : public Printable {
    RQ_OBJECT("evdb::Canvas")
    
  public:
    Canvas(TGMainFrame* mf);
    virtual ~Canvas();
  
    virtual void Draw(const char* opt=0) = 0;
    
    // Sub-classes must define these
    virtual const char* PrintTag()    const {return "sub-class needs print tag"; }
    virtual const char* Description() const {return "sub-class needs description"; }
    virtual void        Print(const char* f);
  
    void Connect(); //!< Make signal/slot connections
    
  protected:
    TGCompositeFrame*    fFrame;     //!< Graphics frame
    TGLayoutHints*       fLayout;    //!< Layout hints for frame
    TRootEmbeddedCanvas* fEmbCanvas; //!< Embedded canvas
    TCanvas*             fCanvas;    //!< The ROOT drawing canvas
    
    unsigned short fXsize;       //!< Size of the canvas;
    unsigned short fYsize;       //!< Size of the canvas;
    float          fAspectRatio; //!< fYsize/fXsize
  };
}

#endif // EVDB_CANVAS_H
////////////////////////////////////////////////////////////////////////
