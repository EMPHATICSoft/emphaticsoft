////////////////////////////////////////////////////////////////////////
/// \file    PMT.cxx
/// \brief
/// \version 
/// \author  wanly@bu.edu
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "TRandom3.h"
#include "PMT.h"

namespace emph {
	namespace arich_util {

		PMT::PMT() :
			fPMTnum(-1), fDarkRate(0), fQECorrection(1)
		{
		}

		//--------------------------------------------------------------------------------

		PMT::PMT(int n, double dark, double qecor, std::vector<std::pair<double, double> > qe) :
			fPMTnum(n), fDarkRate(dark), fQECorrection(qecor), fQE(qe)
		{
		}

		bool PMT::ifDet(double wavelength)
		{
			//return true;
			std::vector<std::pair<double, double> >::const_iterator iter = fQE.begin();
			std::vector<std::pair<double, double> >::const_iterator iter1 = fQE.end();
			iter1--;
			if(wavelength<=iter->first || wavelength>=iter1->first)
				return false;
			else
			{
				while(wavelength>(iter->first))
				{
					iter1 = iter;
					iter++;
				}
				double w1 = iter1->first;
				double w2 = iter->first;
				double q1 = iter1->second;
				double q2 = iter->second;
				double eff1 = q1+(q2-q1)/(w2-w1)*(wavelength-w1);
				double eff = eff1*fQECorrection;

				TRandom3 *rand = new TRandom3(0);
				if( rand->Uniform() <= eff ) return true;
				else return false;
			}

			return false;
		}

		int PMT::findBlockNumberFromName(std::string name)
		{
			int x, y, ax, ay;
			std::string blockNumStr;
			if (name.find("mPMT") == 0 ) {
				blockNumStr = name.substr(9, 1);
				y = atoi(blockNumStr.c_str());
				blockNumStr = name.substr(11, 1);
				x = atoi(blockNumStr.c_str());
				blockNumStr = name.substr(18, 1);
				ay = atoi(blockNumStr.c_str());
				blockNumStr = name.substr(20, 1);
				ax = atoi(blockNumStr.c_str());
				return y*1000+x*100+ay*10+ax;
			}
			return -1;
		}


	} // end namespace geo
} // end namespace emph
