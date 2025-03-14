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

    void Detector::LocalToMother(double x1[3], double x2[3]) const
    {
      double tx[3];
      if (fName.find("ssd") != std::string::npos) { // include mount position if this is a SSD
	fGeoMatrix->LocalToMaster(x1,tx);
	fGeoMatrixMount->LocalToMaster(tx,x2);
      }
      else
	fGeoMatrix->LocalToMaster(x1,x2);
    }

    //--------------------------------------------------------------------------------
    sensorView Detector::View() const
    {
     auto pi = TMath::Pi();
     float ang = this->Rot();

     // x-view: π/2, 3π/2
     if ( abs(sin(ang-pi/2)) < 0.2)
         return X_VIEW;
     // y-view: 0,π
     else if (abs(sin(ang)) < 0.2)
         return Y_VIEW;

     // Flipped diagonal sensors have a Rot angle π/2 out-of-phase
     // so we needto take the flip into account when finding the angle
     // Adding earlier messes up x,y double sensor planes.
     ang += pi/2*this->IsFlip();
     // u-view: 3π/4, 7π/4
     if (abs(sin(ang-3*pi/4)) < 0.2)
         return W_VIEW;
     // w-view: π/4, 5π/4
     else if (abs(sin(ang-pi/4)) < 0.2)
         return U_VIEW;
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

    Target::Target() :
      fPos(-1e6,-1e6,-1e6), fDPos(0.,0.,0.), fDensity(0.)
    {
      fFrac.clear();
      fA.clear();
      fZ.clear();
    }

    //--------------------------------------------------------------------------------

    Geometry::Geometry() :
      fIsLoaded(false), fGDMLFile("")
    {
      fNSSDStations = 0;
      fNSSDPlanes = 0;
      fNSSDs = 0;
      fTarget = 0;
      fGeoManager = 0;
      fSSDSensorMap.clear();
      for (int i=0; i<3; ++i) {
	fMagnetUSPos[i] = -1e6;
	fMagnetDSPos[i] = -1e6;
      }
      fTargetUSZPos = -1e7;
      fTargetDSZPos = -1e7;
      fGeoManager = new TGeoManager("EMPHGeometry","EMPHATIC Geometry Manager");

    }

    //--------------------------------------------------------------------------------

    Geometry::Geometry(std::string fname) :
      fIsLoaded(false), fGDMLFile(fname), fSSDStation(0)
    {
      fGeoManager = new TGeoManager("EMPHGeometry","EMPHATIC Geometry Manager");

      for ( int i = Trigger ; i < NDetectors ; i ++ ) fDetectorLoad[i] = false;
      fMagnetLoad = false;
      fSSDSensorMap.clear();
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
      
      if (fIsLoaded && (fname == fGDMLFile)) // we've already opened this file, nothing to do
	return true;
      else if (fIsLoaded) { // need to load new geometry
	fGDMLFile = fname;
	delete fGeoManager;
	fIsLoaded = false;
	fMagnetLoad = false;
	fSSDSensorMap.clear();
	fGeoManager = new TGeoManager("EMPHGeometry","EMPHATIC Geometry Manager");
      }
      
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

      mf::LogWarning("LoadNewGeometry") << "loading new geometry file "
					<< fGDMLFile << "\n";

      //      if (fGeoManager) delete fGeoManager;

      int old_verbosity = gGeoManager->GetVerboseLevel();

      // TGeoManager is too verbose when loading geometry.
      // Make it quiet.
      fGeoManager->SetVerboseLevel(0);
      fGeoManager->LockDefaultUnits(0);
      fGeoManager->SetDefaultUnits(TGeoManager::EDefaultUnits::kG4Units);
      fGeoManager->LockDefaultUnits(1);
      fGeoManager = fGeoManager->Import(fGDMLFile.c_str());

      //      fGeoManager->Import(fname.c_str());
      /*
      TGeoManager::LockDefaultUnits(0);
      TGeoManager::SetDefaultUnits(TGeoManager::EDefaultUnits::kG4Units);
      TGeoManager::LockDefaultUnits(1);
      TGeoManager::Import(fGDMLFile.c_str());
      */
      //      fGeoManager = new TGeoManager( *gGeoManager);

      //      fGeoManager->SetVerboseLevel(old_verbosity);

      const TGeoNode* world_n = (TGeoNode*)fGeoManager->GetTopNode();

      const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();

      TGeoBBox* world_box = (TGeoBBox*)world_v->GetShape();      

      fWorldHeight = world_box->GetDY();
      fWorldWidth  = world_box->GetDX();
      fWorldLength = world_box->GetDZ();

      ExtractTargetInfo(world_v);
      mf::LogInfo("ExtrackGeometry") << "extracted target geometry \n";

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

      mf::LogWarning("LoadNewGeometry") << "loaded new geometry files\n";

      fIsLoaded = true;

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

    void Geometry::ExtractTargetInfo(const TGeoVolume* world_v)
    {
      fTargetUSZPos = -1e7;
      fTargetDSZPos = -1e7;
      
      TGeoNode* target_n = (TGeoNode*)world_v->GetNode("target_phys");
      if ( target_n == nullptr ){
	mf::LogWarning("ExtractTargetInfo") 
	  << " target not found in gdml. Check that this is ok! \n";
	return;
      }
      
      TGeoVolume* target_v = (TGeoVolume*)target_n->GetVolume();
      TGeoBBox* target_box = (TGeoBBox*)target_v->GetShape();

      fTarget = new Target();
      fTarget->SetPos(target_n->GetMatrix()->GetTranslation());
      TVector3 dPos(target_box->GetDX(),target_box->GetDY(),target_box->GetDZ());
      fTarget->SetDPos(dPos);

      fTargetUSZPos = fTarget->Pos()[2]-fTarget->DPos()[2];
      fTargetDSZPos = fTarget->Pos()[2]+fTarget->DPos()[2];

      TGeoMaterial* target_mat = target_v->GetMaterial();
      int nel = target_mat->GetNelements();
      for (int i=0; i<nel; ++i) {
	TGeoElement* target_el = target_mat->GetElement(i);
	fTarget->AddElement(target_el->GetName(),
			    1., // JMP: note, the weight needs to be fixed...
			    target_el->A(), target_el->Z());
      }
      
    }
      
    //--------------------------------------------------------------------------------

    void Geometry::ExtractMagnetInfo(const TGeoVolume* world_v)
    {
      TGeoNode* magnet_n = (TGeoNode*)world_v->GetNode("magnet_phys");
		if ( magnet_n == nullptr ){
			mf::LogWarning("LoadNewGeometry") 
				<< " magnet not found in gdml. \n"
				<< "check your spelling. \n";
			return;
		}

      TGeoVolume* magnet_v = (TGeoVolume*)magnet_n->GetVolume();
      TGeoBBox* magnet_box = (TGeoBBox*)magnet_v->GetShape();

      double xcenter = magnet_n->GetMatrix()->GetTranslation()[0];
      double ycenter = magnet_n->GetMatrix()->GetTranslation()[1];
      double zcenter = magnet_n->GetMatrix()->GetTranslation()[2];
      double dz = magnet_box->GetDZ();

      fMagnetUSPos[2] = zcenter-dz;
      fMagnetDSPos[2] = zcenter+dz;
      fMagnetUSPos[0] = xcenter;
      fMagnetDSPos[0] = xcenter;
      fMagnetUSPos[1] = ycenter;
      fMagnetDSPos[1] = ycenter;
      fMagnetLoad = true;

    }

    //--------------------------------------------------------------------------------

    void Geometry::ExtractPMTInfo(const TGeoVolume* world_v)
    {
      std::string PMT_name="PMT_H12700", QE_name="_QE", DN_name="_DarkNoise", CrossTalk_name = "_CrossTalk";

      TGDMLMatrix* qematrix = (TGDMLMatrix*)fGeoManager->GetGDMLMatrix((PMT_name+QE_name).c_str());
      if(qematrix==nullptr)std::cout<<"empty"<<std::endl;
      std::vector<std::pair<double, double> > qeV = ReadMatrix(qematrix);
      mf::LogInfo("ExtractGeometry") << "PMT QE is " << qeV.begin()->first <<" nm " <<qeV.begin()->second << " \n";

      double darkr = fGeoManager->GetProperty((PMT_name+DN_name).c_str());
      mf::LogInfo("ExtractGeometry") << "PMT dark rate is " << darkr << " Hz\n";

      double XTalk = fGeoManager->GetProperty((PMT_name+CrossTalk_name).c_str());
      mf::LogInfo("ExtractGeometry") << "PMT Cross Talk is " << XTalk*100 << "%" ;

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
	  mpmt.SetCrossTalk(XTalk);
	  mpmt.SetTriggerWin(5.);   // 5 ns of trigger window  
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
      std::string smountString = "ssd_mount";
      std::string ssubString = "ssdsensor";
      std::string schanString = "ssd_chan";

      for (int i=0; i<nnodes; ++i) {
	std::string name = world_v->GetNode(i)->GetName();
	if (name.find(sString) != std::string::npos)
	  nodeName.push_back(name);
      }

      fNSSDStations = (int)nodeName.size();
      fSSDStation.resize(nodeName.size());
      fNSSDPlanes = 0;
      fNSSDs = 0;

      double angle;
      bool flip;

      for (auto name : nodeName) {

	SSDStation st;
	TGeoNode* st_n = (TGeoNode*)world_v->GetNode(name.c_str());
	TGeoVolume* st_v = (TGeoVolume*)st_n->GetVolume();
	TGeoBBox* st_box = (TGeoBBox*)st_v->GetShape();

	// get the right sensor id
	int stId;
	sscanf(name.c_str(),"ssdStation%d",&stId);
	
	// first add basic info about the SSD station
	st.SetName(name);
	st.SetId(stId);
	st.SetDz(st_box->GetDZ());
	st.SetPos(st_n->GetMatrix()->GetTranslation());
	st.SetWidth(2*st_box->GetDX());
	st.SetHeight(2*st_box->GetDY());
	st.SetGeoMatrix(st_n->GetMatrix());

	// now add individual SSDs to the station
	// loop over SSD mounts and sensors to create planes
	int nmounts = st_n->GetNodes()->GetEntries();
	int iSt, iPl, iSe;
	int nplanes=0;
	Plane* plane = new Plane();
	for( int j=0; j<nmounts; ++j){
	  std::string mountname = st_v->GetNode(j)->GetName();
	  if (mountname.find(smountString) == std::string::npos) continue;
	  TGeoNode* mount_n = (TGeoNode*)st_v->GetNode(mountname.c_str());
	  TGeoVolume* mount_v = (TGeoVolume*)mount_n->GetVolume();
	  int nsensors = mount_n->GetNodes()->GetEntries();
	  for (int kk=0; kk<nsensors; ++kk) {
	    std::string sname = mount_v->GetNode(kk)->GetName();
	    if (sname.find(ssubString) != std::string::npos){	
	      sscanf(sname.c_str(),"ssdsensor_%d_%d_%d_phys",&iSt,&iPl,&iSe);
	      if (nplanes < iPl) { // new plane
		st.AddPlane(Plane(*plane));
		plane = new Plane();
		fNSSDPlanes++;
		nplanes++;
	      }
	      Detector sensor;
	      TGeoNode* sensor_n = (TGeoNode*)mount_v->GetNode(sname.c_str());
	      TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
	      TGeoBBox* sensor_box = (TGeoBBox*)sensor_v->GetShape();
	      
	      sensor.SetName(sname);
	      sensor.SetDz(sensor_box->GetDZ());
	      sensor.SetGeoMatrix(sensor_n->GetMatrix());
	      sensor.SetGeoMatrixMount(mount_n->GetMatrix());
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
		  std::string namestr = sensor_v->GetNode(k)->GetName();
		  if(namestr.find(schanString) != std::string::npos){
		    Strip strip;
		    TGeoNode* strip_n = (TGeoNode*)sensor_v->GetNode(namestr.c_str());
		    TGeoVolume* strip_v = (TGeoVolume*)strip_n->GetVolume();
		    TGeoBBox* strip_box = (TGeoBBox*)strip_v->GetShape();
		    
		    strip.SetName(namestr);
		    strip.SetDw(2*strip_box->GetDY());
		    strip.SetPos(strip_n->GetMatrix()->GetTranslation());
		    strip.SetGeoMatrix(strip_n->GetMatrix());
		    sensor.AddStrip(strip);
		  }
		}
	      }
	      plane->AddSSD(sensor);
	      fSSDSensorMap[fNSSDs] = &sensor;
	      fNSSDs++;	    
	    }
	  }
	}
	// don't forget to add the last plane!
	st.AddPlane(Plane(*plane));
	fNSSDPlanes++;

	fSSDStation[st.Id()] = st;
      }
      
    }

    //------------------------------------------------------------
    
    int Geometry::GetSSDId(int station, int plane, int sensor) const
    {
      return fSSDStation[station].GetPlane(plane)->SSD(sensor)->Id();
    }
    
    //------------------------------------------------------------

    emph::arich_util::PMT Geometry::FindPMTByName(std::string name)
    {
      for(int i=0; i<fNPMTs; i++){
	if(fPMT[i].Name()==name)return fPMT[i];
      }
      mf::LogWarning("LoadNewGeometry") << "Cannot Find PMT " << name << "\n" << "Using PMT No. 0 as an instance \n";
      return fPMT[0];
    }
   
 //-----------------------------------------------------------------
    emph::arich_util::PMT Geometry::FindPMTByBlockNumber(int number)
   {
    for(int i=0; i<fNPMTs; i++){
        if(fPMT[i].PMTnum() ==number)return fPMT[i];
    }
    mf::LogWarning("LoadNewGeometry") << "Cannot Find PMT " << number << "\n" << "Using PMT No. 0 as an instance \n";
    return fPMT[0];
   }

 
  } // end namespace geo
} // end namespace emph
