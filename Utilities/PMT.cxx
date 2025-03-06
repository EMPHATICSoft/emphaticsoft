////////////////////////////////////////////////////////////////////////
/// \file    PMT.cxx
/// \brief
/// \version 
/// \author  wanly@bu.edu
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <ostream> 
#include <iostream>
#include "TRandom3.h"
#include "PMT.h"
#include "TMath.h"
#include "CLHEP/Random/Randomize.h"



namespace emph {
  namespace arich_util {

    PMT::PMT() :
      fPMTnum(-1), fName(""), fXTalk(0.01), fDarkRate(0), fQECorrection(1), 
      fON(true), fTriggerWin(20)    //need to implement thes from GDML file
    {
      rand_gen = new TRandom3(0);	
    }

    //--------------------------------------------------------------------------------

    PMT::PMT(int n, std::string name, double dark, double qecor, double fxtalk, std::vector<std::pair<double, double> > qe) :
      fPMTnum(n), fName(name), fXTalk(fxtalk), fDarkRate(dark), 
      fQECorrection(qecor), fQE(qe)
    {
      rand_gen = new TRandom3(0);
    }


    void PMT::PrintInfo(){

      std::cout<<"PMT_"<< fPMTnum <<":"<<std::endl;
      std::cout<<"Dark noise rate: "<<fDarkRate<<" Hz"<<std::endl;
      std::cout<<"QE correction: "<<fQECorrection<<std::endl;
      std::cout<<"Trigger window: "<<fTriggerWin<<std::endl;
      std::cout<<"Cross Talk: "<< fXTalk <<std::endl;
      std::cout<<"Postion: ( "<<fPos[0] <<"mm, "<<fPos[1] <<"mm, "<<fPos[2] <<"mm )"<<std::endl;
      std::cout<<std::endl;

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

	  if( rand_gen->Uniform() <= eff ) return true;
	  else return false;
	}

      return false;
    }

    double PMT::GetDarkRate()
    {
      double DarkProb=fDarkRate*1e-9;// 1/ns  [P = rate * time] for Hz the conversion is 10^-9
      double val = rand_gen->Uniform();
      if( val > DarkProb) return -1;
      else return  val ;
    }		

		
    double PMT::GetCharge()
    {
      double charge=1;
      while(rand_gen->Uniform() < fXTalk){
	charge++;
      }
      return charge*fQECorrection;
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


    std::vector<float> PMT::GetPosfromBlockNumber(int block_number)
    {
      std::vector<float> pos;
      std::vector<int> vnumber;
      std::string number = std::to_string(block_number);
		           
      double mPMT_thick = 16.4; // mm
      double mPMT_size = 49.3;// mm
      double mPMT_gap = 5.4; // mm
      double manode_size = 6.0; // mm
      double mPMT_shift = 103.0; // mm

      double x, y, z;

      for(auto& Integer: number){	
	vnumber.push_back(static_cast<int>(Integer - '0'));
      }
      while(vnumber.size() < 4){
	vnumber.insert(vnumber.begin(),0); }

      // vnumber is now [y_0,x_0,ay_0,ax_0] -> use the positional indeces to compute x,y,z position
      // x = (-1+x_0)*(mPMT_size+mPMT_gap)+(-3.5+ax_0)*manode_size 
      // y = (-1+y_0)*(mPMT_size+mPMT_gap)+(-3.5+ay_0)*manode_size
      // z = mPMT_shift+0.5*mPMT_thick 
		
      x = (-1+vnumber[1])*(mPMT_size+mPMT_gap)+(-3.5+vnumber[3])*manode_size;
      y = (-1+vnumber[0])*(mPMT_size+mPMT_gap)+(-3.5+vnumber[2])*manode_size;
      z = mPMT_shift+0.5*mPMT_thick; 
                
      pos.push_back(x);
      pos.push_back(y);
      pos.push_back(z);
      return pos;
    }
  } // end namespace arich_utils
} // end namespace emph
