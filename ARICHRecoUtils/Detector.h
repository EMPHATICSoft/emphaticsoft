////////////////////////////////////////////////////////////////////////
///// \brief   Definition of DETECTOR class
///// \author  mdallolio
///// \date
//////////////////////////////////////////////////////////////////////////


#ifndef DETECTOR_INCLUDE
#define DETECTOR_INCLUDE

#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TH2Poly.h"
#include "TH1D.h"
#include "Beam.h"
#include "Photon.h"
#include "Particle.h"
#include "Utility.h"

    namespace ARICHRECO{

        class Detector {
        private:
          double darkrate;
          double fillFactor;
          double window;
          double zPos;
          TFile *fdet;
          TH2D *hDet;
          TGraph *quantumEff;
          bool mirror;
          std::shared_ptr<TRandom3> randomGenerate;

        public:
          double xmin = -24.2;
          double xmax = 24.2;
          double ymin = -24.2;
          double ymax = 24.2;

          Detector(bool mirror, double drate, double dwin, double ffactor, double zposition, TString file);
          virtual ~Detector();
          TH2D* makeDetectorHist(char* name, char* title);
          double evalQEff(double);
          double getDarkRate();
          double getWin();
          double getFillFactor();
          double getDist()const { return zPos;};
          void projectPhotons(TH2D*,  std::vector<ARICHRECO::Photon*>, TH1D* rHist = nullptr);

        };
}
#endif