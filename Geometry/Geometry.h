///////////////////////////////////////////////////////////////////////
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
#include "TGeoMatrix.h"

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
      void LocalToMother(double x1[3], double x2[3]) const { fGeoMatrix->LocalToMaster(x1,x2); }
      //      TGeoMatrix* GeoMatrix() const { return fGeoMatrix; }

      void SetName(std::string n) {fName = n; }
      void SetPos(TVector3 pos) {fPos = pos;}
      void SetDw(double dw) {fDw = dw;}
      void SetGeoMatrix(TGeoMatrix* m) {fGeoMatrix = m; }

    private:    
      std::string fName;
      TVector3 fPos;
      double fDw;
      TGeoMatrix* fGeoMatrix;
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
      int  Id() const { return fId; }

      void AddStrip(Strip strip) {fStrip.push_back(strip); }

      void SetGeoMatrix(TGeoMatrix* m) {fGeoMatrix = m; }
      void SetGeoMatrixMount(TGeoMatrix* m) {fGeoMatrixMount = m; }
      void SetName(std::string n) {fName = n; }
      void SetPos(TVector3 pos) {fPos = pos;}
      void SetRot(double rot) {fRot = rot;}
      void SetFlip(bool flip) {fFlip = flip;}
      void SetId(int id) {fId = id;}
      //				void SetX(double x) {fX = x;}
      //				void SetY(double y) {fY = y;}
      void SetDz(double dz) {fDz = dz;}
      void SetWidth(double w) {fWidth = w;}
      void SetHeight(double h) {fHeight = h;}
      int NStrips() const {return (int)fStrip.size(); };
      const Strip* GetStrip(int i) const {return &fStrip[i]; }
      void LocalToMother(double x1[3], double x2[3]) const;

    private:    
      int   fId;
      std::string fName;
      TVector3 fPos;
      double fRot; // rotation in x-y plane, starting from y-axis (fRot = 0 for y-axis), anticlockwise as seen by the beam
      bool fFlip; // facing or back to the beam, for fRot = 0, if channel 0 is at the bottom and 640 at the top (read-out on the right side), fFlip = 0
      double fDz;
      double fWidth;
      double fHeight;
      std::vector<Strip> fStrip;
      TGeoMatrix* fGeoMatrix;
      TGeoMatrix* fGeoMatrixMount;
    };

    class Plane {
    public:
      Plane();
      ~Plane() {};

      const Detector* SSD(int i) const { return &fSSD[i];}
      int  NSSDs() const { return int(fSSD.size()); }
      sensorView View() const { if (fSSD.empty()) return INIT; return fSSD[0].View(); }

      void AddSSD(Detector ssd) { fSSD.push_back(ssd); }

    private:
      std::vector<Detector> fSSD;
    };

    class SSDStation {
    public:
      SSDStation();
      ~SSDStation() {};

      void SetName(std::string n) {fName = n; }
      void SetId(int id) { fId = id; }
      void SetPos(TVector3 pos) {fPos = pos;}
      void SetDz(double dz) {fDz = dz;}
      void SetWidth(double w) {fWidth = w;}
      void SetHeight(double h) {fHeight = h;}
      void AddPlane(Plane p) {fPlane.push_back(p); }
      void SetGeoMatrix(TGeoMatrix* m) {fGeoMatrix = m; }

      std::string Name() const { return fName; }
      int Id() const { return fId; }
      TVector3 Pos() const {return fPos;}

      int NPlanes() const {return (int)fPlane.size(); };
      const Plane* GetPlane(int i) const {return &fPlane[i]; }
      double Dz() const { return fDz;}
      double Width() const { return fWidth; }
      double Height() const {return fHeight; }
      void LocalToMother(double x1[3], double x2[3]) const
      {fGeoMatrix->LocalToMaster(x1,x2);}
      //      TGeoMatrix* GeoMatrix() const { return fGeoMatrix; }

    private:
      int fId;
      std::string fName;
      TVector3 fPos;
      double fDz;
      double fWidth;
      double fHeight;
      std::vector<Plane> fPlane;
      TGeoMatrix* fGeoMatrix;
    };

    class Target {
    public:
      Target();
      ~Target() {};

      void SetPos(TVector3 p) { fPos = p; }
      void SetDPos(TVector3 dp) { fDPos = dp; }
      void SetDensity(double rho) { fDensity = rho; }
      void AddElement(std::string el, double frac, double A, int Z) 
      { fEl.push_back(el); fFrac.push_back(frac); 
	fA.push_back(A); fZ.push_back(Z); }

      TVector3 Pos() const { return fPos; }
      TVector3 DPos() const { return fDPos; }
      double   Density() const { return fDensity; }
      int      NEl() const { return fEl.size(); }
      std::string El(int i) const { return fEl[i]; }
      double   Frac(int i) const { return fFrac[i]; }
      double   AtomA(int i) const { return fA[i]; }
      int      AtomZ(int i) const { return fZ[i]; }

    private:
      TVector3 fPos;
      TVector3 fDPos;
      double   fDensity;
      std::vector<std::string> fEl;
      std::vector<double> fFrac;
      std::vector<double> fA;
      std::vector<int>    fZ;

    };

    class Geometry {
    public:
      Geometry(std::string fname);
      ~Geometry() {};

      bool SetGDMLFile(std::string fname);

      double WorldWidth() const  { return fWorldWidth; }
      double WorldHeight() const { return fWorldHeight; }
      double WorldLength() const { return fWorldLength;}

      double TargetUSZPos() const {return fTargetUSZPos; }
      double TargetDSZPos() const {return fTargetDSZPos; }

      double MagnetUSZPos() const {return fMagnetUSPos[2]; }
      double MagnetDSZPos() const {return fMagnetDSPos[2]; }
      double MagnetUSXPos() const {return fMagnetUSPos[0]; }
      double MagnetDSXPos() const {return fMagnetDSPos[0]; }
      double MagnetUSYPos() const {return fMagnetUSPos[1]; }
      double MagnetDSYPos() const {return fMagnetDSPos[1]; }
      bool MagnetLoad() const {return fMagnetLoad; }

      double DetectorUSZPos(int i) const {return fDetectorUSZPos[i]; }
      double DetectorDSZPos(int i) const {return fDetectorDSZPos[i]; }
      bool DetectorLoad(int i) const {return fDetectorLoad[i]; }

      int NSSDStations() const { return fNSSDStations; }
      int NSSDPlanes() const { return fNSSDPlanes; }
      int NSSDs() const { return fNSSDs; }
      const SSDStation* GetSSDStation(int i) const {return &fSSDStation[i]; }
      const Detector* GetSSDSensor(int i) {return fSSDSensorMap[i]; }
      int GetSSDId(int station, int plane, int sensor) const;

      int NPMTs() const { return fNPMTs; }
      emph::arich_util::PMT GetPMT(int i){return fPMT[i]; }
      emph::arich_util::PMT FindPMTByName(std::string name);
      emph::arich_util::PMT FindPMTByBlockNumber(int number);
      const Target* GetTarget() { return fTarget; }
      
      //    TGeoMaterial* Material(double x, double y, double z) const;

      std::string GDMLFile() const {return fGDMLFile; }

      TGeoManager* ROOTGeoManager() const { return fGeoManager; }

      bool IsLoaded() const { return fIsLoaded; }

    private:
      Geometry();

      bool LoadGDMLFile();
      std::vector<std::pair<double, double> > ReadMatrix(TGDMLMatrix* matrix);
      void ExtractPMTInfo(const TGeoVolume* v);
      void ExtractDetectorInfo(int i, const TGeoNode* n);
      void ExtractMagnetInfo(const TGeoVolume* v);
      void ExtractSSDInfo(const TGeoNode* n);
      void ExtractTargetInfo(const TGeoVolume* v);

      bool fIsLoaded;

      std::string fGDMLFile;

      int    fNSSDStations;
      int    fNSSDPlanes;
      int    fNSSDs;
      double fWorldWidth;
      double fWorldHeight;
      double fWorldLength;
      double fMagnetUSPos[3];
      double fMagnetDSPos[3];
      double fTargetUSZPos;
      double fTargetDSZPos;
      bool   fMagnetLoad;
      std::vector<SSDStation> fSSDStation;
      double fDetectorUSZPos[NDetectors];
      double fDetectorDSZPos[NDetectors];
      bool   fDetectorLoad[NDetectors];
      int    fNPMTs;
      std::vector<emph::arich_util::PMT> fPMT;
      std::unordered_map<int, const Detector*> fSSDSensorMap;
      Target* fTarget;

      TGeoManager* fGeoManager;

    };
  }  // end namespace geo
} // end namespace emph


#endif
