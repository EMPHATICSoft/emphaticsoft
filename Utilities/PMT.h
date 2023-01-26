////////////////////////////////////////////////////////////////////////
/// \file    PMT.h
/// \brief
/// \version 
/// \author  wanly@bu.edu
////////////////////////////////////////////////////////////////////////

#ifndef ARICH_PMT_H
#define ARICH_PMT_H

#include <utility>
#include <vector>

namespace emph {
	namespace arich_util {

		class PMT {
			public:
				PMT();
				PMT(int n, std::string name, double dark, double qecor, std::vector<std::pair<double, double> > qe);
				~PMT() {};

				int PMTnum() const { return fPMTnum;} 
				std::string Name() const { return fName;} 
				double DarkRate() const {return fDarkRate;}
				double QEcor() const {return fQECorrection;}
				std::vector<std::pair<double, double> > QE() const {return fQE;}

				void SetPMTnum(int n) {fPMTnum = n; }
				void SetName(std::string name) {fName = name; }
				void SetDarkRate(double dark) {fDarkRate = dark;}
				void SetQEcor(double qecor) {fQECorrection = qecor;}
				void SetQE(std::vector<std::pair<double, double> > qe) {fQE = qe;}

				bool ifDet(double wavelength);
				int findBlockNumberFromName(std::string name);

			private:    
				int fPMTnum;
				std::string fName;
				double fDarkRate;
				double fQECorrection;
				std::vector<std::pair<double, double> > fQE;
		};

	}  // end namespace geo
} // end namespace emph


#endif
