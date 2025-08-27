#[================================================================[.rst:
Findsrproxy
----------
  find srproxy

#]================================================================]

#MESSAGE("srproxy_INC is $ENV{SRPROXY_INC}")
if (srproxy_FIND_REQUIRED)
  set(_cet_srproxy_FIND_REQUIRED ${srproxy_FIND_REQUIRED})
  unset(srproxy_FIND_REQUIRED)
else()
  unset(_cet_srproxy_FIND_REQUIRED)
endif()
find_package(srproxy CONFIG QUIET)
if (_cet_srproxy_FIND_REQUIRED)
  set(srproxy_FIND_REQUIRED ${_cet_srproxy_FIND_REQUIRED})
  unset(_cet_srproxy_FIND_REQUIRED)
endif()
if (srproxy_FOUND)
  set(_cet_srproxy_config_mode CONFIG_MODE)
else()
  unset(_cet_srproxy_config_mode)
  find_file(_cet_srproxy_h NAMES SRProxy/BasicTypesProxy.h HINTS ENV SRPROXY_INC)
  MESSAGE("${_cet_srproxy_h}")
  if (_cet_srproxy_h)
    get_filename_component(_cet_srproxy_include_dir "${_cet_srproxy_h}" PATH)
    #MESSAGE("${_cet_srproxy_include_dir}")
    if (_cet_srproxy_include_dir STREQUAL "/")
      unset(_cet_srproxy_include_dir)
    endif()
  endif()
  if (EXISTS "${_cet_srproxy_include_dir}")
    set(srproxy_FOUND TRUE)
    get_filename_component(_cet_srproxy_dir "${_cet_srproxy_include_dir}" PATH)
    #MESSAGE("${_cet_srproxy_dir}")
    if (_cet_srproxy_dir STREQUAL "/")
      unset(_cet_srproxy_dir)
    endif()
    set(srproxy_INCLUDE_DIRS "$ENV{SRPROXY_INC}")
  endif()
endif()
if (srproxy_FOUND)
  add_library(srproxy::srproxy INTERFACE IMPORTED)
  MESSAGE("sproxy include directory set to $ENV{SRPROXY_INC}")
  set_target_properties(srproxy::srproxy PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "$ENV{SRPROXY_INC}"
    INTERFACE_COMPILE_DEFINITIONS "${SRPROXY_DEFINITIONS}"
    )
  
  if (CETMODULES_CURRENT_PROJECT_NAME AND
      ${CETMODULES_CURRENT_PROJECT_NAME}_OLD_STYLE_CONFIG_VARS)
    include_directories("${srproxy_INCLUDE_DIRS}") ### MIGRATE-ACTION-RECOMMENDED (migrate-3.13.02) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(srproxy ${_cet_srproxy_config_mode}
  REQUIRED_VARS srproxy_FOUND
  srproxy_INCLUDE_DIRS)


unset(_cet_srproxy_FIND_REQUIRED)
unset(_cet_srproxy_config_mode)
unset(_cet_srproxy_dir)
unset(_cet_srproxy_include_dir)
unset(_cet_srproxy_h CACHE)
