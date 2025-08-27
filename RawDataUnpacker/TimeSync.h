#include <bits/stdc++.h>
// std::minmax_element of a vector
// - NTK

#define timeUncertainty 16
template <typename T, typename S> // this type will likely be 'double'
std::vector<T> calcDifferences(S timestampA, S timestampB, std::vector<size_t> skip, T scale) {
	// timestamps from A and B, skipping B sample indices of skip[index]

	std::vector<T> timeDiffs; timeDiffs.reserve(timeStampA.size() * timeStampB.size());

	// sort by descending order so we can sequentially pop off skipped sample's indices
	sort(skip.begin(), skip.end(), std::greater<>());

	size_t skipMe = timeStampB.size() + 1;
	if(!skip.empty()) {
		skipMe = skip.back();
		skip.pop_back();
	}

	for (size_t i=0; i<timeStampA.size(); ++i) {
		for (size_t j=0; j<timeStampB.size(); ++j) {
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

// Wrapper with no skips
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
std::tuple<uint64_t,uint64_t,int64_t> findOffset(std::vector<T> dt) {
	// returns <offsetBin, N_occurrences, standardDeviation>
	// - NTK
	char hname[256];
	char htitle[256];
	// nbins is rounded
	//art::ServiceHandle<art::TFileService> tfs; // for drawing the histograms to file <@@>
	//art::TFileDirectory tdir2 = tfs->mkdir("TimeOffsetHistograms",""); // for drawing the histograms to file <@@>

	static int histIndex = 0;
	sprintf(hname,"Time_Offset_%d",histIndex);
	sprintf(htitle,"Fragment Time Differences for pair #%d",histIndex);
	histIndex++;
	const auto [min, max] = std::minmax_element(dt.begin(), dt.end());
	TH1I dtHist(hname, htitle, abs(*max - *min + 1), *min-0.5, *max+0.5);

	for(auto x : dt)
		dtHist.Fill(x);

	//tdir2.make<TH1I>(dtHist); // this draws the histogram <@@>
	auto indexBin = dtHist.GetMaximumBin();
	auto N_occur = 0;
//	NTK: add bins around maximum within CAEN resolution
	for(auto sameBin = indexBin - timeUncertainty; sameBin <= indexBin + timeUncertainty; ++sameBin) {
		N_occur += dtHist.GetBinContent(sameBin);
	}
	return { indexBin, N_occur, binToTime(dt, indexBin) };
}
// Returns the last time synchronized item
template <typename T>
std::tuple<uint64_t,uint64_t> indexOfLastSync(const std::vector<T> A, const std::vector<T> B) {
	uint64_t aIndex = 0;
	uint64_t bIndex = 0;

	{
		bool exitLoop = false;
		for(int aEvent = A.size() - 1; aEvent >= 0; --aEvent) {
			for(int bEvent = B.size() - 1; bEvent >= 0; --bEvent) {
				if(abs(A[aEvent] - B[bEvent]) <= timeUncertainty) {
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

template <typename T>
std::tuple<uint64_t, int64_t> xcorr(std::vector<T> father, std::vector<T> child) {
	std::vector<int64_t> dt = calcDifferences<int64_t>(father, child);
	auto [indexBin, N_occur, offset]  = findOffset(dt);
	return { N_occur, offset };
}

// Calibrate the cross correlation routine
template <typename T>
std::tuple<uint64_t, uint64_t, uint64_t, int64_t> calibrateXcorr(std::vector<T> grandfather, std::vector<T> child, double percentOverlap) {
	// Outputs: { index of last synced event, timeOffset }
	uint64_t index = 0;
	uint64_t N_occur = 0;
	int64_t timeOffset = 0;

	size_t N_compare = child.size();
	// Check that the child vector is smaller than the grandfather vector
	if(child.size() > grandfather.size()) return {2*N_compare, 2*N_compare, 0, 0};
	// Check that percentOverlap is less than 1 and that we are checking for at least 1 overlapping sample
	if(child.size() * percentOverlap < 1 || percentOverlap > 1) return {2*N_compare, 2*N_compare, 0, 0};

	// iterate over whole setup until we find enough overlapping events

	{ uint64_t maxOccur = 0;

		for(size_t startSample = 0; startSample < grandfather.size()-N_compare; ++startSample) {
			auto begin = grandfather.begin() + startSample;
			std::vector<T> father(begin, begin+N_compare);
			std::vector<int64_t> dt = calcDifferences<int64_t>(father, child);
			// NTK <@@>
			for(size_t tmp = 0; tmp < N_compare; ++tmp) {
				std::cout << father[tmp] << " - " << child[tmp] << " = " << father[tmp] - child[tmp] << "\n";
			}
			std::cout << std::endl;

			auto [indexBin, N_occur, offset]  = findOffset(dt);
			if(N_occur > percentOverlap * N_compare) { // found enough overlapping events!
				maxOccur = N_occur;
				timeOffset = offset;
				index = startSample; // sets index of last synced event to the front of the last synced set
				break;
				if(maxOccur < N_occur) { // tries to get highest number of occurrences
					maxOccur = N_occur;
					timeOffset = offset;
					index = startSample;
				}
			}
		}
		N_occur = maxOccur;
	}
	for( auto &timeStamp : child ) // offsets all of the child timestamps
		timeStamp += timeOffset;
	auto begin = grandfather.begin() + index;
	std::vector<T> father(begin, begin+N_compare);
	auto [fIndex, cIndex] = indexOfLastSync(father, child);
	return { index+fIndex, cIndex, N_occur, timeOffset };
}
