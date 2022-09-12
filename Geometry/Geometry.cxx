////////////////////////////////////////////////////////////////////////
/// \file    Geometry.cxx
/// \brief
/// \version 
/// \author  jpaley@fnal.gov wanly@bu.edu
////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <fstream>
#include <stdio.h>

//ROOT includes
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TVirtualGeoPainter.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"
#include "cetlib/search_path.h"

#include "Geometry/Geometry.h"

namespace emph {

  namespace geo {
    
    Detector::Detector() :
      fName(""), fPos(-1e6,-1e6,-1e6), fDz(-99999), fWidth(0.), fHeight(0.)
    {
    }
    
    //--------------------------------------------------------------------------------
    
    Detector::Detector(std::string name, TVector3 pos, double dz, double w, double h) :
      fName(name), fDz(dz), fWidth(w), fHeight(h)
    {
      fPos = pos;
      
    }
    
    //--------------------------------------------------------------------------------
    
    SSDStation::SSDStation() :
      fName(""), fPos(-1e6,-1e6,-1e6), fDz(-99999), fWidth(0.), fHeight(0.), fSSD(0)
    {
      
    }
    
    //--------------------------------------------------------------------------------
    
    Geometry::Geometry() :
      fGDMLFile("")
    {
      fGeoManager = 0;
    }
    
    //--------------------------------------------------------------------------------
    
    Geometry::Geometry(std::string fname) :
      fGDMLFile(fname), fSSDStation(0)
    {
      fGeoManager = 0;
      for ( int i = Trigger ; i < NDetectors ; i ++ ) fDetectorLoad[i] = false;
      this->SetGDMLFile(fname);
    }
    
    //--------------------------------------------------------------------------------
    
    bool Geometry::SetGDMLFile(std::string fname)
    {
      if (fname.empty()) {
	throw cet::exception("GeometrySetGDMLFile")
	  << "cannot use empty string for GDML file, please fix this\n"
	  << __FILE__ << ":" << __LINE__ << "\n";
      }
      
      fGDMLFile = fname;
      return this->LoadGDMLFile();
    }
    
    //--------------------------------------------------------------------------------
    
    bool Geometry::LoadGDMLFile()
    {
      std::ifstream geoFile;
      std::string file_path;
      std::string fname;
      
      geoFile.open(fGDMLFile.c_str());
      if (!geoFile.is_open()) {
	throw cet::exception("GeometryFileLoad")
	  << "cannot find GDML file " << fname << " bail ungracefully\n"
	  << __FILE__ << ":" << __LINE__ << "\n";
	return false;
      }
      geoFile.close();
      
      mf::LogWarning("LoadNewGeometry") << "loading new geometry files\n"
					<< fGDMLFile << "\n";

      //      if (fGeoManager) delete fGeoManager;
      int old_verbosity = gGeoManager->GetVerboseLevel();
      
      // TGeoManager is too verbose when loading geometry.
      // Make it quiet.
      gGeoManager->SetVerboseLevel(0);
      
      TGeoManager::Import(fGDMLFile.c_str());
      
      fGeoManager = gGeoManager;
      
      mf::LogWarning("LoadNewGeometry") << "loaded new geometry files\n";
      
      fGeoManager->SetVerboseLevel(old_verbosity);
      
      const TGeoNode* world_n = (TGeoNode*)fGeoManager->GetTopNode();
      std::cout << "world_n = " << world_n << std::endl;
      
      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();
      std::cout << "world_v = " << world_n << std::endl;
      
      TGeoBBox* world_box = (TGeoBBox*)world_v->GetShape();      
      std::cout << "world_box = " << world_box << std::endl;
      
      fWorldHeight = world_box->GetDY();
      fWorldWidth  = world_box->GetDX();
      fWorldLength = world_box->GetDZ();
      
      //ExtractMagnetInfo(world_v);

      mf::LogWarning("ExtractGeometry") << "extracted magnet geometry \n";
      
      for ( int i = Trigger ; i < NDetectors ; i ++ ){
	ExtractDetectorInfo(i, world_n);
	if ( fDetectorLoad[i] == true ){
	  mf::LogWarning("ExtractGeometry") << "extracted "
					    << DetInfo::Name(DetectorType(i)) << " geometry \n";
	}
      }
      
      return true;
    }
    
    //--------------------------------------------------------------------------------
    
    void Geometry::ExtractDetectorInfo(int i, const TGeoNode* world_n)
    {
      if ( i < 3 || i == ARICH ){
	mf::LogWarning("LoadNewGeometry") << DetInfo::Name(DetectorType(i)) 
					  << " detector not in gdml yet. \n"
					  << "experts should confirm whether they should be implemented. \n";
	return;
      }
      
      if ( i == SSD ){
	
	ExtractSSDInfo(world_n);
	
	if ( fNSSDStations > 0 ){
	  
	  std::cout<<"n SSD Stations: "<<fNSSDStations <<std::endl;
	  std::cout<<"n SSD sensors: "<<fNSSDs <<std::endl;
	  
	  fDetectorLoad[i] = true;
	  fDetectorUSZPos[i] = fSSDStation.front().Pos()[2]-fSSDStation.front().Dz();
	  fDetectorDSZPos[i] = fSSDStation.back().Pos()[2]-fSSDStation.back().Dz();
	  
	}
	
	return;
	
      }
      
      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();
      TString detector_name=DetInfo::Name(DetectorType(i))+"_phys";
      TGeoNode* detector_n = (TGeoNode*)world_v->GetNode(detector_name);
      
      if ( detector_n == nullptr ){
	mf::LogWarning("LoadNewGeometry") << DetInfo::Name(DetectorType(i))
					  << " detector not found in gdml. \n"
					  << "check your spelling. \n";
	return;
      }
      
      TGeoVolume* detector_v = (TGeoVolume*)detector_n->GetVolume();
      TGeoBBox* detector_box = (TGeoBBox*)detector_v->GetShape();
      
      double zcenter = detector_n->GetMatrix()->GetTranslation()[2];
      double dz = detector_box->GetDZ();
      
      fDetectorUSZPos[i] = zcenter-dz;
      fDetectorDSZPos[i] = zcenter+dz;
      fDetectorLoad[i] = true;
      
    }
    
    
    //--------------------------------------------------------------------------------
    
    void Geometry::ExtractMagnetInfo(const TGeoVolume* world_v)
    {
      TGeoNode* magnet_n = (TGeoNode*)world_v->GetNode("magnet_phys");
      TGeoVolume* magnet_v = (TGeoVolume*)magnet_n->GetVolume();
      TGeoBBox* magnet_box = (TGeoBBox*)magnet_v->GetShape();
      
      double zcenter = magnet_n->GetMatrix()->GetTranslation()[2];
      double dz = magnet_box->GetDZ();
      
      fMagnetUSZPos = zcenter-dz;
      fMagnetDSZPos = zcenter+dz;
      
    }
    
    //--------------------------------------------------------------------------------
    
    void Geometry::ExtractSSDInfo(const TGeoNode* world_n)
    {
      int nnodes = world_n->GetNodes()->GetEntries();
      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();
      
      std::vector<std::string> nodeName;
      
      std::string sString = "ssdStation";
      std::string ssubString = "ssdsensor";
      
      for (int i=0; i<nnodes; ++i) {
	std::string name = world_v->GetNode(i)->GetName();
	if (name.find(sString) != std::string::npos)
	  nodeName.push_back(name);
      }
      
      fNSSDStations = (int)nodeName.size();
      fNSSDs = 0;
      
      for (auto name : nodeName) {
	SSDStation st;
	TGeoNode* st_n = (TGeoNode*)world_v->GetNode(name.c_str());
	TGeoVolume* st_v = (TGeoVolume*)st_n->GetVolume();
	TGeoBBox* st_box = (TGeoBBox*)st_v->GetShape();
	
	// first add basic info about the SSD station
	st.SetName(name);
	st.SetDz(st_box->GetDZ());
	st.SetPos(st_n->GetMatrix()->GetTranslation());
	st.SetWidth(2*st_box->GetDX());
	st.SetHeight(2*st_box->GetDY());
	
	// now add individual SSDs to the station
	
	int nsub = st_n->GetNodes()->GetEntries();
	for( int j=0; j<nsub; ++j){
	  std::string name = st_v->GetNode(j)->GetName();
	  if (name.find(ssubString) != std::string::npos){
	    Detector sensor;
	    TGeoNode* sensor_n = (TGeoNode*)st_v->GetNode(name.c_str());
	    TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
	    TGeoBBox* sensor_box = (TGeoBBox*)sensor_v->GetShape();
	    
	    sensor.SetName(name);
	    sensor.SetDz(sensor_box->GetDZ());
	    sensor.SetPos(sensor_n->GetMatrix()->GetTranslation());
	    sensor.SetWidth(2*sensor_box->GetDX());
	    sensor.SetHeight(2*sensor_box->GetDY());
	    
	    st.AddSSD(sensor);
	    fNSSDs++;
	  }
	}
	
	fSSDStation.push_back(st);
      }
      
    }
    
  } // end namespace geo

} // end namespace emph
