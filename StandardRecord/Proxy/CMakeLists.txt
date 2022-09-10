FILE(GLOB SR_DEPENDENCIES StandardRecord/*.h)

add_custom_command(# Rebuild if anything in StandardRecord/ changes
                   DEPENDS ${SR_DEPENDENCIES}
		   OUTPUT SRProxy.cxx SRProxy.h FwdDeclare.h
		   COMMAND gen_srproxy -i StandardRecord/StandardRecord.h
		   	   	       -o SRProxy
				       --target caf::StandardRecord
				       --include-path $ENV{CETPKG_SOURCE}:$ENV{ROOT_INC}
				       --output-path StandardRecord/Proxy/
				       --prolog ${CMAKE_CURRENT_SOURCE_DIR}/Prolog.h
				       --extra SRVector3D ${CMAKE_CURRENT_SOURCE_DIR}/SRVector3DAddons.h
				       --epilog-fwd ${CMAKE_CURRENT_SOURCE_DIR}/EpilogFwd.h
				       --extra-cflags ' -D_Float16=short'
				       
		   )

include_directories($ENV{SRPROXY_INC})

# This is a very picky error buried inside template instantiations
#add_definitions(-Wno-int-in-bool-context)

art_make_library(LIBRARY_NAME StandardRecordProxy
                 SOURCE       SRProxy.cxx Instantiations.cxx
		 LIBRARIES    ${ROOT_BASIC_LIB_LIST} ROOT::TreePlayer)

install_headers(EXTRAS $ENV{MRB_BUILDDIR}/StandardRecord/Proxy/SRProxy.h $ENV{MRB_BUILDDIR}/StandardRecord/Proxy/FwdDeclare.h)
