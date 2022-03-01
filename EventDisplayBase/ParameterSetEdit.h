#ifndef EDVB_PARAMETERSETEDIT_H
#define EDVB_PARAMETERSETEDIT_H
#include "TQObject.h"
#include "RQ_OBJECT.h"
#include "TGFrame.h"
#include <vector>
#include <string>
class TGCanvas;
class TGMainFrame;
class TGCompositeFrame;
class TGGroupFrame;
class TGMatrixLayout;
class TGLayoutHints;
class TGLabel;
class TGTextButton;
class TGTextEntry;
class TGVScrollBar;

namespace evdb {
  /// Helper class to setup scroll bars in evdb::ParameterSetEdit
  class ParamFrame {
    RQ_OBJECT("evdb::ParamFrame")
    
  private:
    TGGroupFrame     *fFrame;
    TGCanvas         *fCanvas;
    TGMatrixLayout   *fML;
    
  public:
    ParamFrame(const TGWindow *p, 
	       std::vector<std::string>&  names,
	       std::vector<std::string>&  value,
	       std::vector<TGTextEntry*>& fT2
	       );
    virtual ~ParamFrame() { delete fFrame; }
    
    TGGroupFrame *GetFrame() const { return fFrame; }
    
    void SetCanvas(TGCanvas *canvas) { fCanvas = canvas; }
    void HandleMouseWheel(Event_t *event);

    int  GetHeight() const;
    int  GetWidth() const;
    
  };
}

//......................................................................

namespace evdb {
  /// Dialog window to edit a parameter set
  class ParameterSetEdit : public TGTransientFrame 
  {
    RQ_OBJECT("evdb::ParameterSetEdit")
  public:
    ParameterSetEdit(TGMainFrame* mf, 
		     const std::string& module,
		     const std::string& label,
		     const std::string& params,
		     std::string*       newpset);
    ~ParameterSetEdit();
    
    int Edit();
    
    void Apply();
    void Cancel();
    void Done();
    void CloseWindow();

    void HandleTab();

  private:
    TGCompositeFrame*         fF1;
    ParamFrame*               fParam;
    TGCanvas*                 fCanvas;
    TGCompositeFrame*         fF3;
    TGLayoutHints*            fLH1;
    TGLayoutHints*            fLH2;
    TGLayoutHints*            fLH3;
    TGLayoutHints*            fLH4;
    TGLabel*                  fL1;
    TGTextButton*             fB3;
    TGTextButton*             fB4;
    TGTextButton*             fB5;
    std::vector<TGTextEntry*> fT2;
  private:
    std::vector<std::string>  fName;
    std::vector<std::string>  fType;
    std::vector<std::string>  fValue;
    std::string*              fResult; ///< New parameter set

    ClassDef(ParameterSetEdit, 0);
  };
}
#endif // EDVB_PARAMETERSETEDIT_H
