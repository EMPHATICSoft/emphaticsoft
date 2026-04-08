////////////////////////////////////////////////////////////////////////
// \file    VertexFiller.cxx
// \brief   Class that does the work to extract reco'd vertexs from
//          the art event and add them to the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/VertexFiller.h"
#include "RecoBase/Vertex.h"
#include "RecoBase/Track.h"
#include "RecoBase/RecoBaseDefs.h"
#include "RecoBase/ArichID.h"

namespace caf
{

  caf::SRBeamTrack VertexFiller::GetBeamTrack(rb::Track& track, const std::vector<sim::SSDHit>& truehitv)
  {
    caf::SRBeamTrack beamTrk = track;

    std::unordered_map<int,const sim::SSDHit*> ssdHitMap;
    int station, plane, sensor, strip, id;

    for (unsigned int truehitId = 0; truehitId < truehitv.size(); ++truehitId) {

      const sim::SSDHit& ssdhit = truehitv[truehitId];

      station = ssdhit.Station();
      plane = ssdhit.Plane();
      sensor = ssdhit.Sensor();
      strip = ssdhit.Strip();
      id = (station*100000) + (plane*10000) + (sensor*1000) + strip;

      ssdHitMap[id] = &ssdhit;
    }
    for (size_t i=0; i<track.NSSDLineSegments(); ++i) {
      auto lseg = track.GetSSDLineSegment(i);
      station = lseg->SSDStation();
      plane = lseg->SSDPlane();
      sensor = lseg->SSDSensor();
      strip = lseg->SSDStrip();
      id = (station*100000) + (plane*10000) + (sensor*1000) + strip; 

      if (station <= 1) {
         caf::SRSimpleTruth truth;
         bool isOk = true; // We want to include the SRSimpleTruth object in the CAF
         auto ssdHitMapEnd = ssdHitMap.end();
         if (ssdHitMap.find(id) == ssdHitMapEnd) {
           id += 1;
           if (ssdHitMap.find(id) == ssdHitMapEnd) {
             id -= 2;
             if (ssdHitMap.find(id) == ssdHitMapEnd)
               isOk = false;
           }
         }
	 if (abs(ssdHitMap[id]->PId()) == 11) isOk = false; // Don't include electrons/delta rays or positrons
         if (isOk) {
           auto ssdhit = ssdHitMap[id];
           truth.pos.SetXYZ(ssdhit->X(),ssdhit->Y(),ssdhit->Z());
           truth.mom.SetXYZ(ssdhit->Px(),ssdhit->Py(),ssdhit->Pz());
           truth.pdgCode = ssdhit->PId();
           truth.G4trkId = ssdhit->TrackID();
           truth.process = ssdhit->Process();

           truth.de = ssdhit->DE();
           truth.station = ssdhit->Station();	 
	   truth.plane = ssdhit->Plane();
	   truth.sensor = ssdhit->Sensor();
	   truth.strip = ssdhit->Strip();  
           beamTrk.truth.push_back(truth);
           //break;
         }
       }
     }
     return beamTrk;

/*
      int station = ssdhit.Station();
      int plane = ssdhit.Plane();
      int sensor = ssdhit.Sensor();
      
      // get particle 
      if (station == 1 && plane == 1 && sensor == 0) {
       br1.truth.pos.SetXYZ(ssdhit.X(),ssdhit.Y(),ssdhit.Z());
       br1.truth.mom.SetXYZ(ssdhit.Px(),ssdhit.Py(),ssdhit.Pz());       
       br1.truth.pdgCode = ssdhit.PId();
       br1.truth.G4trkId = ssdhit.TrackID();
       br1.truth.process = ssdhit.Process();
      }
    }
*/
  }     

  //---------------------------------

  caf::SRSecondaryTrack VertexFiller::GetSecondaryTrack(rb::Track& track, const std::vector<sim::SSDHit>& truehitv, rb::ArichID& arichid)
  {
    caf::SRSecondaryTrack secTrk = track;
    
    //Inserting ARICH informations in track 
    secTrk.arich.trackID = arichid.trackID;
    secTrk.arich.scoresLL = arichid.scoresLL;
    secTrk.arich.scoresML = arichid.scoresML;
    secTrk.arich.nhit =  arichid.nhit;
    

    if (! truehitv.empty()) {
      std::unordered_map<int,const sim::SSDHit*> ssdHitMap;
      int station, plane, sensor, strip, id;

      for (unsigned int truehitId = 0; truehitId < truehitv.size(); ++truehitId) {

        const sim::SSDHit& ssdhit = truehitv[truehitId];

        station = ssdhit.Station();
        plane = ssdhit.Plane();
        sensor = ssdhit.Sensor();
        strip = ssdhit.Strip();
        id = station*100000+plane*10000+sensor*1000+strip;
//        std::cout << "true ssdhit at (station,plane,sensor,strip) = (" << station << "," << plane << ","
//                  << sensor << "," << strip << ")" << std::endl;
        ssdHitMap[id] = &ssdhit;
      }
      // now loop over all SSD linesegments in a track
      for (size_t i=0; i<track.NSSDLineSegments(); ++i) {
        auto lseg = track.GetSSDLineSegment(i);
        station = lseg->SSDStation();
        plane = lseg->SSDPlane();
        sensor = lseg->SSDSensor();
        strip = lseg->SSDStrip();
        id = station*100000+plane*10000+sensor*1000+strip;
//        std::cout << "digit at (station,plane,sensor,strip) = (" << station << "," << plane << ","
//                  << sensor << "," << strip << ")" << std::endl;
        if (station == 2 || station == 3) {
          id = station*100000+plane*10000+sensor*1000+strip;
          caf::SRSimpleTruth truth;
          bool isOk = true; // We want to include the SRSimpleTruth object in the CAF
          auto ssdHitMapEnd = ssdHitMap.end();
          if (ssdHitMap.find(id) == ssdHitMapEnd) {
            id += 1;
            if (ssdHitMap.find(id) == ssdHitMapEnd) {
              id -= 2;
              if (ssdHitMap.find(id) == ssdHitMapEnd)
                isOk = false;
            }
          }
          if (abs(ssdHitMap[id]->PId()) == 11) isOk = false; // Don't include electrons/delta rays or positrons
          if (isOk) {
            auto ssdhit = ssdHitMap[id];
            truth.pos.SetXYZ(ssdhit->X(),ssdhit->Y(),ssdhit->Z());
            truth.mom.SetXYZ(ssdhit->Px(),ssdhit->Py(),ssdhit->Pz());       
            truth.pdgCode = ssdhit->PId();
            truth.G4trkId = ssdhit->TrackID();
            truth.process = ssdhit->Process();

            truth.de = ssdhit->DE();
            truth.station = ssdhit->Station();
            truth.plane = ssdhit->Plane();
            truth.sensor = ssdhit->Sensor();
            truth.strip = ssdhit->Strip();
            secTrk.truth.push_back(truth);
            break;
          }
        }
      }
    } 
    return secTrk;
  }
  //---------------------------------

  void VertexFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    stdrec.vtxs.vtx.clear();
    stdrec.vtxs.nvtx = 0;

    auto hv = evt.getHandle<std::vector<rb::Vertex> >(fVertexLabel);
    auto ht = evt.getHandle<std::vector<rb::Track> >(fTrackLabel);
    auto truehitv = evt.getHandle<std::vector<sim::SSDHit> >(fSSDHitLabel);
    auto ha = evt.getHandle<std::vector<rb::ArichID>> (fArichIDLabel);
 
    std::vector <rb::Vertex> vtxs;
    std::vector <rb::Track> trks;
    std::vector <sim::SSDHit> ssdhits;
    std::vector <rb::ArichID> arichids;

    if ( !hv.failedToGet()) vtxs = *hv;
    if ( !ht.failedToGet()) trks = *ht;
    if ( !truehitv.failedToGet()) ssdhits = *truehitv;
    if ( !ha.failedToGet()) arichids = *ha;

    stdrec.vtxs.nvtx = vtxs.size();


    // loop over vertices
    for (int iv= 0; iv< (int)vtxs.size();iv++) {
      rb::Vertex v = vtxs[iv];
      caf::SRVertex srv = v;
      caf::SRTrack tr1 = trks[0]; // beam track is always first track
      caf::SRBeamTrack btr;
      if (!ssdhits.empty()) btr = GetBeamTrack(trks[0], ssdhits);
      for (size_t i=0; i<trks[0].NTrackSegments(); i++){     
        auto rbts = trks[0].GetTrackSegment(i);
        caf::SRTrackSegment srts;
        srts.vtx = rbts->vtx;
        srts.mom = rbts->mom;
        srts.region = rbts->region;
        srts.nspacepoints = rbts->NSpacePoints();
        srts.pointA = rbts->pointA;
        srts.pointB = rbts->pointB;
        srts.chi2 = rbts->chi2;
        srts.thetaX = rbts->thetaX;
        srts.thetaY = rbts->thetaY;
        btr.Add(srts);
      }
      srv.SetBeamTrack(btr);
      // loop over secondary tracks in vertex
      for (size_t it=0; it < v.sectrkIdx.size(); ++it) {
        auto idx = v.sectrkIdx[it];

        //for now it's easy with single particle, arich ID always has one entry: the first
        caf::SRSecondaryTrack srt = GetSecondaryTrack(trks[idx], ssdhits,arichids[0]);
        for (size_t i=0; i<trks[idx].NTrackSegments(); i++){
          auto rbts = trks[idx].GetTrackSegment(i);
          caf::SRTrackSegment srts;
          srts.region = rbts->region;
          srts.vtx = rbts->vtx;
          srts.mom = rbts->mom;
          srts.nspacepoints = rbts->NSpacePoints();
          srts.pointA = rbts->pointA;
          srts.pointB = rbts->pointB;
          srts.chi2 = rbts->chi2;
          srts.thetaX = rbts->thetaX;
          srts.thetaY = rbts->thetaY;
          srt.Add(srts);
        }
        srv.Add(srt);
      }
      stdrec.vtxs.vtx.push_back(srv);
    }
    stdrec.vtxs.nvtx = stdrec.vtxs.vtx.size();
    
  } // end of loop over vertexs

} // end namespace caf
