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
#include <regex>

//ROOT includes
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGDMLMatrix.h"
#include "TVirtualGeoPainter.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"
#include "cetlib/search_path.h"

#include "Geometry/Geometry.h"

namespace emph {
  namespace geo {

    Strip::Strip() :
      fName(""), fPos(-1e6,-1e6,-1e6), fDw(-99999)
    {
    }

    //--------------------------------------------------------------------------------

    Strip::Strip(std::string name, TVector3 pos, double dw) :
      fName(name), fDw(dw)
    {
      fPos = pos;

    }

    //--------------------------------------------------------------------------------

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

    sensorView Detector::View() const
    {
      // x-view: π/2, 3π/2
      if (abs(fmod(this->Rot()-3.14/2,3.14)) < 0.2)
	return X_VIEW;
      // y-view: 0,π
      else if (abs(fmod(this->Rot(),3.14)) < 0.2)
	return Y_VIEW;
      // u-view: 3π/4, 7π/4
      else if (abs(fmod(this->Rot()-3*3.14/4,3.14)) < 0.2)
	return U_VIEW;
      // w-view: π/4, 5π/4
      else if (abs(fmod(this->Rot()-3.14/4,3.14)) < 0.2)
	return W_VIEW;
      return INIT;
    }

    //--------------------------------------------------------------------------------

    Plane::Plane() :
      fSSD(0)
    {
    }

    //--------------------------------------------------------------------------------

    SSDStation::SSDStation() :
      fName(""), fPos(-1e6,-1e6,-1e6), fDz(-99999), fWidth(0.), fHeight(0.), fPlane(0)
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
      geoFile.open(fGDMLFile.c_str());
      if (!geoFile.is_open()) {
	throw cet::exception("GeometryFileLoad")
	  << "cannot find GDML file " << fGDMLFile << " bail ungracefully\n"
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

      //fGeoManager->Import(fname.c_str());
      TGeoManager::LockDefaultUnits(0);
      TGeoManager::SetDefaultUnits(TGeoManager::EDefaultUnits::kG4Units);
      TGeoManager::LockDefaultUnits(1);
      TGeoManager::Import(fGDMLFile.c_str());

      fGeoManager = gGeoManager;

      mf::LogWarning("LoadNewGeometry") << "loaded new geometry files\n";

      fGeoManager->SetVerboseLevel(old_verbosity);

      const TGeoNode* world_n = (TGeoNode*)fGeoManager->GetTopNode();

      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();

      TGeoBBox* world_box = (TGeoBBox*)world_v->GetShape();      

      fWorldHeight = world_box->GetDY();
      fWorldWidth  = world_box->GetDX();
      fWorldLength = world_box->GetDZ();

      ExtractMagnetInfo(world_v);
      mf::LogInfo("ExtractGeometry") << "extracted magnet geometry \n";

      ExtractPMTInfo(world_v);
      mf::LogInfo("ExtractGeometry") << "extracted PMT info \n";

      for ( int i = Trigger ; i < NDetectors ; i ++ ){
	ExtractDetectorInfo(i, world_n);
	if ( fDetectorLoad[i] == true ){
	  mf::LogInfo("ExtractGeometry") << "extracted "
					 << DetInfo::Name(DetectorType(i)) << " geometry \n";
	}
      }

      return true;
    }

    //--------------------------------------------------------------------------------
    std::vector<std::pair<double, double> > Geometry::ReadMatrix(TGDMLMatrix *matrix)
    {
      int rows = matrix->GetRows();
      std::vector<std::pair<double, double> > fQEVector;
      for(int i=0; i<rows; i++)
	{
	  double w,q;
	  w = matrix->Get(i,0);
	  q = matrix->Get(i,1);
	  //nm->mm, percent->probability
	  fQEVector.push_back(std::make_pair(w*1e-6,q*1e-2));
	}
      sort(fQEVector.begin(),fQEVector.end());

      return fQEVector;
    }
		
    //---------------------------------------------------------------------
    bool Geometry::LocalToWorld(int station, int plane, int sensor, int strip, 
				double pos[3], double newpos[3])
    {
      
      const TGeoNode* world_n = (TGeoNode*)fGeoManager->GetTopNode();
      int nnodes = world_n->GetNodes()->GetEntries();
      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();

      std::vector<std::string> nodeName;

      std::string sString = "ssdStation";
      std::string ssubString = "ssdsensor";
      std::string schanString = "ssd_chan";
      std::string ssubSubString = "ssdsensordouble";
      std::string schanStringCompare = std::to_string(int(strip));
      std::string num;

      TGeoMatrix *stripM=0;
      TGeoMatrix *sensorM=0;
      TGeoMatrix *stationM=0;

      for (int i=0; i<nnodes; ++i) {
	std::string name = world_v->GetNode(i)->GetName();
        if (name.find(sString) != std::string::npos)
	  nodeName.push_back(name);
      }

      for (auto name : nodeName) {
	TGeoNode* st_n = (TGeoNode*)world_v->GetNode(name.c_str());
	TGeoVolume* st_v = (TGeoVolume*)st_n->GetVolume();

	int nsub = st_n->GetNodes()->GetEntries();
	for( int j=0; j<nsub; ++j){
	  std::string name = st_v->GetNode(j)->GetName();
	  if (name.find(ssubString) != std::string::npos){
	    TGeoNode* sensor_n = (TGeoNode*)st_v->GetNode(name.c_str());
	    TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
	    if(sensor_n->GetNodes()!=NULL){
	      int nchan = sensor_n->GetNodes()->GetEntries();
	      for( int k=0; k<nchan; ++k){
		std::string name2 = sensor_v->GetNode(k)->GetName();
		if(name2.find(schanString) != std::string::npos){
		  num = Form("%d_%d_%d_%d_",station,plane,sensor,(int)strip);
		  if(name2.find(num) != std::string::npos){
		    TGeoNode* strip_n = (TGeoNode*)sensor_v->GetNode(name.c_str());
		    stripM = strip_n->GetMatrix();
		    sensorM = sensor_n->GetMatrix();		    
		    stationM = st_n->GetMatrix();
		  }
		}
	      }
	    }
	  }
	}
      }

      if (!stripM || !sensorM || !stationM) {
	return false;
      }

      double tpos[3];
      stripM->LocalToMaster(pos,newpos);
      sensorM->LocalToMaster(newpos,tpos);
      stationM->LocalToMaster(tpos,newpos);

      return true;
    }
		
    //--------------------------------------------------------------------------------
    void Geometry::ExtractDetectorInfo(int i, const TGeoNode* world_n)
    {
      if ( i < 3 ){
	mf::LogInfo("LoadNewGeometry") << DetInfo::Name(DetectorType(i)) 
				       << " detector not in gdml yet. \n";
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

    void Geometry::ExtractPMTInfo(const TGeoVolume* world_v)
    {
      std::string PMT_name="PMT_H12700", QE_name="_QE", DN_name="_DarkNoise";

      TGDMLMatrix* qematrix = (TGDMLMatrix*)fGeoManager->GetGDMLMatrix((PMT_name+QE_name).c_str());
      if(qematrix==nullptr)std::cout<<"empty"<<std::endl;
      std::vector<std::pair<double, double> > qeV = ReadMatrix(qematrix);
      mf::LogInfo("ExtractGeometry") << "PMT QE is " << qeV.begin()->first <<" nm " <<qeV.begin()->second << " \n";

      double darkr = fGeoManager->GetProperty((PMT_name+DN_name).c_str());
      mf::LogInfo("ExtractGeometry") << "PMT dark rate is " << darkr << " Hz\n";


      TGeoNode* arich_n = (TGeoNode*)world_v->GetNode("ARICH_phys");
      TGeoVolume* arich_v = (TGeoVolume*)arich_n->GetVolume();

      int nsub = arich_n->GetNodes()->GetEntries();
      for( int j=0; j<nsub; ++j){
	std::string name = arich_v->GetNode(j)->GetName();
	if (name.find("PMT_phys") != std::string::npos){
	  emph::arich_util::PMT mpmt;
	  int num = mpmt.findBlockNumberFromName(name);
	  if(num<0)continue;
	  mpmt.SetPMTnum(num);
	  mpmt.SetName(name);
	  mpmt.SetQE(qeV);
	  mpmt.SetDarkRate(darkr);

	  fNPMTs++;
	  fPMT.push_back(mpmt);
	}
      }

    }

    //--------------------------------------------------------------------------------

    void Geometry::ExtractSSDInfo(const TGeoNode* world_n)
    {
      int nnodes = world_n->GetNodes()->GetEntries();
      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();

      std::vector<std::string> nodeName;

      std::string sString = "ssdStation";
      std::string ssubString = "ssdsensor";
      std::string schanString = "ssd_chan";

      for (int i=0; i<nnodes; ++i) {
	std::string name = world_v->GetNode(i)->GetName();
	if (name.find(sString) != std::string::npos)
	  nodeName.push_back(name);
      }

      fNSSDStations = (int)nodeName.size();
      fNSSDs = 0;

      double angle;
      bool flip;

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
	st.SetGeoMatrix(st_n->GetMatrix());

	// now add individual SSDs to the station
	int nsub = st_n->GetNodes()->GetEntries();
	int nplanes=-1;
	int iSt, iPl, iSe;
	Plane* plane=0;
	for( int j=0; j<nsub; ++j){
	  std::string name = st_v->GetNode(j)->GetName();
	  if (name.find(ssubString) != std::string::npos){
	    sscanf(name.c_str(),"ssdsensor_%d_%d_%d_vol",&iSt,&iPl,&iSe);
	    if (nplanes < iPl) { // new plane
	      st.AddPlane(Plane(*plane));
	      plane = new Plane();
	      nplanes++;
	    }
	    Detector sensor;
	    TGeoNode* sensor_n = (TGeoNode*)st_v->GetNode(name.c_str());
	    TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
	    TGeoBBox* sensor_box = (TGeoBBox*)sensor_v->GetShape();
	    
	    sensor.SetName(name);
	    sensor.SetDz(sensor_box->GetDZ());
	    sensor.SetGeoMatrix(sensor_n->GetMatrix());
	    sensor.SetPos(sensor_n->GetMatrix()->GetTranslation());
	    angle = acos(sensor_n->GetMatrix()->GetRotationMatrix()[0]);
	    if(sensor_n->GetMatrix()->GetRotationMatrix()[1]<-0.1)angle = 2*TMath::Pi()-angle;
	    sensor.SetRot(angle);
	    const Double_t* rotation_matrix = sensor_n->GetMatrix()->GetRotationMatrix();
	    if(*(rotation_matrix+8)>0)flip=false;
	    else flip=true;
	    sensor.SetFlip(flip);
	    sensor.SetWidth(2*sensor_box->GetDX());
	    sensor.SetHeight(2*sensor_box->GetDY());

	    // now add channels to each SSD sensor
	    if(sensor_n->GetNodes()!=NULL){
	      int nchan = sensor_n->GetNodes()->GetEntries();
	      for( int k=0; k<nchan; ++k){
		std::string name = sensor_v->GetNode(k)->GetName();
		if(name.find(schanString) != std::string::npos){
		  Strip strip;
		  TGeoNode* strip_n = (TGeoNode*)sensor_v->GetNode(name.c_str());
		  TGeoVolume* strip_v = (TGeoVolume*)strip_n->GetVolume();
		  TGeoBBox* strip_box = (TGeoBBox*)strip_v->GetShape();
		  
		  strip.SetName(name);
		  strip.SetDw(2*strip_box->GetDY());
		  strip.SetPos(strip_n->GetMatrix()->GetTranslation());
		  strip.SetGeoMatrix(strip_n->GetMatrix());
		  sensor.AddStrip(strip);
		}
	      }
	    }
	    plane->AddSSD(sensor);
	    fNSSDs++;
	    
	  }
	}
	
	fSSDStation.push_back(st);
      }
      
    }
    
    emph::arich_util::PMT Geometry::FindPMTByName(std::string name)
    {
      for(int i=0; i<fNPMTs; i++){
	if(fPMT[i].Name()==name)return fPMT[i];
      }
      mf::LogWarning("LoadNewGeometry") << "Cannot Find PMT " << name << "\n" << "Using PMT No. 0 as an instance \n";
      return fPMT[0];
    }
    
  } // end namespace geo
} // end namespace emph
