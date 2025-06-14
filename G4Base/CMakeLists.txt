FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
		 LIBRARY_NAME G4Base
 		 LIBRARIES
    		 art::Framework_Core
    		 art::Persistency_Common
    		 canvas::canvas
    		 fhiclcpp::fhiclcpp
    		 ROOT::Core
    		 ROOT::Physics
    		 SimulationBase
    		 MagneticField
    		 MagneticFieldService_service
    		 Geometry
    		 GeometryService_service
    		 Geant4::G4analysis
    		 Geant4::G4error_propagation
    		 Geant4::G4event
    		 Geant4::G4FR
    		 Geant4::G4geometry
    		 Geant4::G4global
    		 Geant4::G4GMocren
    		 Geant4::G4graphics_reps
    		 Geant4::G4intercoms
    		 Geant4::G4interfaces
    		 Geant4::G4materials
    		 Geant4::G4modeling
    		 Geant4::G4OpenGL
    		 Geant4::G4parmodels
    		 Geant4::G4particles
    		 Geant4::G4persistency
   		 Geant4::G4physicslists
    		 Geant4::G4processes
    		 Geant4::G4RayTracer
    		 Geant4::G4readout
    		 Geant4::G4run
    		 Geant4::G4tracking
    		 Geant4::G4track
    		 Geant4::G4Tree
    		 Geant4::G4visHepRep
    		 Geant4::G4vis_management
    		 Geant4::G4VRML
    		 Geant4::G4zlib 
		 BASENAME_ONLY
)

install_headers()
install_source()
