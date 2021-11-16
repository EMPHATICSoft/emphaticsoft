
# the utcDateTime variable is used by the GetPackageBuildInfo code
# to report the build datetime
execute_process( COMMAND "date" "-u" "+%d-%b-%Y %H:%M:%S %Z" OUTPUT_VARIABLE rawUTCDate )
string( STRIP ${rawUTCDate} utcDateTime )

execute_process(COMMAND git describe --tags
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_DESCRIBE_RESULT
        OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)

message("${GIT_DESCRIBE_RESULT}: ${GIT_DESCRIBE_VERSION} || $ENV{CETPKG_VERSION}")

if(${GIT_DESCRIBE_RESULT} EQUAL 0)
  set(version_with_git ${GIT_DESCRIBE_VERSION})
else(${GIT_DESCRIBE_RESULT})
  set(version_with_git $ENV{CETPKG_VERSION})
endif()

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/GetPackageBuildInfo.cc.in
                 ${CMAKE_CURRENT_BINARY_DIR}/GetPackageBuildInfo.cc @ONLY )

cet_make_library( LIBRARY_NAME sbndaq-artdaq_BuildInfo
                  SOURCE  ${CMAKE_CURRENT_BINARY_DIR}/GetPackageBuildInfo.cc
                 )

install_headers()
install_source()
