////////////////////////////////////////////////////////////////////////
// \file    VertexFiller.cxx
// \brief   Class that does the work to extract reco'd vertexs from
//          the art event and add them to the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/VertexFiller.h"
#include "RecoBase/Vertex.h"
#include "RecoBase/Track.h"
//#include "StandardRecord/SRTrackSegment.h"
#include "StandardRecord/SRBaseDefs.h"
//#include "RecoBase/ArichID.h"

namespace caf
{

  void VertexFiller::GetBeamTrackTruth(caf::SRBeamTrack& beamTrk, rb::Track& track, const std::vector<sim::SSDHit>& truehitv)
  {
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
  }     

  //---------------------------------

  void VertexFiller::GetSecondaryTrackTruth( caf::SRSecondaryTrack& secTrk, rb::Track& track, const std::vector<sim::SSDHit>& truehitv)
  {
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
  }

  //---------------------------------

  void VertexFiller::FillKalman(art::Event& evt, std::vector<caf::SRVertex>& vtx)
  {
    vtx.clear();    
    auto hv = evt.getHandle<std::vector<rb::Vertex> >(fKVertexLabel);
    auto ht = evt.getHandle<std::vector<rb::Track> >(fKTrackLabel);
    auto hbt = evt.getHandle<std::vector<rb::Track> >(fBeamTrackLabel);
    auto truehitv = evt.getHandle<std::vector<sim::SSDHit> >(fSSDHitLabel);
    auto ha = evt.getHandle<std::vector<rb::ArichID>> (fArichIDLabel);
 
    std::vector <rb::Vertex> vtxs;
    std::vector <rb::Track> trks;
    std::vector <sim::SSDHit> ssdhits;
    std::vector <rb::ArichID> arichids;
    std::vector <rb::Track> btrks;

    if ( !hbt.failedToGet()) {
      btrks = *hbt;
    } else {
      std::cout << "VertexFiller: Failed to get Beam Tracks from " << fBeamTrackLabel << std::endl;
    }
    if ( !hv.failedToGet()) 
    {
      vtxs = *hv;
    }
    else {
      std::cout << "VertexFiller: Failed to get Vertexes from " << fKVertexLabel << std::endl;
    } 
    if ( !ht.failedToGet()) 
    {
      trks = *ht;
    } 
    else  {
      std::cout << "VertexFiller: Failed to get Tracks from " << fKTrackLabel << std::endl;
    }
    if ( !truehitv.failedToGet()) ssdhits = *truehitv;
    if ( !ha.failedToGet()) arichids = *ha;

    // loop over vertices
    for (int iv= 0; iv< (int)vtxs.size();iv++) {
      rb::Vertex v = vtxs[iv];
      caf::SRVertex srv = v;
//      caf::SRTrack tr1 = trks[0]; // beam track is always first track
      caf::SRBeamTrack btr = btrks[0]; // beam track is always first track
      if (!ssdhits.empty()) GetBeamTrackTruth(btr, btrks[0], ssdhits);
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
        //if arich reco was not run, use a dummy ArichID to avoid out-of-bounds access
        rb::ArichID dummyArich;
        caf::SRSecondaryTrack secTrk = trks[idx]; // Initialize the secondary track with the rb::Track data

        //Inserting ARICH informations in track 
        if (!arichids.empty()) { // note, we assume that the first ArichID corresponds to the first secondary track, which may not always be true in a more complex event 
          secTrk.arich.trackID = arichids[0].trackID;
          secTrk.arich.scoresLL = arichids[0].scoresLL;
          secTrk.arich.scoresML = arichids[0].scoresML;
          secTrk.arich.nhit =  arichids[0].nhit;
        } else { // not sure if this is really necessary, but just in case, we fill the ARICH info with the dummyArich data
          secTrk.arich.trackID = dummyArich.trackID;
          secTrk.arich.scoresLL = dummyArich.scoresLL;
          secTrk.arich.scoresML = dummyArich.scoresML;
          secTrk.arich.nhit =  dummyArich.nhit;
        }
    
        if (!ssdhits.empty()) 
          GetSecondaryTrackTruth(secTrk, trks[idx], ssdhits);
        srv.Add(secTrk);
      }
      vtx.push_back(srv);
    }

  }

  //---------------------------------

  void VertexFiller::FillSimple(art::Event& evt, std::vector<caf::SRVertex>& vtx)
  {
    vtx.clear();    

    auto hv = evt.getHandle<std::vector<rb::Vertex> >(fVertexLabel);
    auto ht = evt.getHandle<std::vector<rb::Track> >(fTrackLabel);
    auto hbt = evt.getHandle<std::vector<rb::Track> >(fBeamTrackLabel);
    auto truehitv = evt.getHandle<std::vector<sim::SSDHit> >(fSSDHitLabel);
    auto ha = evt.getHandle<std::vector<rb::ArichID>> (fArichIDLabel);
 
    std::vector <rb::Vertex> vtxs;
    std::vector <rb::Track> trks;
    std::vector <rb::Track> btrks;
    std::vector <sim::SSDHit> ssdhits;
    std::vector <rb::ArichID> arichids;

    if ( !hv.failedToGet()) vtxs = *hv;
    if ( !hbt.failedToGet()) btrks = *hbt;
    if ( !ht.failedToGet()) trks = *ht;
    if ( !truehitv.failedToGet()) ssdhits = *truehitv;
    if ( !ha.failedToGet()) arichids = *ha;

    // loop over vertices
    for (int iv= 0; iv< (int)vtxs.size();iv++) {
      rb::Vertex v = vtxs[iv];
      caf::SRVertex srv = v;
      caf::SRBeamTrack btr = btrks[0]; // beam track is always first track  
      if (!ssdhits.empty()) GetBeamTrackTruth(btr, btrks[0], ssdhits);
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
        //if arich reco was not run, use a dummy ArichID to avoid out-of-bounds access
        rb::ArichID dummyArich;
        caf::SRSecondaryTrack secTrk = trks[idx]; // Initialize the secondary track with the rb::Track data
        if (!ssdhits.empty()) 
          GetSecondaryTrackTruth(secTrk, trks[idx], ssdhits);
        //Inserting ARICH informations in track 
        if (!arichids.empty()) { // note, we assume that the first ArichID corresponds to the first secondary track, which may not always be true in a more complex event 
          secTrk.arich.trackID = arichids[0].trackID;
          secTrk.arich.scoresLL = arichids[0].scoresLL;
          secTrk.arich.scoresML = arichids[0].scoresML;
          secTrk.arich.nhit =  arichids[0].nhit;
        } else { // not sure if this is really necessary, but just in case, we fill the ARICH info with the dummyArich data
          secTrk.arich.trackID = dummyArich.trackID;
          secTrk.arich.scoresLL = dummyArich.scoresLL;
          secTrk.arich.scoresML = dummyArich.scoresML;
          secTrk.arich.nhit =  dummyArich.nhit;
        }

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
          secTrk.Add(srts);
        }
        srv.Add(secTrk);
      }
      vtx.push_back(srv);
    }
    
  } // end of loop over vertexs

} // end namespace caf
