#ifndef TIMESYNC_H
#define TIMESYNC_H
//#define VERBOSE
//#define INTERACTIVE

#include <bits/stdc++.h>
// std::minmax_element of a vector

	template <typename T, typename S> // this type will likely be 'double'
	std::vector<T> calcDifferences(S timestampA, S timestampB, std::vector<size_t> skip, T scale) {
		// timestamps from A and B, skipping B sample indices of skip[index]

		std::vector<T> timeDiffs; timeDiffs.reserve(timestampA.size() * timestampB.size());

		// sort by descending order so we can sequentially pop off skipped sample's indices
		sort(skip.begin(), skip.end(), std::greater<>());

		size_t skipMe = timestampB.size() + 1;
		if(!skip.empty()) {
			skipMe = skip.back();
			skip.pop_back();
		}

		for (size_t i=0; i<timestampA.size(); ++i) {
			for (size_t j=0; j<timestampB.size(); ++j) {
				if(j == skipMe) {
					if(!skip.empty()) {
						skipMe = skip.back();
						skip.pop_back();
					}
					continue;
				}
				T dt = timestampA[i] - timestampB[j];
				if(scale != 1)
					dt = timestampA[i] - timestampB[j]*scale;
				timeDiffs.push_back(dt);
			}
		}
		return timeDiffs;
	}

	template <typename T, typename S> // this type will likely be 'double'
	std::vector<T> calcDifferences(S timestampA, S timestampB) {
		std::vector<size_t> skip;
		T scale = 1;
		return calcDifferences(timestampA, timestampB, skip, scale);
	}

	template <typename T>
	int64_t binToTime(std::vector<T> dt, uint64_t bin) {
		const auto [min, max] = std::minmax_element(dt.begin(), dt.end());
		int nbins = abs(*max - *min + 1);
		return  1.0 *  bin / (nbins-1) * (*max - *min) + 1.0 * *min / (nbins-1) * nbins - 1.0 * *max/(nbins-1);
	}

	template <typename T>
	std::tuple<uint64_t,int64_t> findOffset(std::vector<T> dt, int timeUncertainty) {
		// returns <N_occurrences, timeOffset>
		// - NTK

		std::sort(dt.begin(), dt.end());

		int64_t bestTimeDiff = 0;
		size_t bestCount = 0;
		{
			int64_t currTimeDiff= 0;
			size_t currCount = 0;
			for(auto value : dt) {
				if(value != currTimeDiff) {
					if(currCount > bestCount) {
						bestCount = currCount;
						bestTimeDiff = currTimeDiff;
					}
					currTimeDiff = value;
					currCount = 1;
				} else {
					++currCount;
				}
			}
			if(currCount > bestCount) {
				bestCount = currCount;
				bestTimeDiff = currTimeDiff;
			}
		}

		bestCount = 0;
		double meanTimeDiff = 0;
		for(auto value : dt) {
			if(std::abs(value - bestTimeDiff) <= timeUncertainty) {
				meanTimeDiff = bestCount * meanTimeDiff + value;
				++bestCount;
				meanTimeDiff = meanTimeDiff/bestCount;
			}
		}
		bestTimeDiff = std::round(meanTimeDiff);
		return { bestCount, bestTimeDiff };
	}
	// Returns the last time synchronized item
	template <typename T>
	std::tuple<uint64_t,uint64_t> indexOfLastSync(const std::vector<T> A, const std::vector<T> B, int timeUncertainty) {
		uint64_t aIndex = 0;
		uint64_t bIndex = 0;

		{
			bool exitLoop = false;
			for(int aEvent = A.size() - 1; aEvent >= 0; --aEvent) {
				for(int bEvent = B.size() - 1; bEvent >= 0; --bEvent) {
					if(std::llabs(A[aEvent] - B[bEvent]) <= timeUncertainty) {
						aIndex = aEvent;
						bIndex = bEvent;
						exitLoop = true;
						break;
					}
				}
				if(exitLoop) break;
			}
		}

		return { aIndex, bIndex };
	}

	// Returns vector of synchronized events
	template <typename T>
	std::vector<int> indexOfMatch(const std::vector<T> A, const std::vector<T> B, const int timeUncertainty) {
		std::vector<int> mask(A.size()); std::fill(mask.begin(),mask.end(),-1);

		uint64_t bStart = 0;
		for(size_t aEvent = 0; aEvent < A.size(); ++aEvent) {
			for(size_t bEvent = bStart; bEvent < B.size(); ++bEvent) {
				if(std::llabs(A[aEvent] - B[bEvent]) <= timeUncertainty) {
					mask[aEvent] = bEvent;
					bStart = bEvent + 1; // no need to check prior events (gives us a speed up)
					break; // Found matched event, no need to check further
				}
			}
		}

		return mask;
	}

	template <typename T>
	std::tuple<uint64_t, int64_t> xcorr(std::vector<T> father, std::vector<T> child, int timeUncertainty) {
		std::vector<int64_t> dt = calcDifferences<int64_t>(father, child);
		auto [N_occur, offset]  = findOffset(dt, timeUncertainty);
		return { N_occur, offset };
	}

	// Calibrate the cross correlation routine
	template <typename T>
	std::tuple<uint64_t, uint64_t, uint64_t, int64_t> calibrateXcorr(std::vector<T> grandfather, std::vector<T> child, double percentOverlap, int timeUncertainty) {
		// Outputs: { index of last synced event, timeOffset }
		uint64_t index = 0;
		uint64_t N_occur = 0;
		int64_t timeOffset = 0;

		size_t N_compare = child.size();
		// Check that the child vector is smaller than the grandfather vector
		if(child.size() > grandfather.size()) return {2*N_compare, 2*N_compare, 0, 0};
		// Check that percentOverlap is less than 1 and that we are checking for at least 1 overlapping sample
		if(child.size() * percentOverlap < 1 || percentOverlap > 1) return {2*N_compare, 2*N_compare, 0, 0};

		std::vector<int64_t> dt = calcDifferences<int64_t>(grandfather, child);
		std::tie(N_occur, timeOffset)  = findOffset(dt, timeUncertainty);

		if(N_occur < percentOverlap * N_compare || N_occur > N_compare) {
			// did NOT find enough overlapping events!
			return {0, N_compare-1, 0, 0};
		}

		for( auto &timeStamp : child ) // offsets all of the child timestamps
			timeStamp += timeOffset;

		auto [gIndex, cIndex] = indexOfLastSync(grandfather, child, timeUncertainty);

		return { gIndex, cIndex, N_occur, timeOffset };
	}
/***********************************************************************************************
  Returns the indices of CHILD that synchronize with that event of GRANDFATHER
***********************************************************************************************/
	std::vector<int> compareGrandfather(std::vector<uint64_t> grandfather, std::vector<uint64_t> child, int timeUncertainty) {
		// This is the output: a mask for grandfather to indicate which events the child synchronizes with
		std::vector<int> mask;

		// Do the xcorrelation
		size_t N_compare=30; // Number of events to compare

		// Sets overlap percentage to the appropriate value for syncing
		double percentOverlap = 1.0*child.size()/grandfather.size();

		auto begin = grandfather.begin();
		std::vector<int64_t> grandCalibrate(begin, grandfather.end());

		begin = child.begin();
		auto end = child.end();
		if(child.size() > 10*N_compare) end = begin + 10*N_compare;
		std::vector<int64_t> childCalibrate(begin, end);

		auto [aIndex, bIndex, calibrateOccur, calibrateOffset] = calibrateXcorr(grandCalibrate, childCalibrate, percentOverlap, timeUncertainty);

#ifdef VERBOSE
		std::cout << "[Calibration] completed\n";
		std::cout << "(" << aIndex << ", " << bIndex << ", "  << calibrateOccur << ", "<< calibrateOffset << ")\n";
#endif

		// offset the child samples by the time determined by calibrateXcorr
		for(size_t isync = 0; isync < child.size(); ++isync)
			child[isync] += calibrateOffset;

		// begin primary syncs
		while(aIndex < grandfather.size() - N_compare && bIndex < child.size() - N_compare) {
			auto begin = grandfather.begin() + aIndex;
			//std::vector<int64_t> grandSync(begin, begin + N_compare);
			std::vector<int64_t> grandSync(begin, grandfather.end());

			begin = child.begin() + bIndex;

			std::vector<int64_t> childSync(begin, begin + N_compare);
			auto [fIndex, cIndex, N_occur, timeOffset] = calibrateXcorr(grandSync, childSync, percentOverlap, timeUncertainty);
			for(size_t isync = bIndex; isync < child.size(); ++isync)
				child[isync] += timeOffset;

			// increment indices of the two datasets
			aIndex+=fIndex;
			bIndex+=cIndex;
#ifdef VERBOSE
			std::cout << "[ Synchronization ] completed\n";
			std::cout << "(" << aIndex << ", " << bIndex << ", "  << N_occur << ", " << timeOffset << ")\n";
#endif
			if(N_occur >= percentOverlap * N_compare) { // Auto-synchronize Routine
				// Continue through synchronization until events aren't capable of aligning
#ifdef VERBOSE
				std::cout << "[Auto-synchronization]\n";
#endif
				while(std::llabs(grandfather[aIndex] - child[bIndex]) <= timeUncertainty/10) {
					aIndex++; bIndex++;
					if(aIndex >= grandfather.size() - N_compare || bIndex >= child.size() - N_compare) break;
				}
			}
		}
#ifdef VERBOSE
		std::cout << "Finished this pair" << std::endl;
#ifdef INTERACTIVE
		std::cout << "Press any key to continue" << std::endl;
		{char x; std::cin >> x;}
#endif
#endif
		mask = indexOfMatch(grandfather, child, timeUncertainty);
		{ // Ignore a few events at the beginning/end
			size_t ignore = 0;
			for(size_t i = 0; i < mask.size(); ++i) {
				if(mask[i] != -1) {
					mask[i] = -1;
					++ignore;
					if(ignore == N_compare/2) break;
				}
			}
			ignore = 0;
			// Note: this had an issue when stopping condition was i >= 0
			// This was due to the type size_t overflowing instead of going below 0
			// Using i < mask.size() since size_t overflows to above mask.size()
			for(size_t i = mask.size()-1; i < mask.size(); --i) {
				if(mask[i] != -1) {
					mask[i] = -1;
					++ignore;
					if(ignore == N_compare/2) break;
				}
			}
		}
		return mask;
	}
#endif
