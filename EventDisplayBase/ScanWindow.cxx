///
/// \file    ScanWindow.cxx
/// \brief   window for hand scanning
/// \author  brebel@fnal.gov
/// \version $Id: ScanWindow.cxx,v 1.19 2012-09-24 15:19:47 brebel Exp $
///
#include "TCanvas.h"
#include "TGFrame.h"  // For TGMainFrame, TGHorizontalFrame
#include "TGLayout.h" // For TGLayoutHints
#include "TGButton.h" // For TGCheckButton
#include "TGDimension.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TGScrollBar.h"
#include "TGCanvas.h"
#include "TMath.h"
#include "TString.h"
#include "TSystem.h"
#include "TTimeStamp.h"

#include "EventDisplayBase/ScanWindow.h"
#include "EventDisplayBase/ScanOptions.h"
#include "EventDisplayBase/NavState.h"
#include "EventDisplayBase/EventHolder.h"
//#include "SimulationBase/MCTruth.h"
//#include "nusimdata/SimulationBase/MCNeutrino.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

static int kInputID = 0;

namespace evdb{

	//......................................................................
	ScanFrame::ScanFrame(TGCompositeFrame* f)
	{
		art::ServiceHandle<evdb::ScanOptions> opts;

		unsigned int nCategories = opts->fCategories.size();

		fFrame = new TGGroupFrame(f, "Please complete these fields", kVerticalFrame);
		fFrameHints = new TGLayoutHints(kLHintsExpandX|
						kLHintsExpandY,
						4,4,4,4);
		f->AddFrame(fFrame, fFrameHints);

		//
		// grab the ScanOptions service and loop over the categories to make a
		// ScanFrame for each
		//
		fCatFrameLH = new TGLayoutHints(kLHintsLeft|
						kLHintsExpandX|
						kLHintsTop,
						2,2,2,2);
		unsigned int pos = 0;
		for(unsigned int c = 0; c < nCategories; ++c){
			std::vector<std::string> types;
			std::vector<std::string> labels;

			for(unsigned int p = 0; p < opts->fFieldsPerCategory[c]; ++p){
	types. push_back(opts->fFieldTypes[pos+p]);
	labels.push_back(opts->fFieldLabels[pos+p]);
			}

			pos += opts->fFieldsPerCategory[c];

			//
			// Create container for the current category.
			//
			TGGroupFrame *catframe = 0;
			catframe = new TGGroupFrame(fFrame,
					opts->fCategories[c].c_str(),
					kRaisedFrame|kVerticalFrame);
			fCatFrames.push_back(catframe);
			fFrame->AddFrame(catframe,fCatFrameLH);

			fFieldFrameHints = new TGLayoutHints(kLHintsExpandX,2,2,2,2);

			// loop over the fields and determine what to draw
			for(unsigned int i = 0; i < types.size(); ++i){
	TGHorizontalFrame* fieldframe = new TGHorizontalFrame(catframe);
	fFieldFrames.push_back(fieldframe);
	catframe->AddFrame(fieldframe,fFieldFrameHints);

	if(types[i] == "Text") {
		TGLabel *l = new TGLabel(fieldframe, labels[i].c_str());
		fieldframe->AddFrame(l);
		fTextBoxes.push_back(new TGTextEntry(fieldframe));
		fieldframe->AddFrame(fTextBoxes[fTextBoxes.size()-1]);
	}

	if(types[i] == "Number"){
		TGLabel *l = new TGLabel(fieldframe, labels[i].c_str());
		fieldframe->AddFrame(l);
		TGNumberEntry*
			ne = new TGNumberEntry(fieldframe,
					 0,
					 2,
					 -1,
					 TGNumberFormat::kNESInteger);
		fieldframe->AddFrame(ne);

		fNumberLabels.push_back(l);
		fNumberBoxes.push_back(ne);
	}

	if(types[i] =="CheckButton"){
		TGCheckButton* cb = new TGCheckButton(fieldframe,
						labels[i].c_str(),
						kInputID);
		fieldframe->AddFrame(cb);
		fCheckButtons.push_back(cb);
	}

	if(types[i] =="RadioButton"){
		TGRadioButton* rb = new TGRadioButton(fieldframe,
						labels[i].c_str(),
						kInputID);
		fieldframe->AddFrame(rb);
		rb->Connect("Clicked()",
					"evdb::ScanFrame",
					this,
					"RadioButton()");

		fRadioButtons.push_back(rb);
		fRadioButtonIds.push_back(kInputID);
	}

	++kInputID;

			}// end loop over types
		} // end loop over categories

		fFrame->Connect("ProcessedEvent(Event_t*)",
				"evdb::ScanFrame",
				this,
				"HandleMouseWheel(Event_t*)");
	}

	//......................................................................
	ScanFrame::~ScanFrame()
	{
		unsigned int i;

		for(i=0; i<fCheckButtons.size(); ++i){
			if( fCheckButtons[i] ) delete fCheckButtons[i];
		}
		for(i=0; i<fRadioButtons.size(); ++i){
			if( fRadioButtons[i] ) delete fRadioButtons[i];
		}
		for (i=0; i<fNumberLabels.size(); ++i) {
			if (fNumberLabels[i]) delete fNumberLabels[i];
		}
		for(i=0; i<fNumberBoxes.size(); ++i){
			if( fNumberBoxes[i] ) delete fNumberBoxes[i];
		}
		for(i=0; i<fTextBoxes.size(); ++i){
			if( fTextBoxes[i] ) delete fTextBoxes[i];
		}
		for(i=0; i<fFieldFrames.size(); ++i){
			if( fFieldFrames[i] ) delete fFieldFrames[i];
		}
		for(i=0; i<fCatFrames.size(); ++i){
			if( fCatFrames[i] ) delete fCatFrames[i];
		}
		delete fCatFrameLH;
		delete fFieldFrameHints;
		delete fFrameHints;
		delete fFrame;
	}

	//......................................................................
	int ScanFrame::GetHeight() const
	{
		if (fFrame) return fFrame->GetHeight();
		else        return 0;
	}

	//......................................................................
	int ScanFrame::GetWidth() const
	{
		if (fFrame) return fFrame->GetWidth();
		else        return 0;
	}

	//......................................................................

	void ScanFrame::HandleMouseWheel(Event_t *event)
	{
		// Handle mouse wheel to scroll.

		if (event->fType != kButtonPress && event->fType != kButtonRelease)
			return;

		Int_t page = 0;
		if (event->fCode == kButton4 || event->fCode == kButton5) {
			if (fCanvas==0) return;
			if (fCanvas->GetContainer()->GetHeight())

	page = Int_t(Float_t(fCanvas->GetViewPort()->GetHeight() *
					 fCanvas->GetViewPort()->GetHeight()) /
				 fCanvas->GetContainer()->GetHeight());
		}

		if (event->fCode == kButton4) {
			//scroll up
			Int_t newpos = fCanvas->GetVsbPosition() - page;
			if (newpos < 0) newpos = 0;
			fCanvas->SetVsbPosition(newpos);
		}
		if (event->fCode == kButton5) {
			// scroll down
			Int_t newpos = fCanvas->GetVsbPosition() + page;
			fCanvas->SetVsbPosition(newpos);
		}
	}

	//......................................................................
	void ScanFrame::ClearFields()
	{
		art::ServiceHandle<evdb::ScanOptions> scanopt;

		unsigned int txtctr = 0;
		unsigned int numctr = 0;
		unsigned int radctr = 0;
		unsigned int chkctr = 0;
		for(unsigned int t = 0; t < scanopt->fFieldTypes.size(); ++t){

			if(scanopt->fFieldTypes[t] == "Text"){
	if(txtctr < fTextBoxes.size()   ){
		fTextBoxes[txtctr]->Clear();
	}
	++txtctr;
			}
			else if(scanopt->fFieldTypes[t] == "Number"){
	if(numctr < fNumberBoxes.size() ){
		fNumberBoxes[numctr]->SetNumber(0);
	}
	++numctr;
			}
			else if(scanopt->fFieldTypes[t] == "RadioButton"){
	if(radctr < fRadioButtons.size()     ){
		fRadioButtons[radctr]->SetState(kButtonUp);
	}
	++radctr;
			}
			else if(scanopt->fFieldTypes[t] == "CheckButton"){
	if(chkctr < fCheckButtons.size()     ){
		fCheckButtons[chkctr]->SetState(kButtonUp);
	}
	++chkctr;
			}

		}// end loop over input field types

	}

	//......................................................................
	void ScanFrame::Record(std::string outfilename,
			 const char* comments)
	{
		art::ServiceHandle<evdb::ScanOptions> scanopt;

		// get the event information
		const art::Event *evt = evdb::EventHolder::Instance()->GetEvent();

		std::ofstream outfile(outfilename.c_str(), std::ios_base::app);

		outfile << evt->run() << " " << evt->subRun() << " " << evt->id().event() << " ";

		// loop over the input fields
		unsigned int txtctr = 0;
		unsigned int numctr = 0;
		unsigned int radctr = 0;
		unsigned int chkctr = 0;
		for(unsigned int t = 0; t < scanopt->fFieldTypes.size(); ++t){

			if(scanopt->fFieldTypes[t] == "Text"){
				if(txtctr < fTextBoxes.size()   ){
					outfile << fTextBoxes[txtctr]->GetText() << " ";
					fTextBoxes[txtctr]->Clear();
				}
			++txtctr;
			}
			else if(scanopt->fFieldTypes[t] == "Number"){
				if(numctr < fNumberBoxes.size() ){
					outfile << fNumberBoxes[numctr]->GetNumber() << " ";
					fNumberBoxes[numctr]->SetNumber(0);
				}
				++numctr;
			}
			else if(scanopt->fFieldTypes[t] == "RadioButton"){
				if(radctr < fRadioButtons.size()     ){
					outfile << (fRadioButtons[radctr]->GetState() == kButtonDown) << " ";
					fRadioButtons[radctr]->SetState(kButtonUp);
				}
				++radctr;
			}
			else if(scanopt->fFieldTypes[t] == "CheckButton"){
				if(chkctr < fCheckButtons.size()     ){
					outfile << (fCheckButtons[chkctr]->GetState() == kButtonDown) << " ";
					fCheckButtons[chkctr]->SetState(kButtonUp);
				}
				++chkctr;
			}
		}// end loop over input field types

		// do we need to get the truth information?
/*
		if(scanopt->fIncludeMCInfo){

			try {
				auto const mclist = evt->getMany<std::vector<simb::MCTruth>>();

				bool listok = (mclist.size()>0);
				bool isnu   = false;
				if ( listok && isnu==false) {
					mf::LogWarning("ScanWindow")
					<< "Unknown particle source or truth information N/A"
					<< " put garbage numbers into the file";
					outfile
					<< -999. << " " << -999. << " " << -999. << " "
					<< -999. << " " << -999. << " " << -999.<< " " << -999.;
				}
*/
/*
	if (listok && isnu) {
		// get the event vertex and energy information,
		const simb::MCNeutrino& nu = mclist[0]->at(0).GetNeutrino();

		outfile << nu.Nu().PdgCode() << " "
			<< nu.Nu().Vx()      << " "
			<< nu.Nu().Vy()      << " "
			<< nu.Nu().Vz()      << " "
			<< nu.Nu().E()       << " "
			<< nu.CCNC()         << " "
			<< nu.Lepton().E()   << " "
			<< nu.InteractionType();
	}
	*/
/*			}
			catch(cet::exception &e){
				mf::LogWarning("ScanWindow")
				<< "MC truth information requested for output file"
				<< " but no MCTruth objects found in event - "
				<< " put garbage numbers into the file";
				outfile
				<< -999. << " " << -999. << " " << -999. << " "
				<< -999. << " " << -999. << " " << -999.;
			}
		}//end if using MC information
*/
		// end this line for the event
		outfile << " " << comments << std::endl;
	}

	//......................................................................

	void ScanFrame::RadioButton()
	{
		TGButton *b = (TGButton *)gTQSender;
		int id = b->WidgetId();

		if(fRadioButtonIds.size() < 2) return;

		if(id >= fRadioButtonIds[0] && id <= fRadioButtonIds[fRadioButtonIds.size()-1]){
			for(unsigned int i = 0; i < fRadioButtonIds.size(); ++i)
	if(fRadioButtons[i]->WidgetId() != id) fRadioButtons[i]->SetState(kButtonUp);
		}

		return;
	}

	//--------------------------------------------------------------------

	void ScanWindow::BuildButtonBar(TGHorizontalFrame* f)
	{
		fCommentLabel = new TGLabel     (f, " Comments:");
		fCommentEntry = new TGTextEntry (f);
		fPrevButton   = new TGTextButton(f, " <<Prev ");
		fNextButton   = new TGTextButton(f, " Next>> ");
		fRcrdButton   = new TGTextButton(f, " Record ");

		fPrevButton->Connect("Clicked()", "evdb::ScanWindow", this, "Prev()");
		fNextButton->Connect("Clicked()", "evdb::ScanWindow", this, "Next()");
		fRcrdButton->Connect("Clicked()", "evdb::ScanWindow", this, "Rec()");

		Pixel_t c;
		gClient->GetColorByName("pink", c);
		fRcrdButton->ChangeBackground(c);

		fButtonBarHintsL = new TGLayoutHints(kLHintsBottom|kLHintsLeft,
					 4,2,2,8);
		fButtonBarHintsC = new TGLayoutHints(kLHintsBottom|kLHintsLeft,
					 2,2,2,8);
		fButtonBarHintsR = new TGLayoutHints(kLHintsBottom|kLHintsLeft,
					 2,4,2,8);
		f->AddFrame(fCommentLabel, fButtonBarHintsL);
		f->AddFrame(fCommentEntry, fButtonBarHintsC);
		f->AddFrame(fPrevButton,   fButtonBarHintsC);
		f->AddFrame(fNextButton,   fButtonBarHintsC);
		f->AddFrame(fRcrdButton,   fButtonBarHintsR);
	}

	//--------------------------------------------------------------------

	void ScanWindow::BuildUserFields(TGCompositeFrame* f)
	{
		unsigned int kCanvasWidth  = 390;
		unsigned int kCanvasHeight = 500;

		fUserFieldsCanvas = new
			TGCanvas(f, kCanvasWidth, kCanvasHeight);
		TGLayoutHints*
			fUserFieldsCanvasHints = new TGLayoutHints(kLHintsExpandX|
						 kLHintsExpandY);
		f->AddFrame(fUserFieldsCanvas, fUserFieldsCanvasHints);

		fScanFrame = new ScanFrame(fUserFieldsCanvas->GetViewPort());
		fUserFieldsCanvas->SetContainer(fScanFrame->GetFrame());
		fScanFrame->GetFrame()->SetCleanup(kDeepCleanup);
	}


	//--------------------------------------------------------------------
	ScanWindow::ScanWindow() :
		TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 50, 50),
		fUserFieldsCanvas(0),
		fUserFieldsFrame(0),
		fUserFieldsHints(0),
		fButtonBar(0),
		fButtonBarHints(0),
		fCommentLabel(0),
		fCommentEntry(0),
		fPrevButton(0),
		fNextButton(0),
		fRcrdButton(0),
		fButtonBarHintsL(0),
		fButtonBarHintsC(0),
		fButtonBarHintsR(0),
		fScanFrame(0)
	{
		//
		// Create a frame to hold the user-configurabale fields
		//
		unsigned int kWidth  = 5*50;
		unsigned int kHeight = 7*50;
		fUserFieldsFrame =
			new TGCompositeFrame(this, kWidth, kHeight);
		fUserFieldsHints =
			new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX|kLHintsExpandY);
		this->AddFrame(fUserFieldsFrame, fUserFieldsHints);

		//
		// Create a frame to hold the button bar at the bottom
		//
		unsigned int kButtonBarWidth  = 388;
		unsigned int kButtonBarHeight = 30;
		fButtonBar =
			new TGHorizontalFrame(this, kButtonBarHeight, kButtonBarWidth);
		fButtonBarHints =
			new TGLayoutHints(kLHintsBottom|kLHintsLeft);
		this->AddFrame(fButtonBar, fButtonBarHints);

		this->BuildButtonBar (fButtonBar);
		this->BuildUserFields(fUserFieldsFrame);
		this->OpenOutputFile();

		//
		// Finalize the window for display
		//
		this->Resize(kButtonBarWidth,kHeight+kButtonBarHeight);
		this->MapSubwindows();
		this->MapWindow();
		this->SetWindowName("Scan dialog window");
	}

	//--------------------------------------------------------------------

	void ScanWindow::OpenOutputFile()
	{
		// set up the file name to store the information
		art::ServiceHandle<evdb::ScanOptions> opts;
		std::string user(gSystem->Getenv("USER"));
		user.append("_");
		TTimeStamp cur;
		std::string time(cur.AsString("s"));
		time.replace(time.find(" "), 1, "_");
		fOutFileName.append(opts->fScanFileBase);
		fOutFileName.append(user);
		fOutFileName.append(time);
		fOutFileName.append(".txt");

		std::ofstream outfile(fOutFileName.c_str());

		//output the labels so we know what each is
		outfile << "Run Subrun Event ";

		//
		// figure out how many categories and maximum number of items for
		// a category
		//
		unsigned int maxFields = 1;
		unsigned int pos       = 0;
		for(unsigned int c = 0; c < opts->fCategories.size(); ++c){
			for(unsigned int p = 0; p < opts->fFieldsPerCategory[c]; ++p){
	if(opts->fFieldsPerCategory[c] > maxFields) {
		maxFields = opts->fFieldsPerCategory[c];
	}
	outfile << opts->fCategories[c].c_str() << ":"
		<< opts->fFieldLabels[pos+p].c_str() << " ";
			}
			pos += opts->fFieldsPerCategory[c];
		} // end loop over categories

		if(opts->fIncludeMCInfo)
			outfile << "Truth:PDG Vtx_x Vtx_y Vtx_Z "
				<< "Nu_E CCNC Lepton_E InteractionType ";

		outfile << "comments" << std::endl;
		}



	//......................................................................
	ScanWindow::~ScanWindow()
	{
		delete fScanFrame;
		delete fButtonBarHintsR;
		delete fButtonBarHintsC;
		delete fButtonBarHintsL;
		delete fRcrdButton;
		delete fNextButton;
		delete fPrevButton;
		delete fCommentEntry;
		delete fCommentLabel;
		delete fButtonBarHints;
		delete fButtonBar;
		delete fUserFieldsHints;
		delete fUserFieldsFrame;
		delete fUserFieldsCanvas;
	}

	//......................................................................
	void ScanWindow::CloseWindow() { delete this; }

	//......................................................................
	void ScanWindow::Prev()
	{
		fScanFrame->ClearFields();
		evdb::NavState::Set(kPREV_EVENT);
	}

	//......................................................................
	void ScanWindow::Next()
	{
		fScanFrame->ClearFields();
		evdb::NavState::Set(kNEXT_EVENT);
	}

	//......................................................................
	void ScanWindow::Rec()
	{
		fScanFrame->Record(fOutFileName, fCommentEntry->GetText());
		fCommentEntry->SetText("");
		evdb::NavState::Set(evdb::kNEXT_EVENT);
	}

}// namespace
