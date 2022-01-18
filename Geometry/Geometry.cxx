////////////////////////////////////////////////////////////////////////
/// \file    Geometry.cxx
/// \brief
/// \version 
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
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

#include "Geometry.h"

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
    
  }
  
  //--------------------------------------------------------------------------------
  
  Geometry::Geometry(std::string fname) :
    fGDMLFile(fname), fSSDStation(0)
  {
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
    
    if (fname == fGDMLFile) return true;
    
    fGDMLFile = fname;
    return this->LoadGDMLFile();
  }
  
  //--------------------------------------------------------------------------------
  
  bool Geometry::LoadGDMLFile()
  {
    //    cet::search_path sp("FW_SEARCH_PATH");
    
    //    std::string fullPathGDMLFile;
    
    /*
    if (!sp.find_file(fGDMLFile,fullPathGDMLFile) ) {
      throw cet::exception("GeometryFileLoad")
        << "cannot find GDML file " << fGDMLFile << " bail ungracefully\n"
        << __FILE__ << ":" << __LINE__ << "\n";
    }
    */
    mf::LogWarning("LoadNewGeometry") << "loading new geometry files\n"
				      << fGDMLFile << "\n";
    
    int old_verbosity = gGeoManager->GetVerboseLevel();
    
    // TGeoManager is too verbose when loading geometry.
    // Make it quiet.
    gGeoManager->SetVerboseLevel(0);
    
    TGeoManager::Import(fGDMLFile.c_str());
    
    gGeoManager->SetVerboseLevel(old_verbosity);

    const TGeoNode* world_n = (TGeoNode*)gGeoManager->GetTopNode();
    const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();
    
    ExtractMagnetInfo(world_v);
    ExtractRICHInfo(world_v);
    ExtractSSDInfo(world_n);

    return true;
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
    fMagnetDSZPos = zcenter-dz;
    
  }
  
  //--------------------------------------------------------------------------------
  
  void Geometry::ExtractRICHInfo(const TGeoVolume* world_v)
  {
    TGeoNode* rich_n = (TGeoNode*)world_v->GetNode("rich_phys");
    TGeoVolume* rich_v = (TGeoVolume*)rich_n->GetVolume();
    TGeoBBox* rich_box = (TGeoBBox*)rich_v->GetShape();
    
    double zcenter = rich_n->GetMatrix()->GetTranslation()[2];
    double dz = rich_box->GetDZ();
    
    fRICHUSZPos = zcenter-dz;
    fRICHDSZPos = zcenter-dz;
    
  }

  //--------------------------------------------------------------------------------
  
  void Geometry::ExtractSSDInfo(const TGeoNode* world_n)
  {
    int nnodes = world_n->GetNodes()->GetEntries();

    const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();
    
    std::vector<std::string> nodeName;
    
    std::string sString = "ssdStation";
    
    for (int i=0; i<nnodes; ++i) {
      std::string name = world_v->GetNode(i)->GetName();
      if (name.find(sString) != std::string::npos)
	nodeName.push_back(name);
    }

    fNSSDStations = (int)nodeName.size();
    
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
    }
    
  }

} // end namespace geo
} // end namespace emph
