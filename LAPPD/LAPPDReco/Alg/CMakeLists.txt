FILE( GLOB src_files *.cc )

art_make_library(LIBRARY_NAME lappd_sp_alg
                SOURCE ${src_files}
                LIBRARIES PRIVATE
                        ROOT::Core
                        ROOT::Minuit
                        lappd_obj
)

install_source()