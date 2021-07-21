////////////////////////////////////////////////////////////////////////
/// \brief   TODO
/// \author  bckhouse@hep.caltech.edu
////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"

#include "Geometry/Geometry.h"
#include "GeometryObjects/Geo.h"
#include "GeometryObjects/CellGeo.h"
#include "MCCheater/BackTracker.h"
#include "RecoBase/Prong.h"

// ROOT includes
#include "TVector3.h"


/// Tutorial modules
namespace tut
{
  class TutProducer : public art::EDProducer
  {
  public:
    explicit TutProducer(const fhicl::ParameterSet& pset);
    ~TutProducer();

    void produce(art::Event& evt);

    void reconfigure(const fhicl::ParameterSet& pset);

    void beginJob();

  protected:
    std::string fGeantLabel, fCellHitLabel;

    rb::Prong FitPhoton(art::Handle<std::vector<rb::CellHit> > chits,
                        TVector3 vtx, int photId) const;
  };
}


////////////////////////////////////////////////////////////////////////
namespace tut
{
  //.......................................................................
  TutProducer::TutProducer(const fhicl::ParameterSet& pset)
  : EDProducer(pset)
  {
    reconfigure(pset);

    produces<std::vector<rb::Prong> >();
  }

  //......................................................................
  TutProducer::~TutProducer()
  {
  }

  //......................................................................
  void TutProducer::reconfigure(const fhicl::ParameterSet& pset)
  {
    fGeantLabel = pset.get<std::string>("GeantLabel");
    fCellHitLabel = pset.get<std::string>("CellHitLabel");
  }

  //......................................................................
  void TutProducer::beginJob()
  {
  }

  //......................................................................
  void TutProducer::produce(art::Event& evt)
  {
    // Declare a container for Prong objects to be stored in the art::event
    std::unique_ptr<std::vector<rb::Prong> > prongcol(new std::vector<rb::Prong>);

    // This bit is all duplicated out of TutFilter. Need to find that pi0
    // again. In a non-demo context we probably wouldn't bother with the filter
    // at all.

    art::Handle<std::vector<rb::CellHit> > chits;
    evt.getByLabel(fCellHitLabel, chits);

    // get the particle navigator from the BackTracker
    art::ServiceHandle<cheat::BackTracker> bt;
    const sim::ParticleNavigator& nav = bt->ParticleNavigator();

    // pizero should be a primary
    for(int primIdx = 0; primIdx < nav.NumberOfPrimaries(); ++primIdx){
      const sim::Particle* pizero = nav.Primary(primIdx);
      // look for pi0's only
      if(pizero->PdgCode() != 111) continue;

      if(pizero->NumberDaughters() != 2) continue;

      // Check the daughters are what they should be
      const sim::Particle* phot0 = nav[pizero->Daughter(0)];
      if(phot0->PdgCode() != 22) continue;
      const sim::Particle* phot1 = nav[pizero->Daughter(1)];
      if(phot1->PdgCode() != 22) continue;

      // New code starts here

      // True vertex
      const TVector3 vtx = phot0->Position(0).Vect();

      // Stick photons in an array so we can loop over them
      const sim::Particle* phots[2] = {phot0, phot1};

      for(int photIdx = 0; photIdx < 2; ++photIdx){
        // This function is getting long, so do all the actual work in
        // FitPhoton(). Put the result in the prongs collection
        const int photId = phots[photIdx]->TrackId();
        prongcol->push_back(FitPhoton(chits, vtx, photId));
      } // end for photIdx
    } // end for primIdx

    // Don't forget to save our prongs...
    evt.put(std::move(prongcol));
  }

  //......................................................................
  rb::Prong TutProducer::FitPhoton(art::Handle<std::vector<rb::CellHit> > chits,
                                   TVector3 vtx,
                                   int photId) const
  {
    // Services we're going to need
    art::ServiceHandle<cheat::BackTracker> bt;
    art::ServiceHandle<geo::Geometry> geom;

    // We're going to store all the constituent hits here
    art::PtrVector<rb::CellHit> prongHits;

    // Fit each view seperately and store the gradients in here
    double dvdz[2];
    for(geo::View_t view: {geo::kX, geo::kY}){

      // We're going to accumulate information about all the matching hits
      // to do a straight line fit with.
      std::vector<double> zs, vs, pes;

      // go through hits and cheat to find what hits are from what photon
      for(unsigned int chitIdx = 0; chitIdx < chits->size(); ++chitIdx){
        art::Ptr<rb::CellHit> chit(chits, chitIdx);

        if(bt->IsNoise(chit)) continue; // if hit is noise, ignore

        if(chit->View() != view) continue; // Wrong view

        // Did this hit make any major contribution to the prong?
        bool any = false;

        // get track IDs of tracks that made light in this cell
        const std::vector<cheat::TrackIDE> ides = bt->HitToTrackIDE(chit);

        // go through IDs
        for(unsigned int idIdx = 0; idIdx < ides.size(); ++idIdx){
          const int id = ides[idIdx].trackID;

          // Did this ID contribute to the right photon?
          if(id == photId){
            // good, so what fraction of the total energy deposited came
            // from this particle?
            const double frac = ides[idIdx].energyFrac;

            // If it's the majority contributor, keep it
            if(frac > 0.5) any = true;

            // Now find out the physical position of the cell
            double xyz[3];
            geo::View_t junk; // Already know the view
            geom->CellInfo(chit->Plane(), chit->Cell(), &junk, xyz);

            zs.push_back(xyz[2]);
            // The view we actually measure with this cell
            vs.push_back(xyz[view]);
            // Don't know where the track goes exactly yet so can't calibrate
            pes.push_back(frac*chit->PE());
          } // end if
        } // end for idIdx

        if(any) prongHits.push_back(chit);
      } // end for chitIdx

      // Not enough hits to do a fit
      if(vs.size() < 2){
        dvdz[view] = 0;
        continue;
      }

      double z1, v1, z2, v2;
      geo::LinFitMinDperp(zs, vs, pes, &z1, &v1, &z2, &v2);

      dvdz[view] = (v2-v1)/(z2-z1);
    } // end for view

    // Combine two fits into one 3D vector
    const TVector3 dir(dvdz[0], dvdz[1], 1);

    // For the sake of the demo, use the known true vertex
    return rb::Prong(prongHits, vtx, dir);
  }

} // end namespace tut


////////////////////////////////////////////////////////////////////////
namespace tut
{
  DEFINE_ART_MODULE(TutProducer)
}
