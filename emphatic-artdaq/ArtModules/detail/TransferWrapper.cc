#include "artdaq/DAQdata/Globals.hh"
#define TRACE_NAME "TransferWrapper"

#include "artdaq-core/Data/Fragment.hh"
#include "artdaq-core/Utilities/ExceptionHandler.hh"
#include "artdaq-core/Utilities/TimeUtils.hh"
#include "emphatic-artdaq/ArtModules/detail/TransferWrapper.hh"
#include "artdaq/DAQdata/NetMonHeader.hh"
#include "artdaq/TransferPlugins/MakeTransferPlugin.hh"

#include "cetlib/BasicPluginFactory.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"

#include <csignal>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

namespace {
volatile std::sig_atomic_t gSignalStatus = 0;  ///< Stores singal from signal handler
}

/**
 * \brief Handle a Unix signal
 * \param signal Signal to handle
 */
void signal_handler(int signal)
{
	gSignalStatus = signal;
}

emphaticdaq::TransferWrapper::TransferWrapper(const fhicl::ParameterSet& pset)
    : timeoutInUsecs_(pset.get<std::size_t>("timeoutInUsecs", 100000))
    , last_received_data_()
    , last_report_(std::chrono::steady_clock::now())
    , transfer_(nullptr)
    , pset_(pset)
    , maxEventsBeforeInit_(pset.get<std::size_t>("maxEventsBeforeInit", 5))
    , allowedFragmentTypes_(pset.get<std::vector<int>>("allowedFragmentTypes", {226, 227, 229}))
    , runningStateTimeout_(pset.get<double>("dispatcherConnectTimeout", 0))
    , runningStateInterval_us_(pset.get<size_t>("dispatcherConnectRetryInterval_us", 1000000))
    , quitOnFragmentIntegrityProblem_(pset.get<bool>("quitOnFragmentIntegrityProblem", true))
    , multi_run_mode_(pset.get<bool>("allowMultipleRuns", true))
{
	std::signal(SIGINT, signal_handler);

	try
	{
		if (metricMan)
		{
			metricMan->initialize(pset.get<fhicl::ParameterSet>("metrics", fhicl::ParameterSet()), "Online Monitor");
			metricMan->do_start();
		}
	}
	catch (...)
	{
          artdaq::ExceptionHandler(
              artdaq::ExceptionHandlerRethrow::no,
              "TransferWrapper: could not configure metrics");
	}

	// Clamp possible values
	if (runningStateInterval_us_ < 1000)
	{
		TLOG(TLVL_WARNING) << "Invalid value " << runningStateInterval_us_ << " us detected for dispatcherConnectRetryInterval_us. Setting to 1000 us";
		runningStateInterval_us_ = 1000;
	}
	if (runningStateInterval_us_ > 30000000)
	{
		TLOG(TLVL_WARNING) << "Invalid value " << runningStateInterval_us_ << " us detected for dispatcherConnectRetryInterval_us. Setting to 30,000,000 us";
		runningStateInterval_us_ = 30000000;
	}
}

artdaq::FragmentPtrs emphaticdaq::TransferWrapper::receiveMessage() {
	artdaq::FragmentPtrs fragmentPtrs;
	bool receivedFragment = false;
	static bool initialized = false;
	static size_t fragments_received = 0;

	while (gSignalStatus == 0)
	{
		receivedFragment = false;
		auto fragmentPtr = std::make_unique<artdaq::Fragment>();

		while (!receivedFragment)
		{
			if (gSignalStatus != 0)
			{
				TLOG(TLVL_INFO) << "Ctrl-C appears to have been hit";
				return fragmentPtrs;
			}

			try
			{
				auto result = transfer_->receiveFragment(*fragmentPtr, timeoutInUsecs_);

				if (result >= artdaq::TransferInterface::RECV_SUCCESS)
				{
					receivedFragment = true;
					fragments_received++;

					static size_t cntr = 0;
					auto mod = ++cntr % 10;
					auto suffix = "-th";
					if (mod == 1)
					{
						suffix = "-st";
					}
					if (mod == 2)
					{
						suffix = "-nd";
					}
					if (mod == 3)
					{
						suffix = "-rd";
					}
					TLOG(TLVL_INFO) << "Received " << cntr << suffix << " event, "
					                << "seqID == " << fragmentPtr->sequenceID()
					                << ", type == " << fragmentPtr->typeString();
					last_received_data_ = std::chrono::steady_clock::now();
					continue;
				}
				if (result == artdaq::TransferInterface::DATA_END)
				{
					TLOG(TLVL_ERROR) << "Transfer Plugin disconnected or other unrecoverable error. Shutting down.";
					if (multi_run_mode_)
					{
						initialized = false;
						continue;
					}
					return fragmentPtrs;
				}
				else
				{
					auto tlvl = TLVL_DEBUG + 33;
					if (artdaq::TimeUtils::GetElapsedTime(last_report_) > 1.0 && artdaq::TimeUtils::GetElapsedTime(last_received_data_) > 1.0)
					{
						tlvl = TLVL_WARNING;
						last_report_ = std::chrono::steady_clock::now();
					}

					auto last_received_milliseconds = artdaq::TimeUtils::GetElapsedTimeMilliseconds(last_received_data_);

					// 02-Jun-2018, KAB: added status/result printout
					// to-do: add another else clause that explicitly checks for RECV_TIMEOUT
					TLOG(tlvl) << "Timeout occurred in call to transfer_->receiveFragmentFrom; will try again"
					           << ", status = " << result << ", last received data " << last_received_milliseconds << " ms ago.";
				}
			}
			catch (...)
			{
                          artdaq::ExceptionHandler(
                              artdaq::ExceptionHandlerRethrow::yes,
				                 "Problem receiving data in TransferWrapper::receiveMessage");
			}
		}

		if (fragmentPtr->type() == artdaq::Fragment::EndOfDataFragmentType)
		{
			//if (monitorRegistered_)
			//{
			//	unregisterMonitor();
			//}
			if (multi_run_mode_)
			{
				initialized = false;
				continue;
			}

			return fragmentPtrs;
		}

		checkIntegrity(*fragmentPtr);

		if (initialized || fragmentPtr->type() == artdaq::Fragment::InitFragmentType)
		{
			initialized = true;
			fragmentPtrs.push_back(std::move(fragmentPtr));
			break;
		}

		if (fragments_received > maxEventsBeforeInit_)
		{
			throw cet::exception("TransferWrapper") << "First " << maxEventsBeforeInit_ << " events received did not include the \"Init\" event containing necessary info for art; exiting...";  // NOLINT(cert-err60-cpp)
		}
	}

	return fragmentPtrs;
}

std::unordered_map<artdaq::Fragment::type_t, std::unique_ptr<artdaq::Fragments>>
emphaticdaq::TransferWrapper::receiveMessages() {
	std::unordered_map<artdaq::Fragment::type_t, std::unique_ptr<artdaq::Fragments>> output;

	auto ptrs = receiveMessage();
	for (auto& ptr : ptrs)
	{
		auto fragType = ptr->type();
		auto fragPtr = ptr.release();
		ptr.reset(nullptr);

		if (output.count(fragType) == 0u)
		{
			output[fragType] = std::make_unique<artdaq::Fragments>();
		}

		output[fragType]->emplace_back(std::move(*fragPtr));
	}

	return output;
}

void emphaticdaq::TransferWrapper::checkIntegrity(
    const artdaq::Fragment& fragment) const {
	const size_t artdaqheader = artdaq::detail::RawFragmentHeader::num_words() *
	                            sizeof(artdaq::detail::RawFragmentHeader::RawDataType);
	const auto payload = static_cast<size_t>(fragment.dataEndBytes() - fragment.dataBeginBytes());
	const size_t metadata = sizeof(artdaq::NetMonHeader);
	const size_t totalsize = fragment.sizeBytes();

	const auto type = static_cast<size_t>(fragment.type());

	if (totalsize != artdaqheader + metadata + payload)
	{
		std::stringstream errmsg;
		errmsg << "Error: artdaq fragment of type " << fragment.typeString() << ", sequence ID " << fragment.sequenceID() << " has internally inconsistent measures of its size, signalling data corruption: in bytes,"
		       << " total size = " << totalsize << ", artdaq fragment header = " << artdaqheader << ", metadata = " << metadata << ", payload = " << payload;

		TLOG(TLVL_ERROR) << errmsg.str();

		if (quitOnFragmentIntegrityProblem_)
		{
			throw cet::exception("TransferWrapper") << errmsg.str();  // NOLINT(cert-err60-cpp)
		}

		return;
	}

	auto findloc = std::find(allowedFragmentTypes_.begin(), allowedFragmentTypes_.end(), static_cast<int>(type));

	if (findloc == allowedFragmentTypes_.end())
	{
		std::stringstream errmsg;
		errmsg << "Error: artdaq fragment appears to have type "
		       << type << ", not found in the allowed fragment types list";

		TLOG(TLVL_ERROR) << errmsg.str();
		if (quitOnFragmentIntegrityProblem_)
		{
			throw cet::exception("TransferWrapper") << errmsg.str();  // NOLINT(cert-err60-cpp)
		}

		return;
	}
}

emphaticdaq::TransferWrapper::~TransferWrapper() {
	artdaq::Globals::CleanUpGlobals();
}
