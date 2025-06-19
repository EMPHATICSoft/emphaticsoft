#[============================================[
Findmillepede_ii
----------------
  find MillepedeII (UPS product millepede_ii)

#]============================================]

find_path(millepedeii_INCLUDE_DIRS
  NAMES millepede_ii/Mille.h
  HINTS ENV MILLEPEDE_INC
  REQUIRED
)

find_library(millepedeii_LIBRARY
  NAMES mille
  HINTS ENV MILLEPEDE_LIB ENV MILLEPEDE_II_DIR
  PATH_SUFFIXES lib
  REQUIRED
)

if (millepedeii_INCLUDE_DIRS AND millepedeii_LIBRARY)
  set(millepedeii_FOUND TRUE)
  if (NOT TARGET millepede_ii::millepede_ii)
    add_library(millepede_ii::millepede_ii STATIC IMPORTED)
    set_target_properties(millepede_ii::millepede_ii PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${millepedeii_INCLUDE_DIRS}"
      IMPORTED_LOCATION "${millepedeii_LIBRARY}"
    )
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(millepedeii
  REQUIRED_VARS millepedeii_FOUND millepedeii_INCLUDE_DIRS millepedeii_LIBRARY
)

mark_as_advanced(millepedeii_INCLUDE_DIRS millepedeii_LIBRARY)
