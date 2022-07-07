#include "artdaq/ArtModules/ArtdaqInputHelper.hh"
#include "emphatic-artdaq/ArtModules/detail/TransferWrapper.hh"

namespace art {
/**
	 * \brief Trait definition (must precede source typedef).
	 */
template<>
struct Source_generator<ArtdaqInputHelper<emphaticdaq::TransferWrapper>> {
	static constexpr bool value = true;  ///< Dummy variable
};

/**
	 * \brief TransferInput is an art::Source using the artdaq::TransferWrapper class as the data source
	 */
using TransferInput = art::Source<ArtdaqInputHelper<emphaticdaq::TransferWrapper>>;
}  // namespace art

DEFINE_ART_INPUT_SOURCE(art::TransferInput)
