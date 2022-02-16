///
/// \file   ParameterSetEditDialog.h
/// \brief  Pop-up window for editing parameter sets
/// \author messier@indiana.edu
///
/// These classes build a dialog window for parameter sets. It can
/// handle "standard" parameter sets like this:
///
/// ServiceConfig {
////  A: 0
///   B: 2
///   C: [0,1]
///   D:"String"
///   E:["String","Strung"]
/// }
///
/// for which it will set up a set of text edit boxes. It can also
/// handle parameter sets that have been made "gui-aware" and follow
/// this format:
///
/// ServiceConfig {
///  A:{val:0 gui:"rb:Choice 1, Choice 2" doc:"Choose one or two"}
///  B:{val:2 gui:"cb:Choice A, Choice B" doc:"Select A or B or both"}
/// }
///
/// In these cases, "val" is the set value of the parameter, and "doc"
/// explains what the parameter is or does. Instructions for building
/// the gui are contained in "gui". Valid tags are:
///
/// gui:"te"                 : Text entry box
/// gui:"lbs:opt1,opt2,opt3" : A list box, single selection allowed
/// gui:"lbm:opt1,opt2,opt3" : A list box, multiple selections allowed
/// gui:"rb:opt1,opt2,opt3"  : A set of radio buttons
/// gui:"cb:opt1,opt2,opt3"  : A set of check boxes
/// gui:"sl:v1,v2            : A slider to choose values between v1 and v2
/// gui:"sli:v1,v2           : A slider for int values between v1 and v2
///
/// When retreiving parameters note that:
///
/// o List boxes will give results which are a single, or possibly
/// multiple, strings representing all the choices the user has made.
///
/// o Radio buttons will give a single integer result showing the index
/// of the option selected (0,1,2,...)
///
/// o Check boxes will give a single integer with bits set indicating
/// which options were selected (opt1 = 0x01, opt2=0x02, opt3=0x03,
/// etc.)
///
/// o Sliders will give one or two floats depending on how they are
/// configured. If its one float, it will be the center value
/// selected. If two it will be the low and high values selected. To
/// configure for low and high values initialize the slider with two
/// values like this example:
///
/// TimeWindow {
///   val:[210,230]
///   gui:"sl:-50,550"
///   doc:"Select low and high values between -50 and 550" }
/// }
///
#ifndef EVDB_PARAMETERSETEDITDIALOG_H
#define EVDB_PARAMETERSETEDITDIALOG_H
#include "RQ_OBJECT.h"
#include "TGFrame.h"
#include "TQObject.h"
class TGTab;
class TGCanvas;
class TGTableLayout;
class TGTableLayoutHints;
class TGListBox;
class TGDoubleSlider;
class TGTextEntry;
class TGRadioButton;
class TGCheckButton;
namespace fhicl {
  class ParameterSet;
}

namespace evdb {
  class ParameterSetEditFrame;
  ///===================================================================
  ///
  /// \brief A single row for editing a single parameter in a set
  ///
  class ParameterSetEditRow {
    RQ_OBJECT("evdb::ParameterSetEditRow")
  public:
    ParameterSetEditRow(ParameterSetEditFrame* frame,
                        TGHorizontalFrame* lhs,
                        TGHorizontalFrame* rhs,
                        const fhicl::ParameterSet& ps,
                        const std::string& key);
    ~ParameterSetEditRow();

    void Finalize();
    std::string AsFHICL() const;

    void TextEntryReturnPressed();
    void ListBoxSelectionChanged();
    void ListBoxSelected(int id);
    void RadioButtonClicked();
    void CheckButtonClicked();
    void SliderPositionChanged();

  private:
    void SetupTextEntry(TGCompositeFrame* f,
                        unsigned int flags,
                        const std::vector<std::string>& value);

    void SetupListBox(TGCompositeFrame* f,
                      const std::vector<std::string>& choice,
                      const std::vector<std::string>& value,
                      bool ismulti);

    void SetupRadioButtons(TGCompositeFrame* f,
                           const std::vector<std::string>& choice,
                           const std::vector<std::string>& value);

    void SetupCheckButton(TGCompositeFrame* f,
                          const std::vector<std::string>& choice,
                          const std::vector<std::string>& value);

    void SetupSlider(TGCompositeFrame* f,
                     const std::vector<std::string>& choice,
                     const std::vector<std::string>& value);

  private:
    static bool IsLegalGUItag(const std::string& s);
    static void ParseGUItag(const std::string& guitag,
                            std::string& frame,
                            std::vector<std::string>& choice);
    static void UnpackParameter(const fhicl::ParameterSet& ps,
                                const std::string& key,
                                unsigned int& flags,
                                std::string& tag,
                                std::vector<std::string>& choice,
                                std::vector<std::string>& value,
                                std::string& gui,
                                std::string& doc);

  public:
    ParameterSetEditFrame* fFrame; ///< The parent frame
  public:
    TGHorizontalFrame* fMother;       ///< Top level frame
    TGLayoutHints* fRightLH{nullptr}; ///< Align to right
    TGLayoutHints* fLeftLH{nullptr};  ///< Align to left
    TGTextButton* fLabel{nullptr};    ///< Label on the left
  public:
    TGTextEntry* fTextEntry{nullptr};
    TGListBox* fListBox{nullptr};
    TGDoubleSlider* fSlider{nullptr};
    std::vector<TGRadioButton*> fRadioButton;
    std::vector<TGCheckButton*> fCheckButton;

  public:
    unsigned int fParamFlags;
    std::string fKEY;
    std::string fGUI;
    std::string fDOC;
    std::vector<std::string> fChoice;
    std::string fValue;
  };

  ///===================================================================
  ///
  /// \brief A frame for editing a single paramter set
  ///
  class ParameterSetEditFrame {
    RQ_OBJECT("evdb:ParameterSetEditFrame")
  public:
    ParameterSetEditFrame(TGCompositeFrame* mother, unsigned int psetid);
    ~ParameterSetEditFrame();

    std::string AsFHICL() const;

    void HandleMouseWheel(Event_t* event);
    void Modified();
    void Finalize();

  public:
    TGCompositeFrame* fTopFrame;
    TGCanvas* fCanvas;
    TGLayoutHints* fCanvasH;
    TGCompositeFrame* fContainer;
    TGTableLayout* fLayout;
    std::vector<TGHorizontalFrame*> fLHS;
    std::vector<TGHorizontalFrame*> fRHS;
    std::vector<TGTableLayoutHints*> fLHSHints;
    std::vector<TGTableLayoutHints*> fRHSHints;
    std::vector<ParameterSetEditRow*> fRow;

  public:
    unsigned int fParameterSetID;
    bool fIsModified;
  };

  ///===================================================================
  ///
  /// \brief Top-level interface to all parameter sets
  ///
  class ParameterSetEditDialog : public TGTransientFrame {
    RQ_OBJECT("evdb::ParameterSetEditDialog")
  public:
    ParameterSetEditDialog(unsigned int psetid);
    ~ParameterSetEditDialog();

    void Apply();
    void Cancel();
    void Done();
    void CloseWindow();
    std::string TabName(const std::string& s);

  private:
    TGTab* fTGTab;
    TGHorizontalFrame* fButtons;
    TGTextButton* fApply;
    TGTextButton* fCancel;
    TGTextButton* fDone;

  private:
    unsigned int fParameterSetID;
    std::vector<ParameterSetEditFrame*> fFrames;

    ClassDef(ParameterSetEditDialog, 0);
  };
}

#endif
////////////////////////////////////////////////////////////////////////
