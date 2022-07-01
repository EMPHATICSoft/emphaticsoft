#ifndef artdaq_ArtModules_TransferWrapper_hh
#define artdaq_ArtModules_TransferWrapper_hh

#include <iostream>
#include <memory>
#include <string>

#include "artdaq-core/Data/RawEvent.hh"
#include "artdaq/TransferPlugins/TransferInterface.hh"

namespace fhicl {
class ParameterSet;
}

namespace artdaq {
class Fragment;
}

namespace emphaticdaq {
    /**
	 * \brief TransferWrapper wraps a TransferInterface so that it can be used in the ArtdaqInput class
	 * to make an art::Source
	 * 
	 * JCF, May-27-2016
	 *
	 * This is the class through which code that wants to access a
	 * transfer plugin (e.g., input sources, AggregatorCore, etc.) can do
	 * so. Its functionality is such that it satisfies the requirements
	 * needed to be a template in the ArtdaqInput class
	 */
class TransferWrapper
{
public:
	/**
		 * \brief TransferWrapper Constructor
		 * \param pset ParameterSet used to configure the TransferWrapper
		 * 
		 * \verbatim
		 * TransferWrapper accepts the following Parameters:
		 * "timeoutInUsecs" (Default: 100000): The receive timeout
		 * "maxEventsBeforeInit" (Default: 5): How many non-Init events to receive before raising an error
		 * "allowedFragmentTypes" (Default: [226,227,229]): The Fragment type codes for expected Fragments
		 * "dispatcherConnectTimeout" (Default: 0): Maximum amount of time (in seconds) to wait for the Dispatcher to reach the Running state. 0 to wait forever
		 * "dispatcherConnectRetryInterval_us" (Default 1,000,000): Amount of time to wait between polls of the Dispatcher status while waiting for it to reach the Running state.
		 * "quitOnFragmentIntegrityProblem" (Default: true): If there is an inconsistency in the received Fragment, throw an exception and quit when true
		 * "allowMultipleRuns" (Default: true): If true, will ignore EndOfData message and reconnect to the Dispatcher once the next run starts
		 * "debugLevel" (Default: 0): Enables some additional messages
		 * "transfer_plugin" (REQUIRED): Name of the TransferInterface plugin to load
		 * 
		 * This parameter set is also passed to TransferInterface, so any necessary Parameters for TransferInterface or the requested plugin
		 * should be included here.
		 * \endverbatim
		 */
	explicit TransferWrapper(const fhicl::ParameterSet& pset);

	/**
		 * \brief TransferWrapper Destructor
		 */
	virtual ~TransferWrapper();

	/**
		 * \brief Receive a Fragment from the TransferInterface, and send it to art
		 * \return Received Fragment
		 */
	artdaq::FragmentPtrs receiveMessage();
	/**
	 * \brief Receive all messsages for an event from ArtdaqSharedMemoryService
	 * \return A map of Fragment::type_t to a unique_ptr to Fragments containing all Fragments in an event
	 */
	std::unordered_map<artdaq::Fragment::type_t, std::unique_ptr<artdaq::Fragments>> receiveMessages();

	/**
		 * \brief Receive the Init message from the TransferInterface, and send it to art
		 * \return Received InitFragment
		 */
	artdaq::FragmentPtrs receiveInitMessage()
	{
		return receiveMessage();
	}

	/**
	 * \brief Get a pointer to the last received RawEventHeader
	 * \return a shared_ptr to the last received RawEventHeader
	 */
	std::shared_ptr<artdaq::detail::RawEventHeader> getEventHeader() { return nullptr; }

private:
	TransferWrapper(TransferWrapper const&) = delete;
	TransferWrapper(TransferWrapper&&) = delete;
	TransferWrapper& operator=(TransferWrapper const&) = delete;
	TransferWrapper& operator=(TransferWrapper&&) = delete;

	void checkIntegrity(const artdaq::Fragment&) const;

	std::size_t timeoutInUsecs_;
	std::chrono::steady_clock::time_point last_received_data_;
	std::chrono::steady_clock::time_point last_report_;
	std::unique_ptr<artdaq::TransferInterface> transfer_;
	const fhicl::ParameterSet pset_;
	const std::size_t maxEventsBeforeInit_;
	const std::vector<int> allowedFragmentTypes_;
	const double runningStateTimeout_;
	size_t runningStateInterval_us_;
	const bool quitOnFragmentIntegrityProblem_;
	const bool multi_run_mode_;
};  // namespace artdaq
}  // namespace artdaq

#endif /* artdaq_ArtModules_TransferWrapper_hh */
