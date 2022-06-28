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
export\ CETPKG_SOURCE\=\\\/emph\\\/app\\\/users\\\/dbattag2\\\/onmonTOFPlots\\\/emphaticsoft$'\n'\
export\ CETPKG_BUILD\=\\\/emph\\\/app\\\/users\\\/dbattag2\\\/onmonTOFPlots\\\/emphaticsoft$'\n'\
CETPKG_NAME\=emphaticsoft$'\n'\
CETPKG_VERSION\=v0_01_00$'\n'\
CETPKG_QUALSPEC\=e19\\\:prof$'\n'\
CETPKG_CQUAL\=e19$'\n'\
CETPKG_BUILD_TYPE\=prof$'\n'\
CETPKG_EXTQUAL\=$'\n'\
CETPKG_USE_TIME_DEPS\=\(art\ art_root_io\ artdaq\ artdaq_core\ geant4\)$'\n'\
CETPKG_BUILD_ONLY_DEPS\=\(caencomm\ caendigitizer\ caenvme\ cetmodules\)$'\n'\
CETPKG_CMAKE_PROJECT\=emphaticsoft$'\n'\
CETPKG_CMAKE_PROJECT_VERSION\=0\\\.01\\\.00$'\n'\
CETPKG_CMAKE_ARGS\=\(\\\-DWANT_UPS\\\:BOOL\\\=ON\ \\\-DUPS_C_COMPILER_ID\\\:STRING\\\=GNU\ \\\-DUPS_C_COMPILER_VERSION\\\:STRING\\\=8\\\.2\\\.0\ \\\-DUPS_CXX_COMPILER_ID\\\:STRING\\\=GNU\ \\\-DUPS_CXX_COMPILER_VERSION\\\:STRING\\\=8\\\.2\\\.0\ \\\-DUPS_Fortran_COMPILER_ID\\\:STRING\\\=GNU\ \\\-DUPS_Fortran_COMPILER_VERSION\\\:STRING\\\=8\\\.2\\\.0\ \\\-Demphaticsoft_UPS_PRODUCT_NAME\\\:STRING\\\=emphaticsoft\ \\\-Demphaticsoft_UPS_PRODUCT_VERSION\\\:STRING\\\=v0_01_00\ \\\-Demphaticsoft_UPS_QUALIFIER_STRING\\\:STRING\\\=e19\\\:prof\ \\\-DUPS_emphaticsoft_CMAKE_PROJECT_NAME\\\:STRING\\\=emphaticsoft\ \\\-DUPS_emphaticsoft_CMAKE_PROJECT_VERSION\\\:STRING\\\=0\\\.01\\\.00\ \\\-Demphaticsoft_UPS_PRODUCT_FLAVOR\\\:STRING\\\=Linux64bit\\\+3\\\.10\\\-2\\\.17\ \\\-Demphaticsoft_UPS_BUILD_ONLY_DEPENDENCIES\\\=caencomm\\\;caendigitizer\\\;caenvme\\\;cetmodules\ \\\-Demphaticsoft_UPS_USE_TIME_DEPENDENCIES\\\=art\\\;art_root_io\\\;artdaq\\\;artdaq_core\\\;geant4\ \\\-DCMAKE_BUILD_TYPE\\\:STRING\\\=RelWithDebInfo\ \\\-DCMAKE_C_COMPILER\\\:STRING\\\=gcc\ \\\-DCMAKE_CXX_COMPILER\\\:STRING\\\=g\\\+\\\+\ \\\-DCMAKE_Fortran_COMPILER\\\:STRING\\\=gfortran\ \\\-DCMAKE_CXX_STANDARD\\\:STRING\\\=17\ \\\-DCMAKE_CXX_STANDARD_REQUIRED\\\:BOOL\\\=ON\ \\\-DCMAKE_CXX_EXTENSIONS\\\:BOOL\\\=OFF\ \\\-Demphaticsoft_EXEC_PREFIX_INIT\\\:STRING\\\=slf7\\\.x86_64\\\.e19\\\.prof\ \\\-Demphaticsoft_FHICL_DIR_INIT\\\=fcl\ \\\-Demphaticsoft_ADD_NOARCH_DIRS\\\:STRING\\\=PERLLIB_DIR\\\;FHICL_DIR\)$'\n'\
CETPKG_BOOTSTRAP_CETMODULES\=1$'\n'\
CETPKG_CMAKE_BUILD_TYPE\=RelWithDebInfo$'\n'\
CETPKG_DEFAULT_QUAL\=e19$'\n'\
CETPKG_FLAVOR\=Linux64bit\\\+3\\\.10\\\-2\\\.17$'\n'\
CETPKG_FQ_DIR\=slf7\\\.x86_64\\\.e19\\\.prof$'\n'\
CETPKG_PATHSPEC_CACHE\=CETPKG_PFILE\=\\\/emph\\\/app\\\/users\\\/dbattag2\\\/onmonTOFPlots\\\/emphaticsoft\\\/ups\\\/product_deps$'\n'\
$'\n'\
__EOF__
( return 0 2>/dev/null ) && unset __EOF__ \
  || true
