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
				PMT(int n, double dark, double qecor, std::vector<std::pair<double, double> > qe);
				~PMT() {};

				int PMTnum() const { return fPMTnum;} 
				double DarkRate() const {return fDarkRate;}
				double QEcor() const {return fQECorrection;}

				void SetPMTnum(int n) {fPMTnum = n; }
				void SetDarkRate(double dark) {fDarkRate = dark;}
				void SetQEcor(double qecor) {fQECorrection = qecor;}
				void SetQE(std::vector<std::pair<double, double> > qe) {fQE = qe;}

				bool ifDet(double wavelength);
				int findBlockNumberFromName(std::string name);

			private:    
				int fPMTnum;
				double fDarkRate;
				double fQECorrection;
				std::vector<std::pair<double, double> > fQE;
		};

	}  // end namespace geo
} // end namespace emph


#endif
