////////////////////////////////////////////////////////////////////////
/////// \brief   AEROGEL class
/////// \author  mdallolio
/////// \date
////////////////////////////////////////////////////////////////////////////


#include "Aerogel.h"
#include "TF1.h"
using namespace std;


const double lowWav = 282E-9;
const double highWav = 892E-9;
const double fineStructConst = 1./137;

namespace ARICHRECO{

        Aerogel::Aerogel(double refractiveIndex, double thickness,  double zPos) {
          randomGenerate=std::make_shared<TRandom3>();
          randomGenerate->SetSeed(0);
          this->thickness = thickness;
          this->refracIndex = refractiveIndex;
          this->zPos = zPos;
          this->scatAngleFunc = new TF1("scatPdf", "1 + cos(x)*cos(x)", 0, TMath::Pi());
          this->interactionLengths = readInteractionLength(refractiveIndex);
        }


        double Aerogel::getThickness() {
          return thickness;
        }

        double Aerogel::getRefractiveIndex() {
          return refracIndex;
        }

        double Aerogel::getZPos() {
          return zPos;
        }

        void Aerogel::setUpIndex(double n) {
          /*
          Note refractive index of upstream material
          */
          this->upIndex = n;
        }

        void Aerogel::setDownIndex(double n) {
          /*
          Note refractive index of downstream material
          */
          this->downIndex = n;
        }

        double Aerogel::calcChAngle(double beta) {
          /*
          Calculate Cherenkov angle
          */
          return acos(1.0 / (refracIndex * beta));
        }

        double Aerogel::calcdNdX(double beta) {
          /*
          Integrate over wavelength range to get dN/dx,
          the number of photons emitted per unit length
          */
          double constTerm = 2*TMath::Pi()*fineStructConst;
          double nbeta = refracIndex*beta;
          return constTerm*(1. - 1./(nbeta*nbeta))*(1./lowWav - 1./highWav);
        }

        std::vector<double> Aerogel::readInteractionLength(double n) {
          /*
          Read Aerogel interaction lengths from file in data folder
          */
          std::string files[5] = {"leps1-1b","btr4-1a","hds2-3b","leps2-1a","leps6-1a"};
          float ns[5] = {1.0505,1.0452,1.0401,1.0352,1.0297};
          // Get nearest index of refraction
          float minDiff = 1.;
          int minI = 0;
          for (int i = 0; i < 5; i++) {
            if (abs(n - ns[i]) < minDiff) {
              minDiff = abs(n - ns[i]);
              minI = i;
            }
          }
          // Get the corresponding file of interaction lengths
          std::string fileName = "../emphaticsoft/ARICHRecoUtils/data/" + files[minI] + "IntLength.csv";
          

	  std::ifstream intLengthFile(fileName);

          std::vector<double> wavs;
          std::vector<double> intLengths;
          std::string line;

          while(std::getline(intLengthFile,line, '\n')) {
            std::string wavString = line.substr(0, line.find(' '));
            std::string intLengthString = line.substr(line.find(' ')+1, -1);

            wavs.push_back(atof(wavString.c_str()));
            intLengths.push_back(atof(intLengthString.c_str()));
          }

          return intLengths;
        }

        double Aerogel::getRandomWav() {
          /*
          Use inverse transform sampling to get random wavelength.
          1) Wavelength PDF is proportional to 1/wav^2
          2) Normalized wavelength pdf: P = (1/(1/lowWav - 1/highWav))*(1/wav^2)
          3) Integrate, get CDF: C = (1/(1/lowWav - 1/highWav))*(1/lowWav - 1/wav)
          4) Inverse of CDF: wav = 1/(1/lowWav - (1/lowWav - 1/highWav) * C)
          5) If we sample C from 0 to 1, we can get a random wavelength sample
          This is about twice as fast as just randomly sampling from 1/wav^2
          */
          double u = randomGenerate->Uniform(0,1);
          return 1./(1./lowWav - (1./lowWav - 1./highWav)*u);
        }


        bool Aerogel::isInAerogel(TVector3 pos) {
          /*
          Check if given position is in aerogel
          */
          return (TMath::Abs(pos[0]) <= width/2.)
                  && (TMath::Abs(pos[1]) <= height/2.)
                  && ((pos[2] - zPos) <= thickness)
                  && ((pos[2] - zPos) >= 0);
        }

        double Aerogel::getDistInGel(ARICHRECO::Particle* pa) {
          /*
          Calculate how far a particle has remaining in the gel
          */
          return abs((zPos + thickness - pa->pos[2]) / pa->dir[2]);
        }

        void Aerogel::exitAerogel(ARICHRECO::Photon* ph, bool refract, double n2) {
          /*
          Advance photon to closest edge of aerogel in direction of travel and refract
          */

          // Get distance to each wall in direction of travel
          double xDist = (TMath::Sign(width/2, ph->dir[0]) - ph->pos[0]) / ph->dir[0];
          double yDist = (TMath::Sign(height/2, ph->dir[1]) - ph->pos[1]) / ph->dir[1];

          bool forwardZ = ph->dir[2] > 0.;
          double zDist = 0;
          if (forwardZ) {
            zDist = (zPos + thickness - ph->pos[2]) / ph->dir[2];
          } else {
            zDist = (zPos - ph->pos[2]) / ph->dir[2];
          }
          // Pick the x y or z dimension with the closest wall and travel that way
          double dists[3] = {xDist, yDist, zDist};
          int minDistDimension = TMath::LocMin(3, dists);
          ph->travelDist(dists[minDistDimension]+1e-8);

          // Get the normal vector of that wall
          TVector3 incidentPlane = TVector3(0.,0.,0.);
          incidentPlane[minDistDimension] = TMath::Sign(1, -ph->dir[minDistDimension]);

          if (refract) {
            // Refract the light
             ph->dir = refractedDirection(ph->dir, incidentPlane, refracIndex, n2);
          }
        }

        void Aerogel::exitAerogel(std::vector<ARICHRECO::Photon*> photons, bool refract, double n2) {
          /*
          Exit and refract all photons that are still in the aerogel
          */
          for(int i = 0; (unsigned) i < photons.size(); i++) {
            Photon* photon = photons[i];
            if (isInAerogel(photon->pos)) {
              // Scatter the photon
              exitAerogel(photon, refract, n2);
            }
          }
        }

        int Aerogel::calcNumPhotons(double particleDist, double beta) {
          /*
          number of photons: N ~= dN/dX * X (in meters)
          */
          return (int) particleDist*0.01*calcdNdX(beta);
        }

        double Aerogel::getIntLengthForWav(double wav) {
          // Get index of nearest wavelengths
          double maxWav = 800.;
          double deltaWav = 0.5;
          wav = wav*1E9; // convert to nm
          int index = floor((maxWav - wav)/deltaWav);
	  return interactionLengths[index];
        }

        double Aerogel::getRandomIntDistance(double wav) {
          /*
          Inverse Transform Sampling:
          Our CDF of whether photon has interacted is 1 - exp(- x / interactionL)
          Sample randomly for value of CDF
          Invert the equation to get the x value that would get yield random value
          */
          double intLength = getIntLengthForWav(wav);
          double randY = randomGenerate->Uniform(0,1);
	  return - intLength * log(1. - randY);
        }

        double Aerogel::getRandomScatAngle() {
          /*
          Rayleigh scattering is proportional to  1 + cos^2(theta)
          */
          return scatAngleFunc->GetRandom();
        }

        void Aerogel::applyPhotonScatter(ARICHRECO::Photon* photon) {
          /*
          Continuously scatter photon while the "distance travelled before interacting"
          is less than the distance to exit the aerogel - update position and direction
          after each scattering event
          */
          if (!isInAerogel(photon->pos)) {
            return;
          }
          double intDist = getRandomIntDistance(photon->wav);
	  TVector3 newPos = photon->pos + intDist*photon->dir;
          TVector3 newDir = photon->dir;
	 while (isInAerogel(newPos) && (photon->numScatters<=100)) {
            // update position
            photon->pos = newPos;
	  // update direction
            double scatTheta = getRandomScatAngle();
            double scatPhi = randomGenerate->Uniform(0., 2.*TMath::Pi());
            TVector3 dirScat = TVector3(sin(scatTheta)*cos(scatPhi),
                                        sin(scatTheta)*sin(scatPhi),
                                        cos(scatTheta));
            dirScat.RotateUz(photon->dir);
            photon->dir = dirScat;
            // Predict where it might scatter next
            photon->numScatters += 1;
            intDist = getRandomIntDistance(photon->wav);
            newPos = photon->pos + intDist*photon->dir;
	    }
        }

        void Aerogel::applyPhotonScatters(std::vector<ARICHRECO::Photon*> photons) {
          /*
          Scatter all photons in the aerogel
          */
          for(int i = 0; (unsigned) i < photons.size(); i++) {
              ARICHRECO::Photon* photon = photons[i];
            if (isInAerogel(photon->pos)) {
              // Scatter the photon
              applyPhotonScatter(photon);
            }
          }
        }

        std::vector<Photon*> Aerogel::generatePhotons(ARICHRECO::Particle* pa, ARICHRECO::Detector* detector) {
          /*
          Create Cherenkov photons as the particle passes through the gel.
          Hacky, but requires detector to immediately reject based off detector efficiency,
          which saves on time spent computing photons that wouldn't be detected anyways
          */
          double beta = pa->beta;
          double paDist = getDistInGel(pa);
          double chAngle = calcChAngle(beta);
          std::vector<ARICHRECO::Photon*> photons;
          if (refracIndex * beta < 1) return photons; // Enforce Cherenkov threshold
          int nPhotons = calcNumPhotons(paDist, beta);
          if(nPhotons < 0) nPhotons = 0;
          photons.reserve(nPhotons);
          for (int i = 0; i < nPhotons; i++){
            // First decide if we want to throw it out yet, based on quantum efficiency
            // Can do this now because the photon's wavelength won't change
            double wav = getRandomWav();
            if (randomGenerate->Uniform(0,1) > detector->evalQEff(wav)) continue;
            else{
              // Get the point where the photon was emitted
              double phIntDist = randomGenerate->Uniform(paDist);
              TVector3 phPos = pa->pos + phIntDist*pa->dir;
              // Get the direction of the new photon
              double phPhi = randomGenerate->Uniform(0., 2.*TMath::Pi());
              TVector3 dirCR = TVector3(sin(chAngle)*cos(phPhi),
                                        sin(chAngle)*sin(phPhi),
                                        cos(chAngle));
              // Rotate onto particle direction
              dirCR.RotateUz(pa->dir);
                ARICHRECO::Photon* photon = new ARICHRECO::Photon(phPos, dirCR, wav);
              photons.push_back(photon);
            }
          }
          return photons;
        }

}


