include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )


art_make_library(LIBRARY_NAME lappd_unpacker
                SOURCE ${src_files}
                LIBRARIES PRIVATE
                        lappd_obj
)

install_headers()
install_source()
install_fhicl()