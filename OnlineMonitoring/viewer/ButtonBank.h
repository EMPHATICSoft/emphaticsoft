#ifndef OM_BUTTONBANK_H
#define OM_BUTTONBANK_H
#include "RQ_OBJECT.h"
#include "TGFrame.h"
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"

class TGPictureButton;
class TGLayoutHints;
class TTimer;

namespace om
{
  class ButtonBank : public TGCompositeFrame, public GUIModelSubscriber
  {
    RQ_OBJECT("om::ButtonBank")

  public:
    ButtonBank(TGWindow* w);
    ~ButtonBank();

    void Back();
    void Forward();
    void Pause();
    void Resume();
    void Update();
    void Histo();
    void PrintPlot();

    Bool_t HandleTimer(TTimer *t __attribute__((unused)));

    ///
    /// Complete the GUIModelDataSubscriber interface
    ///
    void GUIModelDataIssue(const GUIModelData& d, unsigned int which);

  private:
    TGLayoutHints*   fLhint;   ///< Layout hints
    TGPictureButton* fBack;    ///< Move back in plot history
    TGPictureButton* fForward; ///< Move forward in plot history
    TGPictureButton* fUpdate;  ///< Update current plot
    TGPictureButton* fPause;   ///< Pause automatic updates
    TGPictureButton* fResume;  ///< Resume automatic updates
    TGPictureButton* fHisto;   ///< Show histogram of 2D contents
    TGPictureButton* fPrint;   ///< Print plot to file

    TTimer*          fTimer;   ///< The timer to issue plot auto-refresh

    ClassDef(ButtonBank, 0)
  };
}

#endif
////////////////////////////////////////////////////////////////////////
