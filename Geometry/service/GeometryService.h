///////////////////////////////////////////////////////////////////////////
/// \brief  Simple service to provide access to the geometry
/// \author jpaley@fnal.gov
//////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRYSERVICE_H
#define GEOMETRYSERVICE_H

#include <array>
#include <map>
#include "Geometry/Geometry.h"

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "TGeoManager.h"
#include "Geometry/ModAlign/ModGDML.h"

namespace emph
{
  namespace geo
  {
    
    class GeometryService 
    {
    public:
      // Get a GeometryService instance here
      GeometryService(const fhicl::ParameterSet& pset,
			art::ActivityRegistry& reg);
      virtual ~GeometryService();
      
      void preBeginRun(const art::Run& run);

      emph::geo::Geometry* Geo() const { return fGeometry.get(); }
      emph::geo::Geometry* GeoRef() const { return fGeometryRef.get(); }

    private:
      std::unique_ptr<emph::geo::Geometry> fGeometry;
      std::unique_ptr<emph::geo::Geometry> fGeometryRef;
      unsigned int fRunNumber;
      bool fGetGDMLFromRunHistory;
      std::string fGDMLFile;
      std::string fGDMLFileRef;
      int fMoveStationNumber;
      float fMoveStationByX, fMoveStationByY, fMoveStationByZ; // fixed move, not random 
      int fRotateStationNumber;
      float fRotateStationBydPhi;
      // Move station 2,3, 5,6,7 .. by 
      std::vector<std::array<float, 3> > fNewCoordStations;
      std::vector<float> fNewRotationStations;
      float fMoveStationByWidthX, fMoveStationByWidthY, fMoveStationByWidthZ, fRotateStationByWidthdPhi;
      //
      bool fMisAlignByStation; // as opposed to by Sensor. 
      bool fMoveAllBySensors; // including station 0 and 4.  
      // Move sensors of stations  0, 1, 2, 3, 4 5,6,7 .. (all of them, this time.. by 
      double fRandomSeed; 
      std::vector<std::array<float, 2> > fNewCoordSensors; // same layout as Mille Params, for sensors move option 
      // same as SingleTrackAlignment, original.. and V2, if option BySensor is active.. 
      std::vector<float> fNewRotationSensors;
      float fMoveSensorByWidthX, fMoveSensorByWidthY, fRotateSensorByWidthdPhi;
     
      float fRotateStation0XPlaneBydPhi, fRotateStation0YPlaneBydPhi; // not in use yet.. 
      float fRotateStation4XPlaneBydPhi, fRotateStation4YPlaneBydPhi;
      
      // internal 
      std::array<short, 8> fNPlanes; // Phase1, late runs 
      std::map<short, size_t > mapBySensorToIndexX; // same key as in MilleRecords 
      std::map<short, size_t > mapBySensorToIndexY; // same key as in MilleRecords 
      std::map<short, size_t > mapBySensorToIndexPhi; // same key as in MilleRecords 
      std::ostringstream fNewFileNameStrStr;
      
    public: 
     //
     // Accessor for analysis of mis-alignment. 
     //
      inline int IsMovedByStations() const { return fMisAlignByStation; } 
      inline int MovedStationNumber() const { return fMoveStationNumber; } 
      inline int RotatedStationNumber() const { return fRotateStationNumber; } 
      inline float MovedStationByX() const { return fMoveStationByX; } 
      inline float MovedStationByY() const { return fMoveStationByY; } 
      inline float MovedStationByZ() const { return fMoveStationByZ; } 
      inline float RotatedStationBydPhi() const { return fRotateStationBydPhi; } 
      // Now, if we  move all stations.
      inline float MovedStationByX(size_t iStation) const {
         if (fNewCoordStations.size() == 0) return 0.;
	 if (iStation >= fNewCoordStations.size()) return 0.; 
         return fNewCoordStations[iStation][0]; 
      } 
      inline float MovedStationByY(size_t iStation) const { 
         if (fNewCoordStations.size() == 0) return 0.;
	 if (iStation >= fNewCoordStations.size()) return 0.; 
         return fNewCoordStations[iStation][1]; 
      } 
      inline float RotatedStationBydPhi(size_t iStation) const {
        if (fNewRotationStations.size() == 0) return 0.;
	if (iStation >= fNewCoordStations.size()) return 0.;
        return fNewRotationStations[iStation];  
       }
      inline float MovedSensorByX(short iStation, short jPlane, short kSe) const {
        short index = 1000*iStation + 100*jPlane + 10*kSe + 1;
	try { 
           size_t ii = mapBySensorToIndexX.at(index);
	   return fNewCoordSensors[ii][0];
	} catch(...) { return 0. ; } 
      }
      inline float MovedSensorByY(short iStation, short jPlane, short kSe) const {
        short index = 1000*iStation + 100*jPlane + 10*kSe + 2;
	try { 
           size_t ii = mapBySensorToIndexX.at(index);
	   return fNewCoordSensors[ii][1];
	} catch(...) { return 0. ; } 
      } 
       inline float RotatedSensorBydPhi(short iStation, short jPlane, short kSe) const {
        short index = 1000*iStation + 100*jPlane + 10*kSe + 3;
 	try { 
          size_t ii = mapBySensorToIndexPhi.at(index);
          return fNewRotationSensors[ii];
	} catch (...) { return 0. ; } 
      } 
      
    private: // internal methods, for code clarity.. 
      void modifyByStation(ModGDML &myMods); 
      void modifyBySensor(ModGDML &myMods);
         
    };
    
  }
}

DECLARE_ART_SERVICE(emph::geo::GeometryService, SHARED)

#endif
