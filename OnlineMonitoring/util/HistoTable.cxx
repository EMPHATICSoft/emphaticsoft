///
/// \file    HistoTable.cxx
/// \brief   Hold and deliver histogram data
/// \version $Id: HistoTable.cxx,v 1.14 2012-11-13 22:06:22 messier Exp $
/// \author  messier@indiana.edu, mbaird42@fnal.gov
///
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include "OnlineMonitoring/util/RegexMatch.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/util/TickerSubscriber.h"
using namespace emph::onmon;

HistoTable::HistoTable() { }

//......................................................................

HistoTable& HistoTable::Instance(const char* f, Detector_t d)
{
  static HistoTable* jamesbrown = 0;
  if (jamesbrown == 0) jamesbrown = new HistoTable();
  if (f != 0) jamesbrown->ReadFile(f, d);
  return *jamesbrown;
}

//......................................................................

void HistoTable::Unquote(std::string& s)
{
  //
  // Check if leading and trailing characters are "'s
  //
  if ( (*s.begin())=='"' && (*s.rbegin()=='"') ) {
    std::string ss;
    ss = s.substr(1,s.length()-2);
    s = ss;
  }
}

//......................................................................

char HistoTable::NewLineChar(const char* f)
{
  //
  // Read a line assuming linux format ('\n' = new line)
  //
  std::string linux_line;
  {
    std::ifstream file(f);
    if (file.good()) {
      getline(file, linux_line, '\n');
    }
  }

  //
  // Read a line assuming windows format ('\r' = new line)
  //
  std::string windows_line;
  {
    std::ifstream file(f);
    if (file.good()) {
      getline(file, windows_line, '\r');
    }
  }

  if (windows_line.length() < linux_line.length()) return '\r';
  return '\n';
}

//......................................................................

void HistoTable::ReadFile(const char* f, Detector_t det)
{
  fTable.clear();

  //
  // See if we can detect if this file is windows or linux formatted
  //
  char newline = this->NewLineChar(f);

  std::string name;
  std::string title;
  std::string category;
  std::string type;
  std::string nx;
  std::string x1;
  std::string x2;
  std::string ny;
  std::string y1;
  std::string y2;
  std::string option;
  std::string detector;
  std::string reset;
  std::string lookback;
  std::string caption;

  int i;
  int lineno = 0;
  std::ifstream file(f);
  bool haveheader = false;
  while (file.good()) {
    getline(file, name,     ',');
    getline(file, title,    ',');
    getline(file, category, ',');
    getline(file, type,     ',');
    getline(file, nx,       ',');
    getline(file, x1,       ',');
    getline(file, x2,       ',');
    getline(file, ny,       ',');
    getline(file, y1,       ',');
    getline(file, y2,       ',');
    getline(file, option,   ',');
    getline(file, detector, ',');
    getline(file, reset,    ',');
    getline(file, lookback, ',');
    getline(file, caption,  newline);
    ++lineno;

    //
    // Some editors put all text into quotes in .csv files. Undo that
    // if its happened
    //
    this->Unquote(name);
    this->Unquote(title);
    this->Unquote(category);
    this->Unquote(type);
    this->Unquote(nx);
    this->Unquote(x1);
    this->Unquote(x2);
    this->Unquote(ny);
    this->Unquote(y1);
    this->Unquote(y2);
    this->Unquote(option);
    this->Unquote(detector);
    this->Unquote(reset);
    this->Unquote(lookback);
    this->Unquote(caption);

    // New on EMPHATIC, seeing issues with name containing the newline character. Remove it
    name.erase(std::remove_if(name.begin(),name.end(), ::isspace),name.end());

    // line for debugging .csv file
    //std::cout << "\n" << lineno << "\t" << name << "\t" << title  << "\t" << category << std::endl;

    //
    // Check if this line is the column labels across the top
    //
    if (name     == "Name"      &&
	title    == "Title"     &&
	category == "Category"  &&
	type     == "Type"      &&
	nx       == "nx"        &&
	x1       == "x1"        &&
	x2       == "x2"        &&
	ny       == "ny"        &&
	y1       == "y1"        &&
	y2       == "y2"        &&
	option   == "Option"    &&
	detector == "Detector"  &&
	reset    == "Reset"     &&
	lookback == "Look back" &&
	caption  == "Caption") {
      if (haveheader==false) {
	haveheader = true;
	continue;
      }
    }

    //
    // Contruct the data object
    //
    HistoData h;
    h.fName    = name;
    h.fTitle   = title;
    h.fCaption = caption;

    h.fNx = atoi(nx.c_str());
    h.fX1 = atof(x1.c_str());
    h.fX2 = atof(x2.c_str());

    h.fNy = atoi(ny.c_str());
    h.fY1 = atof(y1.c_str());
    h.fY2 = atof(y2.c_str());

    h.fDetector = kUnknownDet;
    const char* detstr[] = {"all", "emph",0};
    Detector_t  detind[] = {kALLDET,kEMPH};
    for (i=0; detstr[i]!=0; ++i) {
      if (detector==detstr[i]) { h.fDetector = detind[i]; break; }
    }

    h.fType = kUnknownType;
    const char* typestr[] = {"TH1F","TH2F",0};
    Histo_t     typeind[] = {kTH1F, kTH2F};
    for (i=0; typestr[i]!=0; ++i) {
      if (type==typestr[i]) { h.fType = typeind[i]; break; }
    }

    const char* rstr[] = {
      "run",
      "subrun",
      "30sec",
      "1min",
      "5min",
      "10min",
      "30min",
      "hour",
      "24hr",
      "utc",
      0
    };
    unsigned int rind[] = {
      TickerSubscriber::kRun,
      TickerSubscriber::kSubrun,
      TickerSubscriber::k30sec,
      TickerSubscriber::k1min,
      TickerSubscriber::k5min,
      TickerSubscriber::k10min,
      TickerSubscriber::k30min,
      TickerSubscriber::kHour,
      TickerSubscriber::k24hr,
      TickerSubscriber::kUTC
    };
    for (i=0; rstr[i]!=0; ++i) {
      if (reset==rstr[i]) { h.fReset = rind[i]; break; }
    }
    h.fLookBack = atoi(lookback.c_str());

    //
    // If the histogram name is a wildcard, we want to create a
    // sub-category to hold all the possible matches.
    //
    bool iswildcard = (name.find('*')!=name.npos);
    std::string suffix;
    if (iswildcard) {
      suffix = "/";
      suffix += name;
    }

    //
    // Parse out the category field
    //
    std::stringstream cat(category);
    std::string c;
    while (cat.good()) {
      getline(cat, c, ':');
      c += suffix;
      h.fCategory.push_back(c);
    }

    std::stringstream opt(option);
    std::string o;
    while (opt.good()) {
      getline(opt, o, ':');
      h.fOption.push_back(o);
    }

    //
    // Check that the entry makes sense before inserting into table
    //
    if (!h.IsValid()) {
      std::cerr << __FILE__ << ":" << __LINE__ <<
	" Bad entry at line " << lineno << " in file " << f << std::endl;
      abort();
    }

    //
    // Insert it into the table if the detector context matches and we
    // do not already have an entry for this histogram
    //
    if (h.fDetector==kALLDET || h.fDetector==det) {
      if (fTable.find(name)==fTable.end()) {
	fTable[name] = h;
      }
      else {
	std::cerr << "Duplicate entries for histogram "
		  << name << std::endl;
	abort();
      }
    }
  }
  if (haveheader==false) {
    std::cout << __FILE__ << ":" << __LINE__
	      << " Bad csv file read " << f
	      << ". Header has incorrect format."
	      << std::endl;
    abort();
  }
}

//......................................................................

const HistoData* HistoTable::LookUp(const char* nm) const
{
  //
  // First, look for an exact match
  //
  std::map<std::string,HistoData>::const_iterator itr(fTable.find(nm));
  if (itr!=fTable.end()) return (&itr->second);

  //
  // If we didn't find an exact match, perhaps there is a wildcard
  // expression that matches?
  //
  unsigned int     nmatch = 0;
  const HistoData* hd     = 0;
  std::map<std::string,HistoData>::const_iterator i(fTable.begin());
  std::map<std::string,HistoData>::const_iterator e(fTable.end());
  for (; i!=e; ++i) {
    if (regex_match(nm, i->first)==0) {
      hd = (&i->second);
      ++nmatch;
    }
  }
  //
  // Success is finding one unique match
  //
  if (nmatch==1) return hd;

  //
  // At this point, we have either zero matches or more than one
  //
  if (nmatch>1) {
    std::cerr << "Match to histogram " << nm << " is not unique, nmatch="
	      << nmatch
	      << std::endl;
    abort();
  }
  return hd;
}

//......................................................................

const HistoData& HistoTable::operator[](const char* nm) const
{
  const HistoData* d = this->LookUp(nm);
  if (d==0) {
    std::cerr << __FILE__ << " " << __LINE__ << ": "
	      << "Unable to find " << nm << std::endl;
    abort();
  }
  return *d;
}
////////////////////////////////////////////////////////////////////////
