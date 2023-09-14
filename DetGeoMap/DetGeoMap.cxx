////////////////////////////////////////////////////////////////////////
/// \brief   DetGeoMap class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "DetGeoMap/DetGeoMap.h"
//#include "Geometry/Geometry.h"
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
//#include "TGeoMatrix.h"

//art::ServiceHandle<emph::geo::GeometryService> geom_;

namespace emph {
  namespace dgmap {
  
    //----------------------------------------------------------------------
    
    DetGeoMap::DetGeoMap():
    geo(art::ServiceHandle<emph::geo::GeometryService>())
    {
     //       art::ServiceHandle<emph::geo::GeometryService> geo;
    }
  
    //----------------------------------------------------------------------
    
    bool DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
    {
      
      int station = cl.Station();
      int sensor = cl.Sensor();
      int plane = cl.Plane();
      //std::cout<<"Plane in cluster is "<<plane<<std::endl;
      int hilo = cl.HiLo();
      //std::cout<<"sensor number: "<<sensor<<std::endl;
      double strip = cl.WgtAvgStrip();
      double pitch = 0.06;
      
      try {
//	art::ServiceHandle<emph::geo::GeometryService> geo;
	auto geom = geo->Geo();
	
	const emph::geo::SSDStation &st = geom->GetSSDStation(station);
	const emph::geo::Detector &sd = st.GetSSD(sensor);
	const emph::geo::Strip &gstrip = sd.GetStrip(int(strip));
	//std::cout<<"strip...is..."<<strip<<std::endl;

	//	auto sxyz = gstrip.Pos();
	//	std::cout << "Strip position = (" << sxyz[0] << "," << sxyz[1] << "," << sxyz[2] << ")" << std::endl;

	double x0[3];
	double x1[3];
	x0[2] = x1[2] = sd.Pos()[2] + st.Pos()[2];
	double strippos = gstrip.Pos()[1]; //sd.Height()/2 - strip*pitch;	
	double rot = sd.Rot();
	if(sd.IsFlip()){
		strippos = - strippos;
		rot = 2*TMath::Pi() - rot;
	}
	double cosrot = cos(rot);
	double sinrot = sin(rot);

	double tx0[2], tx1[2];

	//	std::cout << "Calculated position = (" << 
	tx0[0] = -sd.Width()/2;
	tx1[0] = sd.Width()/2;
	tx0[1] = strippos;
	tx1[1] = strippos;

	x0[0] = -tx0[0]*cosrot + tx0[1]*sinrot + sd.Pos()[0];
	x0[1] = tx0[0]*sinrot + tx0[1]*cosrot + sd.Pos()[1];

	x1[0] = -tx1[0]*cosrot + tx1[1]*sinrot + sd.Pos()[0];
	x1[1] = tx1[0]*sinrot + tx1[1]*cosrot + sd.Pos()[1];

       //check with geometry
        double r_chan0[3];   double r_chan1[3]; 
        double r_sens0[3];   double r_sens1[3];
        double r_stn0[3];    double r_stn1[3];
        double r_world0[3];  double r_world1[3];

        r_chan0[0] = tx0[0];
        r_chan0[1] = (strip - floor(strip))*pitch; //0;
        r_chan0[2] = 0.; //sd.Pos()[2] + st.Pos()[2];
        r_chan1[0] = tx1[0]; 
        r_chan1[1] = (strip - floor(strip))*pitch; //0; //gstrip.Pos()[1]; //tx1[1];
        r_chan1[2] = r_chan0[2];

        if(sd.IsFlip()){
          r_chan0[0] *= -1.;
          r_chan1[0] *= -1.;    
        }

        auto geoMgr = geom->ROOTGeoManager();

        const TGeoNode* world_n = (TGeoNode*)geoMgr->GetTopNode();
        //std::cout << "world_n = " << world_n <<std::endl;
        //std::cout << "world_n->GetNodes() = " << world_n->GetNodes() << std::endl;
        int nnodes = world_n->GetNodes()->GetEntries();
        const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();

        std::vector<std::string> nodeName;

        std::string sString = "ssdStation";
        std::string ssubString = "ssdsensor";
        std::string schanString = "ssd_chan";
        std::string ssubSubString = "ssdsensordouble";
        std::string schanStringCompare = std::to_string(int(strip));
        //std::cout<<"string compare is "<<schanStringCompare<<std::endl;
        std::string num;

//       TGeoIterator next(geom->GetTopVolume());
        TGeoMatrix *l1;
        TGeoMatrix *l2;
        TGeoMatrix *l3;

        //TGeoHMatrix *g1;
        //TGeoHMatrix *g2;
        //TGeoHMatrix *g3;

        bool yay = false;

        for (int i=0; i<nnodes; ++i) {
        std::string name = world_v->GetNode(i)->GetName();
        if (name.find(sString) != std::string::npos)
           nodeName.push_back(name);
        }
        for (auto name : nodeName) {
            TGeoNode* st_n = (TGeoNode*)world_v->GetNode(name.c_str());
            TGeoVolume* st_v = (TGeoVolume*)st_n->GetVolume();
	    l3 = st_n->GetMatrix();
	    //g3 = gGeoManager->GetCurrentMatrix();

            int nsub = st_n->GetNodes()->GetEntries();
            for( int j=0; j<nsub; ++j){
               std::string name = st_v->GetNode(j)->GetName();
               if (name.find(ssubString) != std::string::npos){
//                 if (name.find(ssubSubString) == std::string::npos){ //single sensor per plane
//		    num = Form("%d%d%d",station,sensor,0);
//		 }
//                 else{
//		     //int jj; int kk;
//		     if (sensor == 0) num = Form("%d%d%d",station,0,0);
//                     if (sensor == 1) num = Form("%d%d%d",station,0,1);
//                     if (sensor == 2) num = Form("%d%d%d",station,1,0);
 //                    if (sensor == 3) num = Form("%d%d%d",station,1,1);
//                     if (sensor == 4) num = Form("%d%d%d",station,2,0);
//                     if (sensor == 5) num = Form("%d%d%d",station,2,1);
//                 }
//                 if (name.find(num) != std::string::npos){
                    TGeoNode* sensor_n = (TGeoNode*)st_v->GetNode(name.c_str());
                    TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
                    l2 = sensor_n->GetMatrix();
	       	    //g2 = gGeoManager->GetCurrentMatrix();
                    if(sensor_n->GetNodes()!=NULL){
                       int nchan = sensor_n->GetNodes()->GetEntries();
                       for( int k=0; k<nchan; ++k){
                          std::string name = sensor_v->GetNode(k)->GetName();
                          if(name.find(schanString) != std::string::npos){
//			   std::cout<<"Strip name: "<<name<<std::endl;
			  //7_19_27_219_vol
//			  std::cout<<station<<"_"<<plane<<"_"<<hilo<<"_"<<(int)strip<<std::endl;
		             num = Form("%d_%d_%d_%d_",station,plane,hilo,(int)strip);
//		          std::cout<<"num: "<<std::endl;
			     if(name.find(num) != std::string::npos){
			        //std::cout<<"Strip name: "<<name<<std::endl;

                                TGeoNode* strip_n = (TGeoNode*)sensor_v->GetNode(name.c_str());
                                TGeoVolume* strip_v = (TGeoVolume*)strip_n->GetVolume();
			        l1 = strip_n->GetMatrix();
			        //g1 = gGeoManager->GetCurrentMatrix(); 
			        yay = true;
			        break;
                    //std::cout<<"sens? name: "<<name<<std::endl;
                 //   TGeoNode* sensor_n = (TGeoNode*)st_v->GetNode(name.c_str());
                 //   TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
//		    TGeoHMatrix *g3 = gGeoManager->GetCurrentMatrix();
		//	std::cout<<"MATRIX: "<<g3->GetRotationMatrix()<<std::endl;
	 	    //TGeoNode* n3 = gGeoManager->GetCurrentNode();
		    //std::cout<<"Node: "<<n3<<std::endl;
		    //std::cout<<"Strip node: "<<strip_n<<std::endl;
       //             TGeoNodeMatrix *l1 = new TGeoNodeMatrix(strip_v,strip_n->GetMatrix()); 

//		    g3->LocalToMaster(r_chan,r_sens);
//		    g3->LocalToMaster(r_chan1,r_sens1);

//		    l1->GetMatrix()->LocalToMaster(r_chan,r_sens);
//		    l1->GetMatrix()->LocalToMaster(r_chan1,r_sens1);
/*		    
		    TGeoNodeMatrix *l2 = sensor_n->GetMatrix();
		    l2->LocalToMaster(r_sens,r_stn);
                    l2->LocalToMaster(r_sens1,r_stn1);

		    TGeoNodeMatrix *l3 = st_n->GetMatrix();
	            l3->LocalToMaster(r_stn,r_world);
                    l3->LocalToMaster(r_stn1,r_world1);
*/

		    //next.GetPath(		    
/*
		    strip_n->LocalToMasterVect(r_chan,r_sens);
                    sensor_n->LocalToMasterVect(r_sens,r_stn);
                    st_n->LocalToMasterVect(r_stn,r_world);
		    strip_n->LocalToMasterVect(r_chan1,r_sens1);
                    sensor_n->LocalToMasterVect(r_sens1,r_stn1);
                    st_n->LocalToMasterVect(r_stn1,r_world1); 
*/
		          }
                       }
		    }
                 }
              }
//          if (yay) std::cout<<"in sensor loop"<<std::endl;
	  if (yay) break;
          } //sensor
//      if (yay) std::cout<<"in station loop"<<std::endl;
      if (yay) break;
      } //station
      if(yay){
      l1->LocalToMaster(r_chan0,r_sens0); 
      l1->LocalToMaster(r_chan1,r_sens1);
      l2->LocalToMaster(r_sens0,r_stn0);
      l2->LocalToMaster(r_sens1,r_stn1);
      l3->LocalToMaster(r_stn0,r_world0);
      l3->LocalToMaster(r_stn1,r_world1);

      }
      //check

      for (size_t i=0; i<3; i++){
          std::cout<<"STRIP:::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_chan0[i]<<std::endl;
	  std::cout<<"SENSOR::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_sens0[i]<<std::endl;
          std::cout<<"STATION:::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_stn0[i]<<std::endl;
          std::cout<<"WORLD:::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_world0[i]<<std::endl;
	  std::cout<<"..............................."<<std::endl;
	  std::cout<<"STRIP:::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_chan1[i]<<std::endl;
          std::cout<<"SENSOR::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_sens1[i]<<std::endl;
          std::cout<<"STATION:::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_stn1[i]<<std::endl;
          std::cout<<"WORLD:::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_world1[i]<<std::endl;
	  std::cout<<"..............................."<<std::endl;
      }

//      if (plane == 0 || plane == 1){
//	 r_world0[2] += 0.3;
//	 r_world1[2] += 0.3;
//      }

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
 
	ls.SetX0(r_world0);
        ls.SetX1(r_world1);
	//ls.SetX0(x0);
	//ls.SetX1(x1);	  
      }
      catch(...) {
	return false;
      }
      
      return true;

    }

    //----------------------------------------------------------------------
    void DetGeoMap::Reset()
    {

    }

    //----------------------------------------------------------------------

  } // end namespace dgmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
