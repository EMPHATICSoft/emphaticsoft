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

		// Centre the search on the densest window of width 2*timeUncertainty, not on the most
		// frequent exact value: true pairs scatter by tens of ns, so they rarely share an exact
		// value, and the exact mode often lands on a random repeat or on the first sorted value.
		// Two pointers over the sorted differences: j never moves back, so this is one pass.
		int64_t bestTimeDiff = 0;
		size_t bestCount = 0;
		{
			size_t bestStart = 0;
			for(size_t k = 0, j = 0; k < dt.size(); ++k) {
				if(j < k) j = k;
				while(j < dt.size() && dt[j] - dt[k] <= 2 * (int64_t)timeUncertainty) ++j;
				if(j - k > bestCount) {
					bestCount = j - k;
					bestStart = k;
				}
			}
			double centre = 0;
			for(size_t i = bestStart; i < bestStart + bestCount; ++i)
				centre += dt[i];
			if(bestCount > 0)
				bestTimeDiff = std::round(centre / bestCount);
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
				auto best = timeUncertainty;
				for(int bEvent = B.size() - 1; bEvent >= 0; --bEvent) {
					if(std::llabs(A[aEvent] - B[bEvent]) <= best) {
						best = std::llabs(A[aEvent] - B[bEvent]);
					} else if(best < timeUncertainty) {
						aIndex = aEvent;
						bIndex = bEvent + 1;
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
			auto best = timeUncertainty;
			for(size_t bEvent = bStart; bEvent < B.size(); ++bEvent) {
				if(std::llabs(A[aEvent] - B[bEvent]) <= best) {
					best = std::llabs(A[aEvent] - B[bEvent]);
				} else if(best < timeUncertainty) {
					mask[aEvent] = bEvent - 1;
					bStart = bEvent; // no need to check prior events (gives us a speed up)
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
		size_t N_compare = 30; // Number of events to compare

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
					if(ignore == N_compare/3) break;
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
					if(ignore == N_compare/3) break;
				}
			}
		}
		return mask;
	}

/***********************************************************************************************
  Seed for the first matching pass: initial offset and scale of a child clock against the
  grandfather, found from the gaps between consecutive triggers (they don't depend on the
  offset and barely on the drift).
  - Windows of N_window child gaps, placed from the first and the last beam gap inward until
    N_perEnd anchor at each end. A window anchors where the normalized cross-correlation of its
    log gaps with the grandfather's log gaps peaks (highest N_peaks peaks; keep the one with most
    gaps agreeing within timeUncertainty + driftUncertainty x gap, at least N_agree).
  - Grandfather events closer than deadFraction x the child's smallest gap to the previous kept
    event are dropped first (dead time of the child: the SSDs miss them).
  - End windows must agree with the start windows' offset within maxDrift x span + offsetAgree
    (broken or foreign data at one end of a spill), otherwise they're skipped and the search
    moves inward.
  - Line fit through the anchored pairs, then nearest-neighbour refinement (5 us, 1 us, 300 ns)
    against the full grandfather sequence.
***********************************************************************************************/
	template <typename T>
	double findMedian(std::vector<T> values) {
		if(values.empty()) return 0;
		size_t half = values.size() / 2;
		std::nth_element(values.begin(), values.begin() + half, values.end());
		if(values.size() % 2) return values[half];
		return 0.5 * (values[half] + *std::max_element(values.begin(), values.begin() + half));
	}

	inline std::vector<double> calcGaps(const std::vector<double>& timestamps) {
		std::vector<double> gaps;
		for(size_t i = 1; i < timestamps.size(); ++i)
			gaps.push_back(timestamps[i] - timestamps[i-1]);
		return gaps;
	}

	// Drop events closer than deadTime to the previous kept event
	inline std::vector<double> deadTimeFilter(const std::vector<double>& timestamps, double deadTime) {
		std::vector<double> kept;
		if(timestamps.empty()) return kept;
		kept.push_back(timestamps[0]);
		for(size_t i = 1; i < timestamps.size(); ++i)
			if(timestamps[i] - kept.back() >= deadTime) kept.push_back(timestamps[i]);
		return kept;
	}

	// Least squares y = intercept + scale * x (centred, so ns-scale times keep their precision)
	inline bool fitLine(const std::vector<double>& x, const std::vector<double>& y, double& intercept, double& scale) {
		if(x.size() < 2) return false;
		double meanX = 0, meanY = 0;
		for(size_t i = 0; i < x.size(); ++i) { meanX += x[i]; meanY += y[i]; }
		meanX /= x.size(); meanY /= x.size();
		double varX = 0, covXY = 0;
		for(size_t i = 0; i < x.size(); ++i) {
			varX += (x[i] - meanX) * (x[i] - meanX);
			covXY += (x[i] - meanX) * (y[i] - meanY);
		}
		if(varX <= 0) return false;
		scale = covXY / varX;
		intercept = meanY - scale * meanX;
		return true;
	}

	// Normalized cross-correlation of window against every stretch of sequence of the same length:
	// ncc[shift] compares window with sequence[shift ... shift + window.size())
	inline std::vector<double> normXcorr(const std::vector<double>& window, const std::vector<double>& sequence) {
		const size_t N_window = window.size();
		if(N_window == 0 || sequence.size() < N_window) return {};
		double windowMean = 0, windowStd = 0;
		for(auto value : window) windowMean += value;
		windowMean /= N_window;
		for(auto value : window) windowStd += (value - windowMean) * (value - windowMean);
		windowStd = std::sqrt(windowStd / N_window);
		if(windowStd <= 0) return {};

		std::vector<double> ncc(sequence.size() - N_window + 1);
		for(size_t shift = 0; shift < ncc.size(); ++shift) {
			double sum = 0, sumSquares = 0, crossSum = 0;
			for(size_t i = 0; i < N_window; ++i) {
				double value = sequence[shift + i];
				sum += value;
				sumSquares += value * value;
				crossSum += (window[i] - windowMean) * value;
			}
			double mean = sum / N_window;
			double std = std::sqrt(std::max(sumSquares / N_window - mean * mean, 1e-12));
			ncc[shift] = crossSum / (N_window * windowStd * std);
		}
		return ncc;
	}

	template <typename T>
	std::tuple<uint64_t, double, double> seedSync(std::vector<T> grandfatherIn, std::vector<T> childIn) {
		// returns <N_matched, timeOffset, scale> with grandfather = timeOffset + scale * child
		// N_matched = 0: no seed found
		const size_t N_window = 16;            // gaps per window
		const size_t N_perEnd = 4;             // anchored windows wanted at each end of the spill
		const size_t N_agree = 6;              // agreeing gaps needed to anchor a window
		const size_t N_peaks = 3;              // correlation peaks checked per window
		const double timeUncertainty = 200;    // ns: gaps agree within this ...
		const double driftUncertainty = 10e-6; // ... plus this fraction of the gap (10 ppm)
		const double deadFraction = 0.95;      // dead-time filter: fraction of the child's smallest gap
		const double spillGap = 1e9;           // ns: gaps this long or longer are not beam
		const double maxDrift = 50e-6;         // end windows: allowed offset change per ns of span (50 ppm)
		const double offsetAgree = 5e3;        // ns: start windows must agree among themselves

		std::vector<double> grandfather(grandfatherIn.begin(), grandfatherIn.end());
		std::vector<double> child(childIn.begin(), childIn.end());
		if(child.size() < N_window + 2 || grandfather.size() < N_window + 2) return {0, 0, 1};

		auto childGaps = calcGaps(child);
		double minChildGap = std::numeric_limits<double>::max();
		for(auto gap : childGaps)
			if(gap > 0 && gap < minChildGap) minChildGap = gap;
		auto grandKept = deadTimeFilter(grandfather, deadFraction * minChildGap);
		if(grandKept.size() < N_window + 2) return {0, 0, 1};
		auto grandGaps = calcGaps(grandKept);
		std::vector<double> logGrandGaps;
		for(auto gap : grandGaps) logGrandGaps.push_back(std::log(std::max(gap, 1.0)));

		// beam: from the first to the last child gap shorter than spillGap
		int64_t firstBeam = -1, lastBeam = -1;
		for(size_t i = 0; i < childGaps.size(); ++i) {
			if(childGaps[i] < spillGap) {
				if(firstBeam < 0) firstBeam = i;
				lastBeam = i;
			}
		}
		if(firstBeam < 0) return {0, 0, 1};
		const int64_t middle = (firstBeam + lastBeam) / 2;

		// an anchored window: matched (child, grandfather) event indices and their median offset
		struct Anchor {
			std::vector<size_t> childIndex;
			std::vector<size_t> grandIndex;
			double timeOffset;
		};
		auto anchorWindow = [&](int64_t windowStart, Anchor& anchor) -> bool {
			if(windowStart < 0 || size_t(windowStart) + N_window > childGaps.size()) return false;
			std::vector<double> windowGaps(childGaps.begin() + windowStart, childGaps.begin() + windowStart + N_window);
			std::vector<double> logWindowGaps;
			for(auto gap : windowGaps) logWindowGaps.push_back(std::log(std::max(gap, 1.0)));
			auto ncc = normXcorr(logWindowGaps, logGrandGaps);
			if(ncc.empty()) return false;

			// check the highest N_peaks correlation peaks, keep the one with most agreeing gaps
			std::vector<size_t> peaks(ncc.size());
			std::iota(peaks.begin(), peaks.end(), 0);
			size_t N_check = std::min(N_peaks, peaks.size());
			std::partial_sort(peaks.begin(), peaks.begin() + N_check, peaks.end(),
				[&](size_t a, size_t b) { return ncc[a] > ncc[b] || (ncc[a] == ncc[b] && a > b); });
			int64_t bestShift = -1;
			std::vector<size_t> bestAgree;
			for(size_t ipeak = 0; ipeak < N_check; ++ipeak) {
				size_t shift = peaks[ipeak];
				std::vector<size_t> agree;
				for(size_t igap = 0; igap < N_window; ++igap)
					if(std::abs(grandGaps[shift + igap] - windowGaps[igap]) < timeUncertainty + driftUncertainty * windowGaps[igap])
						agree.push_back(igap);
				if(agree.size() >= N_agree && (bestShift < 0 || agree.size() > bestAgree.size())) {
					bestShift = shift;
					bestAgree = agree;
				}
			}
			if(bestShift < 0) return false;

			// both events of each agreeing gap are a matched pair
			anchor.childIndex.clear();
			anchor.grandIndex.clear();
			for(auto igap : bestAgree) {
				anchor.childIndex.push_back(windowStart + igap);
				anchor.grandIndex.push_back(bestShift + igap);
			}
			for(auto igap : bestAgree) {
				anchor.childIndex.push_back(windowStart + igap + 1);
				anchor.grandIndex.push_back(bestShift + igap + 1);
			}
			std::vector<double> dt;
			for(size_t i = 0; i < anchor.childIndex.size(); ++i)
				dt.push_back(grandKept[anchor.grandIndex[i]] - child[anchor.childIndex[i]]);
			anchor.timeOffset = findMedian(dt);
			return true;
		};

		// start windows: from the first beam gap inward
		std::vector<Anchor> startAnchors;
		for(int64_t windowStart = firstBeam; windowStart < middle; windowStart += N_window) {
			if(startAnchors.size() >= N_perEnd) break;
			Anchor anchor;
			if(anchorWindow(windowStart, anchor)) startAnchors.push_back(anchor);
		}
		if(startAnchors.empty()) {
#ifdef VERBOSE
			std::cout << "[Seed] no start window anchored\n";
#endif
			return {0, 0, 1};
		}
		std::vector<double> startOffsets, startIndices;
		for(auto& anchor : startAnchors) {
			startOffsets.push_back(anchor.timeOffset);
			for(auto index : anchor.childIndex) startIndices.push_back(index);
		}
		const double startOffset = findMedian(startOffsets);
		const size_t startCentre = findMedian(startIndices);
		{ // start windows must agree among themselves
			std::vector<Anchor> agreeing;
			for(auto& anchor : startAnchors)
				if(std::abs(anchor.timeOffset - startOffset) < offsetAgree) agreeing.push_back(anchor);
			startAnchors = agreeing;
		}

		// end windows: from the last beam gap inward; they must agree with the start offset
		std::vector<Anchor> endAnchors;
		size_t N_skipped = 0;
		for(int64_t windowStart = lastBeam - int64_t(N_window) + 1; windowStart > middle; windowStart -= N_window) {
			if(endAnchors.size() >= N_perEnd) break;
			Anchor anchor;
			if(!anchorWindow(windowStart, anchor)) continue;
			double allowed = maxDrift * std::abs(child[windowStart] - child[startCentre]) + offsetAgree;
			if(std::abs(anchor.timeOffset - startOffset) > allowed) {
				++N_skipped;
				continue;
			}
			endAnchors.push_back(anchor);
		}
#ifdef VERBOSE
		std::cout << "[Seed] windows: start " << startAnchors.size() << ", end " << endAnchors.size() << ", end skipped " << N_skipped << "\n";
#endif
		if(endAnchors.empty()) return {0, 0, 1};

		// line through the anchored pairs
		std::vector<double> fitChild, fitGrand;
		for(auto* anchors : {&startAnchors, &endAnchors})
			for(auto& anchor : *anchors)
				for(size_t i = 0; i < anchor.childIndex.size(); ++i) {
					fitChild.push_back(child[anchor.childIndex[i]]);
					fitGrand.push_back(grandKept[anchor.grandIndex[i]]);
				}
		double timeOffset = 0, scale = 1;
		if(!fitLine(fitChild, fitGrand, timeOffset, scale)) return {0, 0, 1};

		// refine: nearest grandfather event of every child event, refit, tighter each time
		uint64_t N_matched = 0;
		for(double matchWindow : {5e3, 1e3, 300.0}) {
			fitChild.clear();
			fitGrand.clear();
			for(auto time : child) {
				double predicted = timeOffset + scale * time;
				size_t above = std::lower_bound(grandfather.begin(), grandfather.end(), predicted) - grandfather.begin();
				above = std::min(std::max(above, size_t(1)), grandfather.size() - 1);
				size_t nearest = (std::abs(grandfather[above-1] - predicted) < std::abs(grandfather[above] - predicted)) ? above - 1 : above;
				if(std::abs(grandfather[nearest] - predicted) < matchWindow) {
					fitChild.push_back(time);
					fitGrand.push_back(grandfather[nearest]);
				}
			}
			N_matched = fitChild.size();
			if(N_matched > 2) fitLine(fitChild, fitGrand, timeOffset, scale);
		}
		return { N_matched, timeOffset, scale };
	}
#endif
