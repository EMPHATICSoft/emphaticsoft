#[============================================[
Findmillepede_ii
----------------
  find MillepedeII (UPS product millepede_ii)

#]============================================]

find_path(millepede_ii_INCLUDE_DIRS
  NAMES millepede_ii/Mille.h
  HINTS ENV MILLEPEDE_INC
  REQUIRED
)

find_library(millepede_ii_LIBRARY
  NAMES mille
  HINTS ENV MILLEPEDE_LIB ENV MILLEPEDE_II_DIR
  PATH_SUFFIXES lib
  REQUIRED
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(millepede_ii
  REQUIRED_VARS millepede_ii_INCLUDE_DIRS millepede_ii_LIBRARY
)

if (millepede_ii_FOUND)
  if (NOT TARGET millepede_ii::millepede_ii)
    add_library(millepede_ii::millepede_ii STATIC IMPORTED)
    set_target_properties(millepede_ii::millepede_ii PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${millepede_ii_INCLUDE_DIRS}"
      IMPORTED_LOCATION "${millepede_ii_LIBRARY}"
    )
  endif()
endif()

mark_as_advanced(millepede_ii_INCLUDE_DIRS millepede_ii_LIBRARY)
