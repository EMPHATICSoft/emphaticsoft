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
    
    DetGeoMap::DetGeoMap():
    geo(art::ServiceHandle<emph::geo::GeometryService>())
    {
    }
  
    //----------------------------------------------------------------------
    
    bool DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
    {
      
      int station = cl.Station();
      int sensor = cl.Sensor();
      int plane = cl.Plane();
      int hilo = cl.HiLo();
      double strip = cl.WgtAvgStrip();
      double pitch = 0.06;
      
      try {
	auto geom = geo->Geo();
	
	const emph::geo::SSDStation &st = geom->GetSSDStation(station);
	const emph::geo::Detector &sd = st.GetSSD(sensor);

        //old way of getting line segments
        //uncomment if you want to compare
        /*
	const emph::geo::Strip &gstrip = sd.GetStrip(int(strip));

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

	tx0[0] = -sd.Width()/2;
	tx1[0] = sd.Width()/2;
	tx0[1] = strippos;
	tx1[1] = strippos;

	x0[0] = -tx0[0]*cosrot + tx0[1]*sinrot + sd.Pos()[0];
	x0[1] = tx0[0]*sinrot + tx0[1]*cosrot + sd.Pos()[1];

	x1[0] = -tx1[0]*cosrot + tx1[1]*sinrot + sd.Pos()[0];
	x1[1] = tx1[0]*sinrot + tx1[1]*cosrot + sd.Pos()[1];
        */

        double r_chan0[3];   double r_chan1[3]; 
        double r_sens0[3];   double r_sens1[3];
	double r_mount0[3];  double r_mount1[3];
        double r_stn0[3];    double r_stn1[3];
        double r_world0[3];  double r_world1[3];

        r_chan0[0] = -sd.Width()/2; //tx0[0];
        r_chan0[1] = (strip - floor(strip))*pitch; //segment may not be centered on a strip
        r_chan0[2] = 0.;
        r_chan1[0] = sd.Width()/2; //tx1[0]; 
        r_chan1[1] = (strip - floor(strip))*pitch; 
        r_chan1[2] = r_chan0[2];

        if(sd.IsFlip()){
          r_chan0[0] *= -1.;
          r_chan1[0] *= -1.;    
        }

        auto geoMgr = geom->ROOTGeoManager();

        const TGeoNode* world_n = (TGeoNode*)geoMgr->GetTopNode();
        int nnodes = world_n->GetNodes()->GetEntries();
        const TGeoVolume* world_v = (TGeoVolume*)world_n->GetVolume();

        std::vector<std::string> nodeName;

        std::string sStn = "ssdStation";
        std::string sMount = "ssd_mount";
        std::string sSens = "ssdsensor";
        std::string sChan = "ssd_chan";

        std::string num;

        TGeoMatrix *l1;
        TGeoMatrix *l2;
        TGeoMatrix *l3;
	TGeoMatrix *l4;

        bool foundStrip = false;

        for (int i=0; i<nnodes; ++i) {
        std::string name = world_v->GetNode(i)->GetName();
        if (name.find(sStn) != std::string::npos)
           nodeName.push_back(name);
        }
        for (auto name : nodeName) {
            TGeoNode* st_n = (TGeoNode*)world_v->GetNode(name.c_str());
            TGeoVolume* st_v = (TGeoVolume*)st_n->GetVolume();
	    l4 = st_n->GetMatrix();

            int nsub = st_n->GetNodes()->GetEntries();
            for( int j=0; j<nsub; ++j){
               std::string name = st_v->GetNode(j)->GetName();
	       if (name.find(sMount) != std::string::npos){
		  TGeoNode* mount_n = (TGeoNode*)st_v->GetNode(name.c_str());
                  TGeoVolume* mount_v = (TGeoVolume*)mount_n->GetVolume();
		  l3 = mount_n->GetMatrix();
	          int nmount = mount_n->GetNodes()->GetEntries();	
	           for( int k=0; k<nmount; ++k){	
	              std::string name = mount_v->GetNode(k)->GetName();
                      if (name.find(sSens) != std::string::npos){
                         TGeoNode* sensor_n = (TGeoNode*)mount_v->GetNode(name.c_str());
            	         TGeoVolume* sensor_v = (TGeoVolume*)sensor_n->GetVolume();
             	         l2 = sensor_n->GetMatrix();
                    	 if (sensor_n->GetNodes()!=NULL){
                       	    int nchan = sensor_n->GetNodes()->GetEntries();
                       	    for( int m=0; m<nchan; ++m){
                               std::string name = sensor_v->GetNode(m)->GetName();
                               if (name.find(sChan) != std::string::npos){
		                  num = Form("%d_%d_%d_%d_",station,plane,hilo,(int)strip);
			     	  if (name.find(num) != std::string::npos){
                                     TGeoNode* strip_n = (TGeoNode*)sensor_v->GetNode(name.c_str());
                                     TGeoVolume* strip_v = (TGeoVolume*)strip_n->GetVolume();
			             l1 = strip_n->GetMatrix();
			             foundStrip = true;
			             break;
			          }
		               }
                            } //strip loop
		         }
                      }
	           if (foundStrip) break;
                   } //sensor loop
	        } 
	      if (foundStrip) break;
              } //mount loop
           if (foundStrip) break;
         } //station loop

         if (foundStrip){
            l1->LocalToMaster(r_chan0,r_sens0); 
            l1->LocalToMaster(r_chan1,r_sens1);
            l2->LocalToMaster(r_sens0,r_mount0);
            l2->LocalToMaster(r_sens1,r_mount1);
            l3->LocalToMaster(r_mount0,r_stn0);
            l3->LocalToMaster(r_mount1,r_stn1);
            l4->LocalToMaster(r_stn0,r_world0);
            l4->LocalToMaster(r_stn1,r_world1);
         }
         
         //check
         //uncomment if you want to compare to old way
         /*
         for (size_t i=0; i<3; i++){
             std::cout<<"STRIP:::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_chan0[i]<<std::endl;
	     std::cout<<"SENSOR::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_sens0[i]<<std::endl;
	     std::cout<<"MOUNT:::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_mount0[i]<<std::endl;
             std::cout<<"STATION:::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_stn0[i]<<std::endl;
             std::cout<<"WORLD:::::x0_"<<i<<": "<<x0[i]<<"......r0_"<<i<<": "<<r_world0[i]<<std::endl;
	     std::cout<<"..............................."<<std::endl;
	     std::cout<<"STRIP:::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_chan1[i]<<std::endl;
             std::cout<<"SENSOR::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_sens1[i]<<std::endl;
             std::cout<<"MOUNT:::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_mount1[i]<<std::endl;
             std::cout<<"STATION:::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_stn1[i]<<std::endl;
             std::cout<<"WORLD:::::x1_"<<i<<": "<<x1[i]<<"......r1_"<<i<<": "<<r_world1[i]<<std::endl;
	     std::cout<<"..............................."<<std::endl;
         }
	 */

	 ls.SetX0(r_world0);
         ls.SetX1(r_world1);
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
