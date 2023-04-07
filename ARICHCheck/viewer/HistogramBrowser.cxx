#include "OnlineMonitoring/viewer/HistogramBrowser.h"
#include <list>
#include <string>
#include <iostream>
#include "TGPicture.h"
#include "TGListTree.h"
#include "TGButton.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/viewer/HistoSource.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
using namespace emph::onmon;

HistogramBrowser::HistogramBrowser(const TGWindow* p,
				   unsigned int w,
				   unsigned int h,
				   unsigned int opt) :
  TGCompositeFrame(p, w, h, opt),
  GUIModelSubscriber(kHistogramSourceID|kCurrentHistogramID),
  fCanvas(0),
  fListTree(0)
{
  fCanvas = new TGCanvas(this, w, h);
  TGLayoutHints* hints = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,
					   2, 2, 2, 2);
  fListTree = new TGListTree(fCanvas, kHorizontalFrame);
  this->AddFrame(fCanvas, hints);

  fListTree->Connect("DoubleClicked(TGListTreeItem*,Int_t)",
		     "emph::onmon::HistogramBrowser",
		     this,
		     "DoubleClicked(TGListTreeItem*,Int_t)");

  TGLayoutHints* xx = new TGLayoutHints(kLHintsExpandX);
  fRefresh = new TGTextButton(this, "Refresh Histogram List");
  fRefresh->SetToolTipText("Request the current histogram list from the histogram source and refresh the browser.");
  fRefresh->Connect("Clicked()",
		    "emph::onmon::HistogramBrowser",
		    this,
		    "HandleRefresh()");
  this->AddFrame(fRefresh, xx);
}

//......................................................................

//
// Populate the browser tree with histograms from the plotter source
//
void HistogramBrowser::Populate()
{
  HistoSource hs(GUIModel::Instance().Data().fHistogramSource.c_str());
  const HistoTable& ht = HistoTable::Instance();

  //
  // Pull out all the histograms from the plotter
  //
  std::list<std::string> obj;
  hs.FindAllMatches("*","*_prev_*",obj);
  obj.sort();

  //
  // Loop over histograms and place them in the correct categories
  //
  const TGPicture* pic1d = gClient->GetPicture("h1_t.xpm");
  const TGPicture* pic2d = gClient->GetPicture("h2_t.xpm");
  const TGPicture* pic = 0;
  TGListTreeItem* h;
  std::list<std::string>::const_iterator itr(obj.begin());
  std::list<std::string>::const_iterator end(obj.end());
  for (; itr!=end; ++itr) {

    // Skip the header object (for now...)
    if((*itr) == "Header") continue;

    const HistoData* hd = ht.LookUp(itr->c_str());
    if (hd==0) {
      std::cerr << " Histogram " << (*itr) << " not found." << std::endl;
      continue;
    }
    if (hd->fType==kTH1F) pic = pic1d;
    if (hd->fType==kTH2F) pic = pic2d;
    if (pic==0)           pic = pic1d;
    for (unsigned int i=0; i<hd->fCategory.size(); ++i) {
      TGListTreeItem* p = this->FindCategory(hd->fCategory[i].c_str());
      h = fListTree->AddItem(p, itr->c_str(), pic, pic);
      if (h) {
	h->SetTipText(hd->fCaption.c_str());
	fHistograms[*itr] = h;
      }
    }
  }
}

//......................................................................

void HistogramBrowser::BuildTree()
{
  this->ClearBrowser();
  this->MakeCategories();
  this->Populate();
}

//......................................................................

void HistogramBrowser::ClearBrowser()
{
  if (fListTree==0) return;

  //
  // How to completely delete the browser tree? Try traversing all the
  // top items and deleting all their children. Then delete all the
  // top level items
  //
  std::vector<TGListTreeItem*> item;
  TGListTreeItem* i1 = fListTree->GetFirstItem();
  if (i1!=0) {
    for (; i1!=0; i1=i1->GetNextSibling()) item.push_back(i1);
    for (unsigned int i=0; i<item.size(); ++i) {
      fListTree->DeleteItem(item[i]);
    }
  }
  fCategories.clear();
  fHistograms.clear();
}

//......................................................................

//
// Make all the top-level categories
//
void HistogramBrowser::MakeCategories()
{
  //
  // Load the table of histograms so we can extract the list of
  // histogram categories
  //
  const GUIModelData& model = GUIModel::Instance().Data();
  HistoTable& ht = HistoTable::Instance(model.fHistoCSVFile.c_str(),
					model.fDetector);

  std::map<std::string,HistoData>::iterator itr   (ht.fTable.begin());
  std::map<std::string,HistoData>::iterator itrEnd(ht.fTable.end());
  std::list<std::string> cats;
  for (; itr!=itrEnd; ++itr) {
    const HistoData& hd = itr->second;
    for (unsigned int i=0; i<hd.fCategory.size(); ++i) {
      cats.push_back(hd.fCategory[i]);
    }
  }

  //
  // Sort the list and make the category. We count on "MakeCategory"
  // to be smart enough to avoid duplicate categories.
  //
  cats.sort();
  std::list<std::string>::iterator iCat(cats.begin());
  std::list<std::string>::iterator iCatEnd(cats.end());
  for (; iCat!=iCatEnd; ++iCat) {
    this->MakeCategory(iCat->c_str());
  }
}

//......................................................................

TGListTreeItem* HistogramBrowser::FindCategory(const char* nm)
{
  std::map<std::string, TGListTreeItem*>::iterator itr;
  itr = fCategories.find(nm);
  if (itr==fCategories.end()) return 0;
  return itr->second;
}

//......................................................................

TGListTreeItem* HistogramBrowser::MakeCategory(const char* nm)
{
  TGListTreeItem* lti = 0;

  //
  // Test if this category already exists - if it does, we're done
  //
  lti = this->FindCategory(nm);
  if (lti!=0) return lti;

  // Split the category into parent and child pieces.
  std::string n(nm);
  std::string p;
  std::string c;
  TGListTreeItem* parent;

  size_t pos = n.find_last_of("/");
  if (pos<n.length()) {
    //
    // Found a "/". Everything before is parent, everything after is child
    //
    c = n.substr(pos+1, n.length());
    p = n.substr(0, pos);
    parent = this->FindCategory(p.c_str());
    if (parent==0) parent = this->MakeCategory(p.c_str());
  }
  else {
    //
    // No "/" found. Must be a top-level category.
    //
    c = n;
    p = "";
    parent = 0;
  }
  //
  // Add the item to the tree and category list
  //
  lti = fListTree->AddItem(parent, c.c_str());
  fCategories[n] = lti;

  return lti;
}

//......................................................................

void HistogramBrowser::DoubleClicked(TGListTreeItem* item, Int_t i __attribute__((unused)))
{
  //
  // Check if clicked item is an end of a branch. If it is, then it
  // must be a plot object and not a container.
  //
  if (item->GetFirstChild()==0) {
    std::string nm = item->GetText();

    //
    // Wildcarded names are never displayable histograms. Skip those.
    //
    bool iswildcard = (nm.find('*')!=nm.npos);
    if (iswildcard) return;

    GUIModel::Instance().SetCurrentHistogram(nm.c_str());
  }
}

//......................................................................

void HistogramBrowser::HandleRefresh()
{
  this->BuildTree();
}

//......................................................................

void HistogramBrowser::GUIModelDataIssue(const GUIModelData& d,
					 unsigned int which)
{
  if (this->GetLock()==false) return;

  if ( ((which&kCurrentHistogramID)==0) &&
       ((which&kHistogramSourceID) ==0) ) {
    this->ReleaseLock();
    return;
  }

  if (which&kHistogramSourceID) {
    this->BuildTree();
  }

  //
  // When the histogram changes, change the state of the browser to
  // avoid presenting conflicting information to the user
  //
  if ( (which&kCurrentHistogramID) ) {
    //
    // If there is no current histogram, there is nothing to do
    //
    static const std::string empty("");
    if (empty == d.fCurrentHistogram.Current()) {
      this->ReleaseLock();
      return;
    }

    //
    // Try to find the currently selected histogram in the collection
    // of histograms in the browser. If we fail for some reason
    // (shouldn't ever happen), bail.
    //
    std::map<std::string,TGListTreeItem*>::iterator itr;
    itr = fHistograms.find(d.fCurrentHistogram.Current());
    if (itr==fHistograms.end()) {
      this->ReleaseLock();
      return;
    }

    //
    // If the current histogram is the one already selected, there is
    // nothing to do
    //
    if (itr->second == fListTree->GetSelected()) {
      this->ReleaseLock();
      return;
    }

    //
    // Clear whatever is currently highlighted and try to highlight the new
    //
    fListTree->ClearHighlighted();
    if ( (itr->second->IsOpen()==1) && (itr->second->IsActive()==0) ) {
      fListTree->HighlightItem(itr->second);
    }
  }
  this->ReleaseLock();
}

//......................................................................

HistogramBrowser::~HistogramBrowser()
{

  if (fCanvas) {
    delete fCanvas;
    fCanvas = 0;
  }
  if (fListTree) {
    delete fListTree;
    fListTree = 0;
  }
  if (fRefresh) {
    delete fRefresh;
    fRefresh = 0;
  }

}

////////////////////////////////////////////////////////////////////////
