#ifndef artdaq_ots_Overlays_FragmentType_hh
#define artdaq_ots_Overlays_FragmentType_hh
#include "artdaq-core/Data/Fragment.hh"

namespace ots
{
  std::vector<std::string> const names{"MISSED", "UDP", "STIB", "DataGen", "CAENV1720", "CAENV1730", "TRB3", "UNKNOWN"};

namespace detail
{
enum FragmentType : artdaq::Fragment::type_t
{
	MISSED = artdaq::Fragment::FirstUserFragmentType,
	UDP,
	STIB,
	DataGen,
	CAENV1720,
	CAENV1730,
        TRB3,
	INVALID  // Should always be last.
};

// Safety check.
static_assert(artdaq::Fragment::isUserFragmentType(FragmentType::INVALID - 1),
              "Too many user-defined fragments!");
}

using detail::FragmentType;

/**
 * \brief Lookup the type code for a fragment by its string name
 * \param t_string Name of the Fragment type to lookup
 * \return artdaq::Fragment::type_t corresponding to string, or INVALID if not found
 */
FragmentType toFragmentType(std::string t_string);

/**
 * \brief Look up the name of the given FragmentType
 * \param val FragmentType to look up
 * \return Name of the given type (from the names vector)
 */
std::string fragmentTypeToString(FragmentType val);

/**
 * \brief Create a list of all Fragment types defined by this package, in the format that RawInput expects
 * \return A list of all Fragment types defined by this package, in the format that RawInput expects
 */
std::map<artdaq::Fragment::type_t, std::string> makeFragmentTypeMap();
}
#endif /* artdaq_ots_core_Overlays_FragmentType_hh */
