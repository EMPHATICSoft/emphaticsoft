FILE( GLOB src_files *.cc )

art_make_library(LIBRARY_NAME lappd_reco
                SOURCE ${src_files}
                LIBRARIES PRIVATE
                        lappd_obj
                        ROOT::Core
                        ROOT::Minuit

)

install_headers()
install_source()

add_subdirectory(Alg)


