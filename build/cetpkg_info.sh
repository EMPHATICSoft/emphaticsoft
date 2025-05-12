#!/bin/bash
########################################################################
# cetpkg_info.sh
#
#   Generated script to define variables required by buildtool to
#   compose the build environment.
#
# If we're being sourced, define the expected shell and environment
# variables; otherwise, print the definitions for user information.
#
##################
# NOTES
#
# * The definitions printed by executing this script are formatted to be
#   human-readable; they may *not* be suitable for feeding to a shell.
#
# * This script is *not* shell-agnostic, as it is not intended to be a 
#   general setup script.
#
# * Most items are not exported to the environment and will therefore
#   not be visible downstream of the shell sourcing this file.
#
########################################################################

( return 0 2>/dev/null ) && eval "__EOF__() { :; }" && \
  _cetpkg_catit=(:) || _cetpkg_catit=(cat '<<' __EOF__ '|' sed -Ee "'"'s&\\([^\\]|$)&\1&g'"'" )
eval "${_cetpkg_catit[@]}"$'\n'\
export\ CETPKG_SOURCE\=\\\/emphaticsoft$'\n'\
export\ CETPKG_BUILD\=\\\/build$'\n'\
CETPKG_NAME\=emphaticsoft$'\n'\
CETPKG_VERSION\=v2_00_00$'\n'\
export\ CETPKG_QUALSPEC\=e20\\\:prof$'\n'\
CETPKG_CQUAL\=e20$'\n'\
CETPKG_BUILD_TYPE\=prof$'\n'\
CETPKG_EXTQUAL\=$'\n'\
CETPKG_USE_TIME_DEPS\=\(art\ art_root_io\ artdaq\ artdaq_core\ geant4\ ifbeam\ ifdh_art\ nucondb\ srproxy\)$'\n'\
CETPKG_BUILD_ONLY_DEPS\=\(caencomm\ caendigitizer\ caenvme\ cetmodules\)$'\n'\
CETPKG_CMAKE_ARGS\=\(\\\-DCMAKE_C_COMPILER\\\:STRING\\\=gcc\ \\\-DUPS_C_COMPILER_ID\\\:STRING\\\=GNU\ \\\-DUPS_C_COMPILER_VERSION\\\:STRING\\\=9\\\.3\\\.0\ \\\-DCMAKE_CXX_COMPILER\\\:STRING\\\=g\\\+\\\+\ \\\-DUPS_CXX_COMPILER_ID\\\:STRING\\\=GNU\ \\\-DUPS_CXX_COMPILER_VERSION\\\:STRING\\\=9\\\.3\\\.0\ \\\-DCMAKE_Fortran_COMPILER\\\:STRING\\\=gfortran\ \\\-DUPS_Fortran_COMPILER_ID\\\:STRING\\\=GNU\ \\\-DUPS_Fortran_COMPILER_VERSION\\\:STRING\\\=9\\\.3\\\.0\ \\\-DCMAKE_CXX_STANDARD\\\:STRING\\\=17\ \\\-DCMAKE_CXX_STANDARD_REQUIRED\\\:BOOL\\\=ON\ \\\-DCMAKE_CXX_EXTENSIONS\\\:BOOL\\\=OFF\ \\\-DCET_PV_ab946f_FHICL_DIR\\\=fcl\ \\\-DCET_PV_ab946f_ADD_NOARCH_DIRS\\\:INTERNAL\\\=FHICL_DIR\\\;PERLLIB_DIR\ \\\-DWANT_UPS\\\:BOOL\\\=ON\ \\\-DCET_PV_ab946f_UPS_PRODUCT_NAME\\\:STRING\\\=emphaticsoft\ \\\-DCET_PV_ab946f_UPS_PRODUCT_VERSION\\\:STRING\\\=v2_00_00\ \\\-DCET_PV_ab946f_UPS_QUALIFIER_STRING\\\:STRING\\\=e20\\\:prof\ \\\-DCET_PV_ab946f_UPS_PRODUCT_FLAVOR\\\:STRING\\\=Linux64bit\\\+6\\\.12\\\-2\\\.17\ \\\-DCET_PV_ab946f_UPS_BUILD_ONLY_DEPENDENCIES\\\=caencomm\\\;caendigitizer\\\;caenvme\\\;cetmodules\ \\\-DCET_PV_PREFIX\\\:STRING\\\=ab946f\ \\\-DCMAKE_BUILD_TYPE\\\:STRING\\\=RelWithDebInfo\ \\\-DCET_PV_ab946f_EXEC_PREFIX\\\:STRING\\\=slf7\\\.x86_64\\\.e20\\\.prof\)$'\n'\
export\ CETPKG_CXX_COMPILER\=g\\\+\\\+$'\n'\
export\ CETPKG_CXX_COMPILER_ID\=GNU$'\n'\
export\ CETPKG_CXX_COMPILER_VERSION\=9\\\.3\\\.0$'\n'\
export\ CETPKG_CXX_STANDARD\=17$'\n'\
export\ CETPKG_C_COMPILER\=gcc$'\n'\
export\ CETPKG_C_COMPILER_ID\=GNU$'\n'\
export\ CETPKG_C_COMPILER_VERSION\=9\\\.3\\\.0$'\n'\
export\ CETPKG_Fortran_COMPILER\=gfortran$'\n'\
export\ CETPKG_Fortran_COMPILER_ID\=GNU$'\n'\
export\ CETPKG_Fortran_COMPILER_VERSION\=9\\\.3\\\.0$'\n'\
CETPKG_CMAKE_BUILD_TYPE\=RelWithDebInfo$'\n'\
CETPKG_DEFAULT_QUAL\=e20$'\n'\
export\ CETPKG_FLAVOR\=Linux64bit\\\+6\\\.12\\\-2\\\.17$'\n'\
export\ CETPKG_FQ_DIR\=slf7\\\.x86_64\\\.e20\\\.prof$'\n'\
CETPKG_PFILE\=\\\/emphaticsoft\\\/ups\\\/product_deps$'\n'\
CETPKG_PROJECT_VARIABLE_PREFIX\=ab946f$'\n'\
$'\n'\
__EOF__
( return 0 2>/dev/null ) && unset __EOF__ \
  || true
