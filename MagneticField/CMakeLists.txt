include(ArtDictionary)
include(ArtMake)

cet_find_package(ROOT REQUIRED)
cet_find_package(Geant4 REQUIRED PUBLIC)

include_directories( $ENV{GEANT4_FQ_DIR}/include )
include_directories( $ENV{ROOT_INC} )

art_make_library(
  SOURCE
    MagneticField.cxx
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    Geant4::G4event
    Geant4::G4intercoms
    Geant4::G4interfaces
    Geant4::G4run
    Geant4::G4tracking
    )

simple_plugin( MagneticField service
  LIBRARIES 
  MagneticField
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  ROOT::Geom
  ROOT::Physics
  Geant4::G4event
  Geant4::G4intercoms
  Geant4::G4interfaces
  Geant4::G4run
  Geant4::G4tracking
  BASENAME_ONLY
  )

install_headers()
install_source()
install_fhicl()

