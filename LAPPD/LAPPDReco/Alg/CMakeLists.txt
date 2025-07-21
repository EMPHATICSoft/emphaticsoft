FILE( GLOB src_files *.cc )

art_make_library(LIBRARY_NAME lappd_alg
                SOURCE ${src_files}
                LIBRARIES PRIVATE
                        ROOT::Core
                        ROOT::Minuit
)

install_source()