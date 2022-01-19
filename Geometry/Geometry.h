////////////////////////////////////////////////////////////////////////
/// \file    Geometry.h
/// \brief
/// \version 
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#ifndef GEO_GEOMETRY_H
#define GEO_GEOMETRY_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "TVector3.h"

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
    std::vector<geo::Detector> fSSD;
  };
  
  class Geometry {
  public:
    Geometry(std::string fname);
    ~Geometry() {};
    
    bool SetGDMLFile(std::string fname);
 
	 double T0ZPos() const {return fT0ZPos; }
  
    double MagnetUSZPos() const {return fMagnetUSZPos; }
    double MagnetDSZPos() const {return fMagnetDSZPos; }
    
    int NSSDStations() const { return fNSSDStations; }
    int NSSDs() const { return fNSSDs; }
    
    SSDStation GetSSDStation(int i) {return fSSDStation[i]; }
	 
	 double RPCZPos() const {return fRPCZPos; }
	 double LGZPos() const {return fLGZPos; }
    
    //    TGeoMaterial* Material(double x, double y, double z) const;
    
    std::string GDMLFile() const {return fGDMLFile; }
    
    TGeoManager* ROOTGeoManager() const;
    
  private:
    Geometry();
    
    bool LoadGDMLFile();
    void ExtractMagnetInfo(const TGeoVolume* v);
    void ExtractRICHInfo(const TGeoVolume* v);
    void ExtractSSDInfo(const TGeoNode* n);
    
    std::string fGDMLFile;

    int    fNSSDStations;
    int    fNSSDs;
	 double fT0ZPos;
    double fMagnetUSZPos;
    double fMagnetDSZPos;
    double fRICHUSZPos;
    double fRICHDSZPos;
	 double fRPCZPos;
	 double fLGZPos;
    std::vector<geo::SSDStation> fSSDStation;
    
  };
  
}  // end namespace geo
} // end namespace emph


#endif
