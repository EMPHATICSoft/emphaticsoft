////////////////////////////////////////////////////////////////////////
/// \file    Geometry.h
/// \brief
/// \version 
/// \author  jpaley@fnal.gov wanly@bu.edu
////////////////////////////////////////////////////////////////////////

#ifndef GEO_GEOMETRY_H
#define GEO_GEOMETRY_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "TVector3.h"
#include "DetectorDefs.h"

class TGeoNode;
class TGeoVolume;
class TGeoMaterial;
class TGeoManager;
class TVector3;

namespace emph {
namespace geo {

  class Detector {
  public:
    Detector();
    Detector(std::string name, TVector3 pos, double dz, double w, double h);
    ~Detector() {};

    std::string Name() { return fName;}
    TVector3 Pos() { return fPos;}
    double Dz() { return fDz;}
    double Width() { return fWidth;}
    double Height() { return fHeight;}
    
    void SetName(std::string n) {fName = n; }
    void SetPos(TVector3 pos) {fPos = pos;}
    void SetDz(double dz) {fDz = dz;}
    void SetWidth(double w) {fWidth = w;}
    void SetHeight(double h) {fHeight = h;}
    
  private:    
    std::string fName;
    TVector3 fPos;
    double fDz;
    double fWidth;
    double fHeight;
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

    std::string Name() { return fName; }
    TVector3 Pos() {return fPos;}

    int NSSDs() const {return (int)fSSD.size(); };
    Detector GetSSD(int i) {return fSSD[i]; }
    void AddSSD(Detector ssd) {fSSD.push_back(ssd); }
    double Dz() { return fDz;}
    double Width() { return fWidth; }
    double Height() {return fHeight; }
    
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
 
    double MagnetUSZPos() const {return fMagnetUSZPos; }
    double MagnetDSZPos() const {return fMagnetDSZPos; }
    
	 double DetectorUSZPos(int i) const {return fDetectorUSZPos[i]; }
	 double DetectorDSZPos(int i) const {return fDetectorDSZPos[i]; }
	 bool DetectorLoad(int i) const {return fDetectorLoad[i]; }
    
    int NSSDStations() const { return fNSSDStations; }
    int NSSDs() const { return fNSSDs; }
    
    SSDStation GetSSDStation(int i) {return fSSDStation[i]; }
	 
    //    TGeoMaterial* Material(double x, double y, double z) const;
    
    std::string GDMLFile() const {return fGDMLFile; }
    
    TGeoManager* ROOTGeoManager() const;
    
  private:
    Geometry();
    
    bool LoadGDMLFile();
    void ExtractDetectorInfo(int i, const TGeoNode* n);
    void ExtractMagnetInfo(const TGeoVolume* v);
    void ExtractSSDInfo(const TGeoNode* n);
    
    std::string fGDMLFile;

    int    fNSSDStations;
    int    fNSSDs;
    double fMagnetUSZPos;
    double fMagnetDSZPos;
    std::vector<SSDStation> fSSDStation;
    double fDetectorUSZPos[NDetectors];
    double fDetectorDSZPos[NDetectors];
    bool fDetectorLoad[NDetectors];
    
  };
  
}  // end namespace geo
} // end namespace emph


#endif
