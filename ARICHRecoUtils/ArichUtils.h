////////////////////////////////////////////////////////////////////////
///// \brief   Definition of ARICH_UTILS class
///// \author  mdallolio
///// \date
//////////////////////////////////////////////////////////////////////////



#ifndef ARICH_UTILS_H
#define ARICH_UTILS_H

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Geometry/service/GeometryService.h"
#include "ARICHRecoUtils/Detector.h"
#include "ARICHRecoUtils/Arich.h"
#include "HoughFitter.h"
#include "TFile.h"
#include "Geometry/Geometry.h"

#include "TVector3.h"
#include <map>
namespace ARICHRECO{

    class ARICH_UTILS {

	public: 

	ARICH_UTILS();
	//ARICH_UTILS(std::string fParamsFile);
	void SetUpDet(double PDdarkrate, double PDwin, double PDfillfactor, double PDzpos,TString PDfile);
	void SetUpArich(double up_n, double down_n, double up_pos, double down_pos, double up_thick, double down_thick);
	~ARICH_UTILS();
	
	double computeLogLikelihood(TH2D* event, TH2D* distribution);
	std::vector<double> recoCherenkov(TH2Poly* eventHist, int nDetected, std::vector<TVector3> pos0s, std::vector<TVector3> dir0s);	
	TH2D* DigsToHist(std::vector<int>& blocks);

	std::vector<double> IdentifyMultiParticle(TH2D* hist, int np, std::vector<double> mom, std::vector<TVector3> pos0s,std::vector<TVector3> dir0s);

	std::vector<std::vector<TH2D*>> GetPDFs(int np, std::vector<double> mom,  std::vector<TVector3> pos0s,std::vector<TVector3> dir0s);

	double calcBeta(int particlei, double mom);
	
	std::map<std::string, double> GetMap() const {return Particle_LogLike;};
	TString GetFile() const{return PDfile;};
	

	private:
        double PDdarkrate;
	double PDwin;
	double PDfillfactor;
	double PDzpos;
	TString PDfile;

	double up_n;
	double up_pos;
	double up_thick;
	double down_n;
	double down_pos;
	double down_thick;
	
	std::vector<double>  fLogLike;
	ARICHRECO::Detector *Detector;	
        ARICHRECO::Arich    *Arich;	

	 std::map<std::string, double> Particle_LogLike;
    };

} //end ARICHRECO
#endif