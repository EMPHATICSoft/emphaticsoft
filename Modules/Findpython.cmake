#[================================================================[.rst:
Findpython
----------
  find python

#]================================================================]

#MESSAGE("python_INC is $ENV{PYTHON_INCLUDE}")
if (python_FIND_REQUIRED)
    set(_cet_python_FIND_REQUIRED ${python_FIND_REQUIRED})
    unset(python_FIND_REQUIRED)
else()
    unset(_cet_python_FIND_REQUIRED)
endif()
find_package(python CONFIG QUIET)
if (_cet_python_FIND_REQUIRED)
    set(python_FIND_REQUIRED ${_cet_python_FIND_REQUIRED})
    unset(_cet_python_FIND_REQUIRED)
endif()
if (python_FOUND)
    set(_cet_python_config_mode CONFIG_MODE)
else()
    unset(_cet_python_config_mode)
    find_file(_cet_python_h NAMES Python.h HINTS ENV PYTHON_INCLUDE )
    #MESSAGE("${_cet_python_h}")
    if (_cet_python_h)
        get_filename_component(_cet_python_include_dir "${_cet_python_h}" PATH)
        #MESSAGE("${_cet_python_include_dir}")
        if (_cet_python_include_dir STREQUAL "/")
            unset(_cet_python_include_dir)
        endif()
    endif()
    if (EXISTS "${_cet_python_include_dir}")
        set(python_FOUND TRUE)
        #MESSAGE("FOUND - python ${python_FOUND}")
        get_filename_component(_cet_python_dir "${_cet_python_include_dir}" PATH)
        #MESSAGE("${_cet_python_dir}")
        if (_cet_python_dir STREQUAL "/")
            unset(_cet_python_dir)
        endif()
        set(python_INCLUDE_DIRS "$ENV{PYTHON_INCLUDE}")
        set(python_LIBRARY_DIR "$ENV{PYTHON_LIB}")
        #MESSAGE("python include dir set to ${python_INCLUDE_DIRS}")
        #MESSAGE("python library dir set to ${python_LIBRARY_DIR}")

        find_library(python_LIBRARY NAMES "python3.9" PATHS ${python_LIBRARY_DIR})

        #MESSAGE("python_LIBRARY set to ${python_LIBRARY}")
    endif()
endif()

if (python_FOUND)

    if (NOT TARGET python::python3)
        MESSAGE("Adding python library target")
        add_library(python::python3 SHARED IMPORTED)
        set_target_properties(python::python3 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "$ENV{PYTHON_INCLUDE}"
                IMPORTED_LOCATION "${python_LIBRARY}"
        )
    endif()
    if (CETMODULES_CURRENT_PROJECT_NAME AND
            ${CETMODULES_CURRENT_PROJECT_NAME}_OLD_STYLE_CONFIG_VARS)
        include_directories("${python_INCLUDE_DIRS}") ### MIGRATE-ACTION-RECOMMENDED (migrate-3.13.02) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(python ${_cet_python_config_mode}
                                  REQUIRED_VARS python_FOUND
                                  python_INCLUDE_DIRS
                                  python_LIBRARY)

unset(_cet_python_FIND_REQUIRED)
unset(_cet_python_config_mode)
unset(_cet_python_dir)
unset(_cet_python_include_dir)
unset(_cet_python_h CACHE)
