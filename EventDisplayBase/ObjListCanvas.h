/////////////////////////////////////////////////////////////////////////
/// \file ObjListCanvas.h
/// \brief Base class for displaying lists of objects (eg, MC truth, reco,
/// etc.)
///
/// \version $Id: ObjListCanvas.h,v 1.1.1.1 2010-12-22 16:18:52 p-nusoftart Exp $
/// \author  jpaley@anl.gov
/////////////////////////////////////////////////////////////////////////
#ifndef EVDB_OBJLISTCANVAS_H
#define EVDB_OBJLISTCANVAS_H

#include "TQObject.h"
#include "RQ_OBJECT.h"
#ifndef EVDB_PRINTABLE_H
#include "EventDisplayBase/Printable.h"
#endif
class TGMainFrame;
class TGCompositeFrame;
class TGLayoutHints;
class TRootEmbeddedCanvas;
class TCanvas;

namespace evdb {
  class ObjListCanvas : public Printable {
    RQ_OBJECT("evdb::ObjListCanvas")
    
  public:
    ObjListCanvas(TGMainFrame* mf);
    virtual ~ObjListCanvas();
  
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

#endif // EVDB_OBJLISTCANVAS_H
////////////////////////////////////////////////////////////////////////
