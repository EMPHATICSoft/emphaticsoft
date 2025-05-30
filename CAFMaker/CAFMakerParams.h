#ifndef CAF_CAFMAKERPARAMS_H
#define CAF_CAFMAKERPARAMS_H

#include "fhiclcpp/types/Table.h"
#include "canvas/Utilities/InputTag.h"
// Give up trying to figure out the correct full set of includes. This does the
// trick
#include "art/Framework/Core/EDAnalyzer.h"

namespace caf
{
  struct CAFMakerParams
  {
    template<class T> using Atom = fhicl::Atom<T>;
    template<class T> using Table = fhicl::Table<T>;
    using Comment = fhicl::Comment;
    using Name = fhicl::Name;
    using string = std::string;
    using InputTag = art::InputTag;

    Atom<std::string> CAFFilename
    {
      Name("CAFFilename"),
      Comment("Provide a string to override the automatic filename.")
    };

    Atom<string> FileExtension     {Name("FileExtension")};
    Atom<string> ARingLabel        {Name("ARingLabel")};
    Atom<string> BACkovHitLabel    {Name("BACkovHitLabel")};
    Atom<string> DataQualLabel     {Name("DataQualLabel")};
    Atom<string> GasCkovHitLabel   {Name("GasCkovHitLabel")};
    Atom<string> LineSegLabel      {Name("LineSegLabel")};    
    Atom<string> SpacePointLabel   {Name("SpacePointLabel")}; 
    Atom<string> SSDClustLabel     {Name("SSDClustLabel")};
    Atom<string> SSDHitLabel       {Name("SSDHitLabel")}; 
    Atom<string> SSDRawLabel       {Name("SSDRawLabel")};
    Atom<string> TrackLabel        {Name("TrackLabel")}; 
    Atom<string> ArichIDLabel	   {Name("ArichIDLabel")};
    Atom<string> TrackSegmentLabel {Name("TrackSegmentLabel")};
    Atom<bool>   GetMCTruth        {Name("GetMCTruth")};
    Atom<bool>   GetMCHits         {Name("GetMCHits")};
    Atom<bool>   SSDOnly           {Name("SSDOnly")};

  }; // end struct
} // end namespace

#endif
