FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME StandardRecord
  LIBRARIES PRIVATE
    ROOT::Core
    ROOT::Tree
    ROOT::EG
    ROOT::Physics
  BASENAME_ONLY
)

build_dictionary(StandardRecord
  DICTIONARY_LIBRARIES 
    StandardRecord
    ROOT::Physics
)

install_headers()
install_source()
