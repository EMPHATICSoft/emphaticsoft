////////////////////////////////////////////////////////////////////////
/// \brief   Simple class to map electronics channels to detector channels
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELMAP_H
#define CHANNELMAP_H

#include <vector>
#include <map>
#include <stdint.h>
#include <iostream>

#include "ChannelMap/ChannelDefs.h"
#include "Geometry/DetectorDefs.h"

namespace emph {

  namespace cmap {

    // Electronics channel class: board type and channel number
    class EChannel {
    public:
      EChannel();
      EChannel(emph::cmap::FEBoardType b, int board, int channel) { fBoardType = b; fBoard = board; fChannel = channel; };
      virtual ~EChannel() {};

      emph::cmap::FEBoardType BoardType() const { return fBoardType; }
      int Board()  const  { return fBoard; }
      int Channel() const { return fChannel; }

      void SetBoardType(emph::cmap::FEBoardType b) { fBoardType = b; }
      void SetBoard(int board) { fBoard = board; }
      void SetChannel(int chan) { fChannel = chan; }

      inline bool operator==(const EChannel& echan) const {
	return ((echan.fBoardType == fBoardType)&&(echan.fBoard == fBoard)&&(echan.fChannel == fChannel));
      }

      inline bool operator<(const EChannel& echan) const {
	if (fBoardType < echan.fBoardType) return true;
	if (fBoardType == echan.fBoardType) {
	  if (fBoard < echan.fBoard) return true;
	  if (fBoard == echan.fBoard) {
	    if (fChannel < echan.fChannel) return true;
	  }
	}
	return false;
      }

      inline bool operator>(const EChannel& echan) const {
	if (fBoardType > echan.fBoardType) return true;
	if (fBoardType == echan.fBoardType) {
	  if (fBoard > echan.fBoard) return true;
	  if (fBoard == echan.fBoard) {
	    if (fChannel > echan.fChannel) return true;
	  }
	}
	return false;
      }

      inline friend std::ostream& operator<<(std::ostream& os, const EChannel& echan) {
	os << "ElectronicsChannel: (" << emph::cmap::Board::Name(echan.fBoardType) << "," << echan.fBoard << "," <<echan.fChannel << ")";
	return os;
      }

    private:
      emph::cmap::FEBoardType fBoardType;
      int fBoard;
      int fChannel;
    };

    // Detector channel class: detector id and channel number.  Note, for SSDs, detId will be 100000*stationNumber + silicon board number*1000.  All other detectors will just be emph::geo::DetectorType.  
    class DChannel {
    public:
      DChannel();
      DChannel(emph::geo::DetectorType detId, int channel, int station, short hilo) { fId = detId; fChannel = channel; fStation = station; fHiLo = hilo; fPlane = -5;};
      virtual ~DChannel() {};

      emph::geo::DetectorType DetId() const { return fId; }
      int Channel() const { return fChannel; }
      int Station() const { return fStation; }
      short HiLo() const { return fHiLo; }
      int Plane () const { return fPlane; }

      void SetDetId(emph::geo::DetectorType id) { fId = id; }
      void SetChannel(int chan) { fChannel = chan; }
      void SetStation(int station) { fStation = station; }
      void SetHiLo(short hilo) { fHiLo = hilo; }
      void SetPlane(int plane) { fPlane = plane; }

      inline bool operator==(const DChannel& dchan) const { // Unecessary, as far as I can tell.. 
        if (dchan.fId != emph::geo::SSD) {
	  return ((dchan.fId == fId)&&(dchan.fStation == fStation)&& 
		(((dchan.fHiLo == fHiLo) && fHiLo>=0) || 
		 ((dchan.fChannel == fChannel) && fChannel>=0))); 
        } else { 
	  return ((dchan.fId == fId)&&(dchan.fStation == fStation) && (dchan.fPlane == fPlane) &&
		(((dchan.fHiLo == fHiLo) && fHiLo>=0) || 
		 ((dchan.fChannel == fChannel) && fChannel>=0))); 
	}
//	if (fId == emph::geo::SSD)std::cerr << " DChannel operator= , SSDs case, return false " << std::endl;
	return false; //should not get here..
      }
      inline bool operator<(const DChannel& dchan) const {
	if ((fId == emph::geo::SSD) && (dchan.fId == emph::geo::SSD))  {
	  if (fPlane != dchan.fPlane) return (fPlane < dchan.fPlane); // True for Phase1b.  Don't need station.. 
	  else {
	    return (fChannel < dchan.fChannel);
	  }
	}
        if (fId < dchan.fId) return true;
	if (fId == dchan.fId) {
	  if (fId == emph::geo::SSD) {
	    if (fStation < dchan.fStation) return true;
	    if (fStation == dchan.fStation) {
	      if ((fChannel>=0) && (fChannel < dchan.fChannel)) return true;
	      if ((fHiLo>=0) && (fHiLo < dchan.fHiLo)) return true;
	    }
	  }
	  else {
	    if (fChannel < dchan.fChannel) return true;
	    if (fChannel == dchan.fChannel) {
	      if (fHiLo < dchan.fHiLo) return true;
	    }
	  }
	}
	return false;
      }
      //
      // Unnecessary, and therefore, not checked.. 
      inline bool operator>(const DChannel& dchan) const {
	if (fId > dchan.fId) return true;
	if (fId == dchan.fId) {
	  if (fId == emph::geo::SSD) {
	    if (fStation > dchan.fStation) return true;
	    if (fStation == dchan.fStation) {
	      if (fPlane > dchan.fPlane) return true;
	      if (fPlane == dchan.fPlane) {  
	        if ((fChannel>=0) && (fChannel > dchan.fChannel)) return true;
	        if ((fHiLo>=0) && (fHiLo > dchan.fHiLo)) return true;
	       }
	    }
	  }
	  else {
	    if (fChannel > dchan.fChannel) return true;
	    if (fChannel == dchan.fChannel) {
	      if (fHiLo > dchan.fHiLo) return true;
	    }
	  }
	}
	return false;
      }

      inline friend std::ostream& operator<<(std::ostream& os, const DChannel& dchan) {
        if (dchan.fId == emph::geo::SSD) {
	  os << "DetectorChannel: (" << emph::geo::DetInfo::Name(dchan.fId) << "," << dchan.fStation 
	                             << "," << dchan.fPlane << "," << dchan.fChannel << ")";
	} else { 
	  os << "DetectorChannel: (" << emph::geo::DetInfo::Name(dchan.fId) << "," << dchan.fChannel << ")";
	}
	return os;
      }

    private:
      emph::geo::DetectorType fId;
      int fChannel;
      int fStation;
      short fHiLo;
      int fPlane;    ///< plane number only for SSDs - other detectors don't need this
    };

    class ChannelMap {
    public:
      ChannelMap(); // Default constructor
      virtual ~ChannelMap() {}; //Destructor

      bool LoadMap(std::string fname="");
      void SetAbortIfFileNotFound(bool f) { fAbortIfFileNotFound = f;}
      void SetMapFileName(std::string fname) { if (fname != fMapFileName) {
	  fMapFileName = fname; fIsLoaded=false;} }

      emph::cmap::DChannel DetChan(emph::cmap::EChannel echan) { if (!fIsLoaded) LoadMap(); return fEChanMap[echan]; }

      emph::cmap::EChannel ElectChan(emph::cmap::DChannel dchan) { if (!fIsLoaded) LoadMap(); return fDChanMap[dchan];}

      bool IsValidEChan(emph::cmap::EChannel& echan);
      
      int GetDetectorPlaneFromStationSensorForPhase1b(const emph::cmap::DChannel &dchan) const; 

      void testAccessSSD();
      
      std::map<emph::cmap::EChannel,emph::cmap::DChannel> EChanMap() { return fEChanMap; }
      std::map<emph::cmap::DChannel,emph::cmap::EChannel> DChanMap() { return fDChanMap; }

    private:

      bool fIsLoaded;
      bool fAbortIfFileNotFound;
      std::string fMapFileName;
      std::map<emph::cmap::EChannel,emph::cmap::DChannel> fEChanMap;
      std::map<emph::cmap::DChannel,emph::cmap::EChannel> fDChanMap;

    };

  }
}

#endif // CHANNELMAP_H

