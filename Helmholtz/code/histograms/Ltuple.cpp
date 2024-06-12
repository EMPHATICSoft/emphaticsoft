/*  L++ Ltuple
 *  Created by Leo Bellantoni on 12/1/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  Light, fast, machine dependent ntuple.
 *  Constructors can throw runtime_error
 *  N.B. Linux has a filesize limit of 2^31 - 1 = 2Gbyte (?)
 *
 */
#include "Ltuple.h"
using namespace std;





// Basic 4
Ltuple::Ltuple() : fp_(nullptr),filename_(nullptr),exists_(false) {};
Ltuple::Ltuple(Ltuple const& inTuple)
    : fp_(nullptr),filename_(nullptr) {
	*this = inTuple;
}
Ltuple& Ltuple::operator= (Ltuple const& rhs) {
	// Assignment constructor - will leave pointers to same file in two places
	if (this != &rhs) {
        if (rhs.fp_) {
            fp_ = rhs.fp_;
            LppUrk.LppIssue(22,"Ltuple::operator= (Ltuple const&)");
        }
        if (rhs.filename_) {
            filename_ = (char*) malloc(strlen(rhs.filename_)+1);
            strcpy(filename_,rhs.filename_);
        }
        exists_ = rhs.exists_;
    }
	return *this;
}
Ltuple::~Ltuple() {
	// Destructor closes the associated file - throw not!
	if (EOF == fclose(fp_)) {
		cout << filename_ << " had trouble closing. [1]" << endl;
	}
	// Frees up the filename space too.
	free(filename_);
	return;
}





// User interface constructor
Ltuple::Ltuple(char const* filename, bool exists) {
	// Main constructor opens the associated file.  Mostly use C rather
	// than C++ constructs, although speed gain is probably pretty small.
	// Constructor opens file in appropriate mode and reads or writes a
	// test structure to verify that the machine dependent fread() and
	// fwrite() have not garbled the data on you.
	struct {int min_exp,max_exp,digi, min_int,max_int,minus1;
            double epsi;}  testio;
	addr itworkt;
    
	// Just assume malloc works.  You ain't allocating much memory anyway.
    filename_ = (char*) malloc(strlen(filename)+1); // +1 for the \0
	strcpy(filename_,filename);
	exists_ = exists;
    
	if (exists) {
		if (nullptr == (fp_ = fopen(filename_,"rb"))) {
            cout << filename_ << endl;
            LppUrk.LppIssue(15,"Ltuple::Ltuple [1]");
		}
		// Read sample of numeric limits
		itworkt = fread(&testio,sizeof(testio),1,fp_);
		if (itworkt!=1) {
            cout << "Ltuple::Ltuple: fread testio " << feof(fp_) << endl;
			LppUrk.LppIssue(15,"Ltuple::Ltuple [2]");
		}
        
		if (testio.min_exp != numeric_limits<double>::min_exponent) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [3]");
		}
		if (testio.max_exp != numeric_limits<double>::max_exponent) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [4]");
		}
		if (testio.digi != numeric_limits<double>::digits) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [5]");
		}
		if (testio.epsi != numeric_limits<double>::epsilon()) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [6]");
		}
		if (testio.min_int != numeric_limits<int>::min()) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [7]");
		}
		if (testio.max_int != numeric_limits<int>::max()) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [8]");
		}
		if (testio.minus1 != -1) {
			LppUrk.LppIssue(15,"Ltuple::Ltuple [9]");
		}
	} else {
        
		// fopen has the property that if you ask it to open an existing
		// file in write mode, it will erase any existing file of that
		// name.  So try to open it and hope to fail:
		if (nullptr != (fp_ = fopen(filename_,"rb"))) {
            cout << filename << endl;
            LppUrk.LppIssue(13,"Ltuple::Ltuple");
		}
		if (nullptr == (fp_ = fopen(filename_,"wb"))) {
            cout << filename << endl;
            LppUrk.LppIssue(14,"Ltuple::Ltuple [1]");
		}
		// Write sample of numeric limits
		testio.min_exp = numeric_limits<double>::min_exponent;
		testio.max_exp = numeric_limits<double>::max_exponent;
		testio.digi    = numeric_limits<double>::digits;
		testio.epsi    = numeric_limits<double>::epsilon();
		testio.min_int = numeric_limits<int>::min();
		testio.max_int = numeric_limits<int>::max();
		testio.minus1  = -1;
        
		itworkt = fwrite(&testio,sizeof(testio),1,fp_);
		if (itworkt<1) {
            cout << filename << endl;
            LppUrk.LppIssue(14,"Ltuple::Ltuple [2]");
		}
	}
	return;
}




void Ltuple::clear_ple() {
	memset(&ple,0,sizeof(ple));
}


// ---   Read/write/copy the Ltuple   ----------------------------------
void Ltuple::write_ple() const {
	addr itworkt = fwrite(&ple,sizeof(ple),1,fp_);
	// If you didn't even return from fwrite, maybe you overwrote fp_
	if (itworkt<1) {
		cout << filename_ << endl;
		LppUrk.LppIssue(14,"Ltuple::write_ple()");
	}
}
bool Ltuple::read_ple() {
	addr itworkt = fread(&ple,sizeof(ple),1,fp_);
	if (feof(fp_) != 0) return false;
	if (itworkt!=1) {
		cout << filename_ << endl;
		LppUrk.LppIssue(15,"Ltuple::write_ple()");
    }
	return true;
}
