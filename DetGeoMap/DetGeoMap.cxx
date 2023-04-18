////////////////////////////////////////////////////////////////////////
/// \brief   DetGeoMap class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "DetGeoMap/DetGeoMap.h"
#include "Geometry/service/GeometryService.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoNavigator.h"

namespace emph {
  namespace dgmap {
  
    //----------------------------------------------------------------------
    
    DetGeoMap::DetGeoMap()
    {
      fSSDStationMinZ.clear();     
      fSSDStationMaxZ.clear();     
      art::ServiceHandle<emph::geo::GeometryService> geo;
    }
  
    //----------------------------------------------------------------------
    
    bool DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
    {
      
      int station = cl.Station();
      int sensor = cl.Sensor();
      //      rb::planeView view = cl.View();
      double strip = cl.WgtAvgStrip();
      double pitch = 0.06;

      try {
	art::ServiceHandle<emph::geo::GeometryService> geo;
	auto geom = geo->Geo();
	
	const emph::geo::SSDStation &st = geom->GetSSDStation(station);
	const emph::geo::Detector &sd = st.GetSSD(sensor);
	
	double x0[3];
	double x1[3];
	x0[2] = x1[2] = sd.Pos()[2] + st.Pos()[2];
	double strippos = strip*pitch - sd.Height()/2;
	double cosrot = cos(sd.Rot());
	double sinrot = sin(sd.Rot());
	double tx0[2], tx1[2];

	tx0[0] = -sd.Width()/2;
	tx1[0] = sd.Width()/2;
	tx0[1] = strippos;
	tx1[1] = strippos;

	x0[0] = -tx0[0]*cosrot + tx0[1]*sinrot + sd.Pos()[0];
	x0[1] = tx0[0]*sinrot + tx0[1]*cosrot + sd.Pos()[1];

	x1[0] = -tx1[0]*cosrot + tx1[1]*sinrot + sd.Pos()[0];
	x1[1] = tx1[0]*sinrot + tx1[1]*cosrot + sd.Pos()[1];

	/*
	tx0[0] = -sd.Width()/2 + sd.Pos()[0];
	tx1[0] = sd.Width()/2 + sd.Pos()[0];
	tx0[1] = strippos + sd.Pos()[1];
	tx1[1] = strippos + sd.Pos()[1];

	x0[0] = tx0[0]*cosrot - tx0[1]*sinrot; 
	x0[1] = tx0[0]*sinrot + tx0[1]*cosrot; 

	x1[0] = tx1[0]*cosrot - tx1[1]*sinrot; 
	x1[1] = tx1[0]*sinrot + tx1[1]*cosrot; 
	*/
 
	ls.SetX0(x0);
	ls.SetX1(x1);	  
      }
      catch(...) {
	return false;
      }
      
      return true;

    }

    //----------------------------------------------------------------------
    void DetGeoMap::Reset()
    {
      fSSDStationMinZ.clear();
      fSSDStationMaxZ.clear();

      art::ServiceHandle<emph::geo::GeometryService> geo;
      auto geom = geo->Geo();
      std::cout << geom << std::endl;

      double z;
      double dz;
      for (int i=0; i<geom->NSSDStations(); ++i) {
	z = geom->GetSSDStation(i).Pos()[2];
	dz = geom->GetSSDStation(i).Dz();
	std::cout << "ssd station " << i << " z = " << z << " +/- " << dz 
		  << std::endl;
	fSSDStationMinZ.push_back(z-dz);
	fSSDStationMaxZ.push_back(z+dz);
      }
      
    }

    //----------------------------------------------------------------------

    rawdata::SSDRawDigit* DetGeoMap::SSDSimHitToRawDigit(const sim::SSDHit& ssdhit)
    {
      rawdata::SSDRawDigit* dig = 0;
      int32_t station=0;
      int32_t module=0; 
      int32_t chip=0; 
      int32_t set=0; 
      int32_t strip=0; 
      int32_t t=0; 
      int32_t adc=0; 
      int32_t trig=0; 

      art::ServiceHandle<emph::geo::GeometryService> geo;
      auto geom = geo->Geo()->ROOTGeoManager();

      TGeoNavigator* nav = new TGeoNavigator(geom);
      std::cout << "here!" << nav << std::endl;

      double mHitPos[3]; // master hit position
      //      double lHitPos[3]; // local hit position on the sensor
      mHitPos[0] = ssdhit.GetX();
      mHitPos[1] = ssdhit.GetY();
      mHitPos[2] = ssdhit.GetZ();
      //      double dE = ssdhit.GetDE();

      std::cout << "nav level = " << nav->GetLevel() << std::endl;
      std::cout << nav->GetPath() << std::endl;
      //      TGeoNode* node2 = nav->FindNode(mHitPos[0],mHitPos[1],mHitPos[2]);
      //      std::cout << node2 << std::endl;

      TGeoNode* node = geom->FindNode(mHitPos[0],mHitPos[1],mHitPos[2]);
      //      node->MasterToLocal(mHitPos,lHitPos);
      std::string nodeName = node->GetName();
      //      std::string node2Name = node2->GetName();
      //      std::cout << nodeName << ", " << node2Name << std::endl;

      int chan;
      sscanf(nodeName.c_str(),"ssd_chan_%d_vol",&chan);
      std::cout << "Channel number: " << chan << std::endl;

      //      std::cout << nav->GetMother()->GetName() << std::endl;

      //      std::cout << nodeName << " has " << node->CountDaughters() 
      //		<< " daughters" << std::endl;
      
      //      TGeoVolume* vol = node->GetVolume();

      //      vol->Print();
      //      std::string volName = vol->GetName(); 
      //      std::cout << "(" << hitX << "," << hitY << "," << hitZ << "), dE= " 
      //		<< dE << std::endl;
      //      std::cout << volName << ", " << nodeName << std::endl;

      //      auto nav = geom->GetCurrentNavigator();
      //      std::cout << "nav = " << nav << std::endl;
      
      //      nav->MasterToLocal(mHitPos,lHitPos);
      /*
      std::cout << "(" << mHitPos[0] << "," << mHitPos[1] << ","
		<< mHitPos[2] << ")-->(" << lHitPos[0] << "," 
		<< lHitPos[1] << "," << lHitPos[2] << ")" << std::endl;
      */
      
      // work some magic and set the values above
      size_t iStation=0;
      for (; iStation<fSSDStationMinZ.size(); ++iStation) {
    	if ((mHitPos[2] > fSSDStationMinZ[iStation]) &&
	    (mHitPos[2] < fSSDStationMaxZ[iStation])) break;
      }
      if (iStation == fSSDStationMinZ.size()) {
	// this should never happen.  It means the hit Z position is outside of a 
	// SSD station volume.  Return a NULL pointer.
	return dig;
      }
     
      station = int32_t(iStation);
      //      std::cout << "station = " << station << std::endl;

      // create raw digit to return
      module = chan/640;
      chip = (chan - 640*module)/128;
      strip = chan%128;
      std::cout << "(station, module, chip, set, strip) = ("
		<< station << ", "
		<< module << ", " 
		<< chip << ", " 
		<< set << ", " 
		<< strip << ")" << std::endl;
      dig = new rawdata::SSDRawDigit(station, module, chip, set,
				     strip, t, adc, trig);

      return dig;

    }

  } // end namespace dgmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
