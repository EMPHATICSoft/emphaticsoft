////////////////////////////////////////////////////////////////////////
/// \file    Geometry.h
/// \brief
/// \version 
/// \author  jpaley@fnal.gov wanly@bu.edu
/// Check DocDB 1260 for details.
////////////////////////////////////////////////////////////////////////

#ifndef GEO_GEOMETRY_H
#define GEO_GEOMETRY_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "TVector3.h"
#include "TGDMLMatrix.h"
#include "Geometry/DetectorDefs.h"
#include "Utilities/PMT.h"

class TGeoNode;
class TGeoVolume;
class TGeoMaterial;
class TGeoManager;
class TVector3;

namespace emph {
	namespace geo {
		enum sensorView {
			INIT=0,
			X_VIEW=1, ///< x-measuring view
			Y_VIEW,   ///< y-measuring view
			U_VIEW,
			W_VIEW
		};
		class Strip {
			public:
				Strip();
				Strip(std::string name, TVector3 pos, double dw);
				~Strip() {};

				std::string Name() const { return fName;}
				TVector3 Pos() const { return fPos;}
				double Dw() const { return fDw;}

				void SetName(std::string n) {fName = n; }
				void SetPos(TVector3 pos) {fPos = pos;}
				void SetDw(double dw) {fDw = dw;}

			private:    
				std::string fName;
				TVector3 fPos;
				double fDw;
		};


		class Detector {
			public:
				Detector();
				Detector(std::string name, TVector3 pos, double dz, double w, double h);
				~Detector() {};

				std::string Name() const { return fName;}
				TVector3 Pos() const { return fPos;}
				double Rot() const { return fRot;} 
				double IsFlip() const { return fFlip;} 
//				double X() const { return fX;} 
//				double Y() const { return fY;} 
				double Dz() const { return fDz;}
				double Width() const { return fWidth;}
				double Height() const { return fHeight;}
				sensorView View() const;
				void AddStrip(Strip strip) {fStrip.push_back(strip); }

				void SetName(std::string n) {fName = n; }
				void SetPos(TVector3 pos) {fPos = pos;}
				void SetRot(double rot) {fRot = rot;}
				void SetFlip(bool flip) {fFlip = flip;}
//				void SetX(double x) {fX = x;}
//				void SetY(double y) {fY = y;}
				void SetDz(double dz) {fDz = dz;}
				void SetWidth(double w) {fWidth = w;}
				void SetHeight(double h) {fHeight = h;}
				int NStrips() const {return (int)fStrip.size(); };
				Strip GetStrip(int i) const {return fStrip[i]; }

			private:    
				std::string fName;
				TVector3 fPos;
				double fRot; // rotation in x-y plane, starting from y-axis (fRot = 0 for y-axis), anticlockwise as seen by the beam
				bool fFlip; // facing or back to the beam, for fRot = 0, if channel 0 is at the bottom and 640 at the top (read-out on the right side), fFlip = 0
				double fDz;
				double fWidth;
				double fHeight;
				std::vector<Strip> fStrip;
		};

		class SSDStation {
			public:
				SSDStation();
				~SSDStation() {};

				void SetName(std::string n) {fName = n; }
				void SetPos(TVector3 pos) {fPos = pos;}
				void SetDz(double dz) {fDz = dz;}
				void SetWidth(double w) {fWidth = w;}
				void SetHeight(double h) {fHeight = h;}
				void AddSSD(Detector ssd) {fSSD.push_back(ssd); }

				std::string Name() const { return fName; }
				TVector3 Pos() const {return fPos;}

				int NSSDs() const {return (int)fSSD.size(); };
				Detector GetSSD(int i) const {return fSSD[i]; }
				double Dz() const { return fDz;}
				double Width() const { return fWidth; }
				double Height() const {return fHeight; }

			private:
				std::string fName;
				TVector3 fPos;
				double fDz;
				double fWidth;
				double fHeight;
				std::vector<Detector> fSSD;
		};


		class Geometry {
			public:
				Geometry(std::string fname);
				~Geometry() {};

				bool SetGDMLFile(std::string fname);

				double WorldWidth() const  { return fWorldWidth; }
				double WorldHeight() const { return fWorldHeight; }
				double WorldLength() const { return fWorldLength;}

				double MagnetUSZPos() const {return fMagnetUSZPos; }
				double MagnetDSZPos() const {return fMagnetDSZPos; }
				bool MagnetLoad() const {return fMagnetLoad; }

				double DetectorUSZPos(int i) const {return fDetectorUSZPos[i]; }
				double DetectorDSZPos(int i) const {return fDetectorDSZPos[i]; }
				bool DetectorLoad(int i) const {return fDetectorLoad[i]; }

				int NSSDStations() const { return fNSSDStations; }
				int NSSDs() const { return fNSSDs; }
				SSDStation GetSSDStation(int i) {return fSSDStation[i]; }

				int NPMTs() const { return fNPMTs; }
				emph::arich_util::PMT GetPMT(int i){return fPMT[i]; }
				emph::arich_util::PMT FindPMTByName(std::string name);

				//    TGeoMaterial* Material(double x, double y, double z) const;

				std::string GDMLFile() const {return fGDMLFile; }

				TGeoManager* ROOTGeoManager() const { return fGeoManager; }

			private:
				Geometry();

				bool LoadGDMLFile();
				std::vector<std::pair<double, double> > ReadMatrix(TGDMLMatrix* matrix);
				void ExtractPMTInfo(const TGeoVolume* v);
				void ExtractDetectorInfo(int i, const TGeoNode* n);
				void ExtractMagnetInfo(const TGeoVolume* v);
				void ExtractSSDInfo(const TGeoNode* n);

				std::string fGDMLFile;

				int    fNSSDStations;
				int    fNSSDs;
				double fWorldWidth;
				double fWorldHeight;
				double fWorldLength;
				double fMagnetUSZPos;
				double fMagnetDSZPos;
				bool fMagnetLoad;
				std::vector<SSDStation> fSSDStation;
				double fDetectorUSZPos[NDetectors];
				double fDetectorDSZPos[NDetectors];
				bool fDetectorLoad[NDetectors];

				int    fNPMTs;
				std::vector<emph::arich_util::PMT> fPMT;

				TGeoManager* fGeoManager;

		};

	}  // end namespace geo
} // end namespace emph


#endif
