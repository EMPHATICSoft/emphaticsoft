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
#include "RecoBase/RecoBaseDefs.h"
//#include "RecoBase/ArichID.h"

namespace caf
{

  void VertexFiller::GetBeamTrackTruth(caf::SRBeamTrack& br1, const std::vector<sim::SSDHit>& truehitv)
  {

    for (unsigned int truehitId = 0; truehitId < truehitv.size(); ++truehitId) {

      const sim::SSDHit& ssdhit = truehitv[truehitId];

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
  }     

  //---------------------------------

  caf::SRSecondaryTrack VertexFiller::GetSecondaryTrack(rb::Track& track, const std::vector<sim::SSDHit>& truehitv)
  {
    caf::SRSecondaryTrack secTrk = track;

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
          bool isOk = true;
          auto ssdHitMapEnd = ssdHitMap.end();
          if (ssdHitMap.find(id) == ssdHitMapEnd) {
            id += 1;
            if (ssdHitMap.find(id) == ssdHitMapEnd) {
              id -= 2;
              if (ssdHitMap.find(id) == ssdHitMapEnd)
                isOk = false;
            }
          }
          if (isOk) {
            auto ssdhit = ssdHitMap[id];
            truth.pos.SetXYZ(ssdhit->X(),ssdhit->Y(),ssdhit->Z());
            truth.mom.SetXYZ(ssdhit->Px(),ssdhit->Py(),ssdhit->Pz());       
            truth.pdgCode = ssdhit->PId();
            truth.G4trkId = ssdhit->TrackID();
            truth.process = ssdhit->Process();
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
    std::vector <rb::Vertex> vtxs;
    std::vector <rb::Track> trks;
    std::vector <sim::SSDHit> ssdhits;

    if ( !hv.failedToGet()) vtxs = *hv;
    if ( !ht.failedToGet()) trks = *ht;
    if ( !truehitv.failedToGet()) ssdhits = *truehitv;

    stdrec.vtxs.nvtx = vtxs.size();

    // loop over vertices
    for (int iv= 0; iv< (int)vtxs.size();iv++) {
      rb::Vertex v = vtxs[iv];
      caf::SRVertex srv = v;
      caf::SRTrack tr1 = trks[0]; // beam track is always first track
      caf::SRBeamTrack btr(tr1);
      if (!ssdhits.empty()) GetBeamTrackTruth(btr,ssdhits);
      srv.SetBeamTrack(btr);
      // loop over secondary tracks in vertex
      for (size_t it=0; it < v.sectrkIdx.size(); ++it) {
        auto idx = v.sectrkIdx[it];
        caf::SRSecondaryTrack srt = GetSecondaryTrack(trks[idx], ssdhits);
        srv.Add(srt);
      }
      stdrec.vtxs.vtx.push_back(srv);
    }
    stdrec.vtxs.nvtx = stdrec.vtxs.vtx.size();
    
  } // end of loop over vertexs

} // end namespace caf
