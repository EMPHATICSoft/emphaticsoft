///
/// \file   ParameterSetEditDialog.cxx
/// \brief  Pop-up window for editing parameter sets
/// \author messier@indiana.edu
///
#include "EventDisplayBase/ParameterSetEditDialog.h"
#include "EventDisplayBase/NavState.h"
#include "EventDisplayBase/ServiceTable.h"

#include "TGButton.h"
#include "TGCanvas.h"
#include "TGDoubleSlider.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGListBox.h"
#include "TGTab.h"
#include "TGTableLayout.h"
#include "TGTextEntry.h"
#include "TROOT.h"

#include <sstream>

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

using namespace evdb;
using namespace std::string_literals;

namespace {
  // Window and row sizes in units of pixels
  constexpr unsigned int kWidth = 500 * 11 / 10;
  constexpr unsigned int kHeight = 500 * 11 / 10;
  constexpr unsigned int kRowW = kWidth - 150;
  constexpr unsigned int kRowH = 18;

  // Flags to help us decide what sort of parameter we need to build a
  // GUI for.
  constexpr int kSINGLE_VALUED_PARAM = 1 << 0; // Expect single value
  constexpr int kVECTOR_PARAM = 1 << 1;        // Expect multiple values
  // constexpr int kVECTOR_OF_VECTOR_PARAM = 1<<2; // Expect multiple values -
  // never used
  constexpr int kHAVE_GUI_TAGS = 1 << 3; // GUI tags are present
  constexpr int kNO_GUI_TAGS = 1 << 4;   // GUI tags are not present
  constexpr int kINTEGER_PARAM = 1 << 5; // Force the value to be int
  constexpr int kPARAMETER_SET_PARAM = 1
                                       << 6; // Value is a parameter set itself

  // The short letter codes for the various GUI objects supported. Also
  // provide a list of all possible tags.

  std::string const kTEXT_ENTRY = "te"; // A text edit box
  std::string const kLIST_BOX_SINGLE =
    "lbs"; // A list box, single choice allowed
  std::string const kLIST_BOX_MULTI =
    "lbm"; // A list box, multuiple choices allowed
  std::string const kRADIO_BUTTONS = "rb"; // Radio buttons
  std::string const kCHECK_BOX = "cb";     // Check boxes
  std::string const kSLIDER = "sl";        // Slider bar
  std::string const kSLIDER_INT = "sli";   // Slider bar, limit to integers

  std::vector<std::string> const gsGUITAG{kTEXT_ENTRY,
                                          kLIST_BOX_SINGLE,
                                          kLIST_BOX_MULTI,
                                          kRADIO_BUTTONS,
                                          kCHECK_BOX,
                                          kSLIDER,
                                          kSLIDER_INT};
  std::string maybe_quoted(std::string element)
  {
    // If there are any symbols in the string that have special
    // meaning in FHiCL, then escape the string with quotes.
    if (element.find_first_of(":[{}]@") != std::string::npos) {
      return '"' + element + '"';
    }
    return element;
  }
}

//======================================================================
//
// ParameterSetEditRow methods
//
ParameterSetEditRow::ParameterSetEditRow(ParameterSetEditFrame* frame,
                                         TGHorizontalFrame* lhs,
                                         TGHorizontalFrame* rhs,
                                         const fhicl::ParameterSet& ps,
                                         const std::string& key)
  : fFrame(frame), fKEY(key)
{
  //
  // Extract information about the parameter for which we are building
  // the GUI
  //
  std::string tag;                 // What sort of frame to build?
  std::vector<std::string> values; // What is the current value?
  this->UnpackParameter(ps, key, fParamFlags, tag, fChoice, values, fGUI, fDOC);
  if (empty(values)) {
    // What happened here? We'll crash if we continue though, so bail out.
    return;
  }

  if (fParamFlags & kVECTOR_PARAM) {
    fValue = "[";
    auto const n = size(values);
    for (std::size_t i = 0; i < n; ++i) {
      fValue += values[i];
      if (i + 1 < n)
        fValue += ",";
      else
        fValue += "]";
    }
  } else if (fParamFlags & kPARAMETER_SET_PARAM) {
    fValue = "{";
    fValue += values[0];
    fValue += "}";
  } else {
    fValue = values[0];
  }

  fLeftLH = new TGLayoutHints(kLHintsLeft, 1, 1, 0, 0);
  fRightLH = new TGLayoutHints(kLHintsRight, 1, 1, 0, 0);

  fLabel = new TGTextButton(lhs,
                            key.c_str(),
                            -1,
                            TGButton::GetDefaultGC()(),
                            TGTextButton::GetDefaultFontStruct(),
                            0);
  lhs->AddFrame(fLabel);
  fLabel->SetToolTipText(fDOC.c_str());
  fLabel->SetTextJustify(kTextRight);

  if (tag == kTEXT_ENTRY) {
    this->SetupTextEntry(rhs, fParamFlags, values);
  }
  if (tag == kLIST_BOX_SINGLE) {
    this->SetupListBox(rhs, fChoice, values, false);
  }
  if (tag == kLIST_BOX_MULTI) {
    this->SetupListBox(rhs, fChoice, values, true);
  }
  if (tag == kRADIO_BUTTONS) {
    this->SetupRadioButtons(rhs, fChoice, values);
  }
  if (tag == kCHECK_BOX) {
    this->SetupCheckButton(rhs, fChoice, values);
  }
  if (tag == kSLIDER) {
    this->SetupSlider(rhs, fChoice, values);
  }
  if (tag == kSLIDER_INT) {
    fParamFlags |= kINTEGER_PARAM;
    this->SetupSlider(rhs, fChoice, values);
  }
}

//......................................................................

ParameterSetEditRow::~ParameterSetEditRow()
{
  cet::for_all(fCheckButton, std::default_delete<TGCheckButton>{});
  cet::for_all(fRadioButton, std::default_delete<TGRadioButton>{});
  delete fSlider;
  delete fListBox;
  delete fTextEntry;
  delete fLeftLH;
  delete fRightLH;
  delete fLabel;
}

//......................................................................

void
ParameterSetEditRow::UnpackParameter(const fhicl::ParameterSet& p,
                                     const std::string& key,
                                     unsigned int& flag,
                                     std::string& tag,
                                     std::vector<std::string>& choice,
                                     std::vector<std::string>& value,
                                     std::string& gui,
                                     std::string& doc)
{
  flag = 0;

  // Try to extract GUI tags
  try {
    gui = p.get<std::string>(key + ".gui");
    doc = p.get<std::string>(key + ".doc");
    flag |= kHAVE_GUI_TAGS;
  }
  catch (...) {
    // If they aren't there, try extracting it as a normal
    // parameter. Default to providing the user with a text entry box.
    gui = kTEXT_ENTRY;
    doc = "See .fcl file for documentation...";
    flag |= kNO_GUI_TAGS;
  }

  // Parse out the GUI string to find out what type of frame to build
  // and the choices we should present to the user
  ParseGUItag(gui, tag, choice);

  // Now extract the assigned value(s) of the parameter
  //
  // The key is either just the key, or in the case of GUI-enabled
  // parameters the key name with ".val" appended
  std::string valkey = key;
  if (flag & kHAVE_GUI_TAGS)
    valkey += ".val";

  // Try first to extract a single value.
  try {
    auto const v = p.get<std::string>(valkey);
    value.push_back(maybe_quoted(v));
    flag |= kSINGLE_VALUED_PARAM;
    return;
  }
  catch (...) {
  }

  // Could not extract as single value; try extracting multiple values
  try {
    auto tmp = p.get<std::vector<std::string>>(valkey);
    for (auto& element : tmp) {
      element = maybe_quoted(element);
    }
    value = move(tmp);
    flag |= kVECTOR_PARAM;
    if (empty(value))
      value.push_back("");
    return;
  }
  catch (...) {
  }

  // Yikes - extracting multiple values failed; try vector of vectors, perhaps?
  try {
    std::vector<std::vector<std::string>> vv;
    vv = p.get<std::vector<std::vector<std::string>>>(valkey);

    // Vectors of vectors are treated as vectors of
    // std::strings. The strings assigned to the values are
    // strings that FHiCL will parse as vectors. So, this:
    //
    // [ [0,0], [1,1] ]
    //
    // is represented as:
    //
    // size(value)=2, value[0]="[0,0]", value[1]="[1,1]"

    flag |= kVECTOR_PARAM;
    for (std::size_t i = 0; i < size(vv); ++i) {
      std::string s{"["};
      auto const m = size(vv[i]);
      for (std::size_t j = 0; j < m; ++j) {
        s += maybe_quoted(vv[i][j]);
        if (j + 2 < m)
          s += ",";
        else
          s += "]";
      }
      value.push_back(s);
    }
    if (empty(vv))
      value.push_back("[[]]");
    return;
  }
  catch (...) {
  }

  // Above failed; what about another fhicl::ParameterSet?
  try {
    auto const v = p.get<fhicl::ParameterSet>(valkey);
    flag |= kPARAMETER_SET_PARAM;
    value.push_back(v.to_string());
    return;
  }
  catch (...) {
  }

  // Now we are very stuck.
  MF_LOG_ERROR("ParameterSetEditDialog") << "Failed to parse " << key << "\n"
                                         << p.to_string();
}

//......................................................................
//
// Parse out what we can from the "gui" tag. Expected format is:
// "frame_tag:choice1,choice2,choice3"
//
void
ParameterSetEditRow::ParseGUItag(const std::string& guitag,
                                 std::string& frame,
                                 std::vector<std::string>& choice)
{
  // Get the frame name. Should be piece just before the ":"
  choice.clear();
  size_t icolon = guitag.find(':');
  if (icolon == std::string::npos)
    frame = guitag;
  else
    frame = guitag.substr(0, icolon);
  if (!IsLegalGUItag(frame))
    frame = kTEXT_ENTRY;

  // Get the list of choices. Should be comma separated.
  size_t icomma = icolon;
  while (icomma != std::string::npos) {
    size_t spos = icomma + 1;
    size_t epos = guitag.find(',', spos);
    std::string s = guitag.substr(spos, epos - spos);
    choice.push_back(s);
    icomma = epos;
  }
}

//......................................................................

bool
ParameterSetEditRow::IsLegalGUItag(const std::string& s)
{
  for (std::size_t i = 0; i < gsGUITAG.size(); ++i) {
    if (s == gsGUITAG[i])
      return true;
  }
  MF_LOG_ERROR("ParameterSetEditDialog") << s << " is not a legal GUI tag.";
  return false;
}

//......................................................................

void
ParameterSetEditRow::SetupTextEntry(TGCompositeFrame* f,
                                    unsigned int flags,
                                    const std::vector<std::string>& value)
{
  static TColor* c = gROOT->GetColor(41);

  fTextEntry = new TGTextEntry(f);
  f->AddFrame(fTextEntry);
  fTextEntry->SetTextColor(c);

  fTextEntry->Connect("ReturnPressed()",
                      "evdb::ParameterSetEditRow",
                      this,
                      "TextEntryReturnPressed()");

  std::string buff;
  if (flags & kVECTOR_PARAM)
    buff += "[";
  if (flags & kPARAMETER_SET_PARAM)
    buff += "{";
  for (std::size_t i = 0; i < size(value); ++i) {
    buff += value[i];
    if ((i + 1) != size(value))
      buff += ",";
  }
  if (flags & kVECTOR_PARAM)
    buff += "]";
  if (flags & kPARAMETER_SET_PARAM)
    buff += "}";
  fTextEntry->SetText(buff.c_str(), 0);
  fTextEntry->Resize(kRowW, kRowH);
}

//......................................................................

void
ParameterSetEditRow::SetupListBox(TGCompositeFrame* f,
                                  const std::vector<std::string>& choice,
                                  const std::vector<std::string>& value,
                                  bool ismulti)
{
  fListBox = new TGListBox(f);
  f->AddFrame(fListBox);
  if (ismulti)
    fListBox->SetMultipleSelections();

  for (size_t i = 0; i < choice.size(); ++i) {
    fListBox->AddEntry(choice[i].c_str(), i);
    for (size_t j = 0; j < size(value); ++j) {
      if (value[j] == choice[i])
        fListBox->Select(i);
    }
  }

  fListBox->Connect("SelectionChanged()",
                    "evdb::ParameterSetEditRow",
                    this,
                    "ListBoxSelectionChanged()");
  fListBox->Connect("Selected(Int_t)",
                    "evdb::ParameterSetEditRow",
                    this,
                    "ListBoxSelected(int)");

  size_t h = kRowH * choice.size();
  if (h > 3 * kRowH)
    h = 3 * kRowH;
  fListBox->Resize(kRowW, h);
}

//......................................................................

void
ParameterSetEditRow::SetupRadioButtons(TGCompositeFrame* f,
                                       const std::vector<std::string>& choice,
                                       const std::vector<std::string>& value)
{
  unsigned int v = atoi(value[0].c_str());

  for (size_t i = 0; i < choice.size(); ++i) {
    TGRadioButton* b = new TGRadioButton(f, choice[i].c_str(), i);
    f->AddFrame(b);

    b->SetTextJustify(kTextLeft);
    b->Connect(
      "Clicked()", "evdb::ParameterSetEditRow", this, "RadioButtonClicked()");

    if (i == v)
      b->SetState(kButtonDown);

    fRadioButton.push_back(b);
  }
}

//......................................................................

void
ParameterSetEditRow::SetupCheckButton(TGCompositeFrame* f,
                                      const std::vector<std::string>& choice,
                                      const std::vector<std::string>& value)
{
  unsigned int mask;
  unsigned int v = atoi(value[0].c_str());
  for (size_t i = 0; i < choice.size(); ++i) {
    TGCheckButton* b = new TGCheckButton(f, choice[i].c_str(), i);
    f->AddFrame(b);
    b->Connect(
      "Clicked()", "evdb::ParameterSetEditRow", this, "CheckButtonClicked()");

    mask = (0x1) << i;
    if (v & mask)
      b->SetState(kButtonDown);

    fCheckButton.push_back(b);
  }
}
//......................................................................
void
ParameterSetEditRow::SetupSlider(TGCompositeFrame* f,
                                 const std::vector<std::string>& choice,
                                 const std::vector<std::string>& value)
{
  fTextEntry = new TGTextEntry(f);
  f->AddFrame(fTextEntry);

  std::string t;
  if (size(value) == 1) {
    t = value[0];
  }
  if (size(value) == 2) {
    t = "[";
    t += value[0];
    t += ",";
    t += value[1];
    t += "]";
  }
  fTextEntry->SetText(t.c_str());

  fTextEntry->Connect("ReturnPressed()",
                      "evdb::ParameterSetEditRow",
                      this,
                      "TextEntryReturnPressed()");

  fSlider = new TGDoubleHSlider(f, 100, kDoubleScaleBoth);
  f->AddFrame(fSlider);

  float min = atof(choice[0].c_str());
  float max = atof(choice[1].c_str());

  float pos1 = 0;
  float pos2 = 0;
  if (size(value) == 1) {
    pos1 = atof(value[0].c_str());
    pos2 = pos1;
  }
  if (size(value) == 2) {
    pos1 = atof(value[0].c_str());
    pos2 = atof(value[1].c_str());
  }

  fSlider->SetRange(min, max);
  fSlider->SetPosition(pos1, pos2);

  fSlider->Connect("PositionChanged()",
                   "evdb::ParameterSetEditRow",
                   this,
                   "SliderPositionChanged()");

  fTextEntry->Resize(kRowW * 1 / 5, kRowH);
  fSlider->Resize(kRowW * 4 / 5, 10 * kRowH);
}

//......................................................................

void
ParameterSetEditRow::TextEntryReturnPressed()
{
  if (fTextEntry == nullptr)
    return;

  const char* text = fTextEntry->GetBuffer()->GetString();

  static TColor* c = gROOT->GetColor(1);
  fTextEntry->SetTextColor(c);

  // If we also have a slider connected to this frame, make sure its
  // state is updated
  if (fSlider) {
    int n = 0;
    float f1 = 0, f2 = 0;
    n = sscanf(text, "[%f, %f]", &f1, &f2);
    if (n != 2) {
      n = sscanf(text, "%f", &f1);
      f2 = f1;
    }
    fSlider->SetPosition(f1, f2);
  }
  fValue = text;
  fFrame->Modified();
}

//......................................................................

void
ParameterSetEditRow::ListBoxSelectionChanged()
{
  // Only need to handle list boxes where multiple selections are
  // allowed here.
  if (fListBox->GetMultipleSelections() == false)
    return;

  fValue = "[";
  TList selections;
  fListBox->GetSelectedEntries(&selections);
  TGLBEntry* sel;
  bool isfirst = true;
  for (unsigned int i = 0;; ++i) {
    sel = (TGLBEntry*)selections.At(i);
    if (sel == 0)
      break;
    if (!isfirst)
      fValue += ",";
    fValue += fChoice[sel->EntryId()];
    isfirst = false;
  }
  fValue += "]";
  fFrame->Modified();
}

//......................................................................

void
ParameterSetEditRow::ListBoxSelected(int id)
{
  //
  // Only handle single selection list boxes here
  //
  if (fListBox->GetMultipleSelections())
    return;
  fValue = fChoice[id];
  fFrame->Modified();
}
//......................................................................

void
ParameterSetEditRow::RadioButtonClicked()
{
  unsigned int value = 0;
  TGButton* b = (TGButton*)gTQSender;
  int id = b->WidgetId();
  for (size_t i = 0; i < fRadioButton.size(); ++i) {
    if (fRadioButton[i]->WidgetId() != id) {
      fRadioButton[i]->SetState(kButtonUp);
    } else
      value = i;
  }
  char buff[256];
  sprintf(buff, "%d", value);
  fValue = buff;
  fFrame->Modified();
}

//......................................................................

void
ParameterSetEditRow::CheckButtonClicked()
{
  int value = 0;
  for (std::size_t i = 0; i < fCheckButton.size(); ++i) {
    if (fCheckButton[i]->IsDown())
      value |= 1 << i;
  }
  char buff[256];
  sprintf(buff, "%d", value);
  fValue = buff;
  fFrame->Modified();
}

//......................................................................

void
ParameterSetEditRow::SliderPositionChanged()
{
  char buff[1024];
  float mn, mx;
  fSlider->GetPosition(mn, mx);

  float const ave = 0.5 * (mn + mx);

  if (fParamFlags & kINTEGER_PARAM) {
    int const mni = rint(mn);
    int const mxi = rint(mx);
    int const avei = rint(ave);
    if (fParamFlags & kVECTOR_PARAM) {
      sprintf(buff, "[%d, %d]", mni, mxi);
    } else {
      sprintf(buff, "%d", avei);
    }
  } else {
    if (fParamFlags & kVECTOR_PARAM) {
      sprintf(buff, "[%.1f, %.1f]", mn, mx);
    } else {
      sprintf(buff, "%.1f", ave);
    }
  }
  fTextEntry->SetText(buff);
  fValue = buff;
  fFrame->Modified();
}

//......................................................................

void
ParameterSetEditRow::Finalize()
{
  if (fTextEntry && fValue != fTextEntry->GetBuffer()->GetString()) {
    this->TextEntryReturnPressed();
  }
}

//......................................................................

std::string
ParameterSetEditRow::AsFHICL() const
{
  std::ostringstream s;
  if (fParamFlags & kNO_GUI_TAGS) {
    s << fKEY << ":" << fValue << " ";
  } else {
    s << fKEY << ": { "
      << "val:" << fValue << " "
      << "gui:\"" << fGUI << "\" "
      << "doc:\"" << fDOC << "\" "
      << "}";
  }
  return s.str();
}

//======================================================================
//
// ParameterSetEditFrame methods
//
ParameterSetEditFrame::ParameterSetEditFrame(TGCompositeFrame* mother,
                                             unsigned int psetid)
  : fParameterSetID(psetid), fIsModified(false)
{
  fCanvas = new TGCanvas(mother, kWidth - 6, kHeight - 50);
  fCanvasH = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
  mother->AddFrame(fCanvas, fCanvasH);

  fContainer = new TGCompositeFrame(fCanvas->GetViewPort());
  fCanvas->SetContainer(fContainer);

  // Locate the parameter set connected to this frame
  const ServiceTable& st = ServiceTable::Instance();
  const fhicl::ParameterSet& pset = st.GetParameterSet(psetid);
  std::vector<std::string> keys = pset.get_names();
  std::size_t const nkey = size(keys);

  // Count the number of "non system" parameters - each of these will
  // need an row in the dialog window.
  unsigned int nparam = 0;
  for (auto const& key : keys) {
    if (!((key == "service_type"s) || (key == "module_type"s) ||
          (key == "module_label"s))) {
      ++nparam;
    }
  }

  //
  // Build the layout
  //
  fLayout = new TGTableLayout(fContainer, nparam, 2);
  fContainer->SetLayoutManager(fLayout);

  for (std::size_t i = 0, j = 0; i < nkey; ++i) {
    if (!((keys[i] == "service_type") || (keys[i] == "module_type") ||
          (keys[i] == "module_label"))) {

      TGHorizontalFrame* lhs = new TGHorizontalFrame(fContainer);
      TGHorizontalFrame* rhs = new TGHorizontalFrame(fContainer);

      TGTableLayoutHints* lhsh = new TGTableLayoutHints(0, 1, j, j + 1);
      TGTableLayoutHints* rhsh = new TGTableLayoutHints(1, 2, j, j + 1);

      fContainer->AddFrame(lhs, lhsh);
      fContainer->AddFrame(rhs, rhsh);

      fLHS.push_back(lhs);
      fRHS.push_back(rhs);
      fLHSHints.push_back(lhsh);
      fRHSHints.push_back(rhsh);

      fRow.push_back(new ParameterSetEditRow(this, lhs, rhs, pset, keys[i]));
      ++j;
    }
  }

  fCanvas->Connect("ProcessedEvent(Event_t*)",
                   "evdb::ParameterSetEditFrame",
                   this,
                   "HandleMouseWheel(Event_t*)");

  fCanvas->Resize();
}

//......................................................................

ParameterSetEditFrame::~ParameterSetEditFrame()
{
  cet::for_all(fRow, std::default_delete<ParameterSetEditRow>{});
  cet::for_all(fRHSHints, std::default_delete<TGTableLayoutHints>{});
  cet::for_all(fLHSHints, std::default_delete<TGTableLayoutHints>{});
  cet::for_all(fRHS, std::default_delete<TGHorizontalFrame>{});
  cet::for_all(fLHS, std::default_delete<TGHorizontalFrame>{});
  delete fLayout;
  //
  // Parent takes care of delete for fContainer, I think. Anyhow,
  // trying to delete it causes a seg fault.
  //
  // delete fContainer;
  delete fCanvasH;
  delete fCanvas;
}

//......................................................................
void
ParameterSetEditFrame::HandleMouseWheel(Event_t* event)
{
  // Handle mouse wheel to scroll.
  if (event->fType != kButtonPress && event->fType != kButtonRelease)
    return;

  Int_t page = 0;
  if (event->fCode == kButton4 || event->fCode == kButton5) {
    if (!fCanvas)
      return;
    if (fCanvas->GetContainer()->GetHeight())
      page = Int_t(Float_t(fCanvas->GetViewPort()->GetHeight() *
                           fCanvas->GetViewPort()->GetHeight()) /
                   fCanvas->GetContainer()->GetHeight());
  }

  if (event->fCode == kButton4) {
    // scroll up
    Int_t newpos = fCanvas->GetVsbPosition() - page;
    if (newpos < 0)
      newpos = 0;
    fCanvas->SetVsbPosition(newpos);
  }
  if (event->fCode == kButton5) {
    // scroll down
    Int_t newpos = fCanvas->GetVsbPosition() + page;
    fCanvas->SetVsbPosition(newpos);
  }
}

//......................................................................
void
ParameterSetEditFrame::Modified()
{
  fIsModified = true;
}

//......................................................................
void
ParameterSetEditFrame::Finalize()
{
  for (auto row : fRow) {
    row->Finalize();
  }
}

//......................................................................

std::string
ParameterSetEditFrame::AsFHICL() const
{
  std::ostringstream s;
  for (auto row : fRow) {
    s << row->AsFHICL() << "\n";
  }
  return s.str();
}

//======================================================================
//
// ParameterSetEditDialog methods
//
ParameterSetEditDialog::ParameterSetEditDialog(unsigned int psetid)
  : TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 4, 4)
{
  fTGTab = new TGTab(this);
  this->AddFrame(fTGTab);

  fButtons = new TGHorizontalFrame(this);
  this->AddFrame(fButtons);

  fApply = new TGTextButton(fButtons, " Apply  ");
  fCancel = new TGTextButton(fButtons, " Cancel ");
  fDone = new TGTextButton(fButtons, " Done   ");

  fButtons->AddFrame(fApply);
  fButtons->AddFrame(fCancel);
  fButtons->AddFrame(fDone);

  fApply->Connect("Clicked()", "evdb::ParameterSetEditDialog", this, "Apply()");
  fCancel->Connect(
    "Clicked()", "evdb::ParameterSetEditDialog", this, "Cancel()");
  fDone->Connect("Clicked()", "evdb::ParameterSetEditDialog", this, "Done()");

  // Loop over all the parameter sets and build tabs for them
  const ServiceTable& st = ServiceTable::Instance();
  assert(psetid < st.fServices.size());
  int which = st.fServices[psetid].fCategory;

  unsigned int i;
  unsigned int top = 0, indx = 0;
  for (i = 0; i < st.fServices.size(); ++i) {
    if (st.fServices[i].fCategory == which) {
      if (i == psetid)
        top = indx;
      std::string tabnm = this->TabName(st.fServices[i].fName);
      TGCompositeFrame* f = fTGTab->AddTab(tabnm.c_str());
      fFrames.push_back(new ParameterSetEditFrame(f, i));
      ++indx;
    }
  }
  fTGTab->SetTab(top);

  switch (which) {
    case kDRAWING_SERVICE:
      this->SetWindowName("Drawing Services");
      break;
    case kEXPERIMENT_SERVICE:
      this->SetWindowName("Experiment Services");
      break;
    default:
      this->SetWindowName("Services Configuration");
  }

  this->MapSubwindows();
  this->Resize(kWidth, kHeight);
  this->MapWindow();
}

//......................................................................

ParameterSetEditDialog::~ParameterSetEditDialog()
{
  unsigned int i;
  for (i = 0; i < fFrames.size(); ++i)
    delete fFrames[i];
  delete fDone;
  delete fCancel;
  delete fApply;
  delete fButtons;
  delete fTGTab;
}

//......................................................................

void
ParameterSetEditDialog::Apply()
{
  // We're not in control of the event loop so what we can do is write
  // the new configuration to the ServiceTable. The main driver will
  // pick it up, apply it, and wipe it clean when a reload / next
  // event is triggered.

  ServiceTable& st = ServiceTable::Instance();
  for (auto frame : fFrames) {
    if (frame->fIsModified) {
      unsigned int psetid = frame->fParameterSetID;

      frame->Finalize();
      std::string p = frame->AsFHICL();

      p += "service_type:";
      p += st.fServices[psetid].fName;

      st.fServices[psetid].fParamSet = p;
    }
  }
  NavState::Set(kRELOAD_EVENT);
}

//......................................................................

void
ParameterSetEditDialog::Cancel()
{
  this->SendCloseMessage();
}

//......................................................................

void
ParameterSetEditDialog::Done()
{
  this->Apply();
  this->SendCloseMessage();
}

//......................................................................

void
ParameterSetEditDialog::CloseWindow()
{
  delete this;
}

//......................................................................
//
// Remove any redundant text from the tab name
//
std::string
ParameterSetEditDialog::TabName(const std::string& s)
{
  std::size_t const n = s.find("DrawingOptions");
  return s.substr(0, n);
}

////////////////////////////////////////////////////////////////////////
