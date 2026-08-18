#!/bin/sh

run=${1}
N_WORKERS=${N_WORKERS:=2}
EMPHATIC_SOFT=${EMPHATIC_SOFT:=/exp/emph/app/users/${USER}/emphaticsoft}
EMPHATIC_BUILD=${EMPHATIC_BUILD:=/exp/emph/app/users/${USER}/build}

iteration=1
while true; do
	# Scale errors on first iteration only
	factor=10
	[ ${iteration} -eq 1 ] \
		&& sed "s/.*scaleerrors .*/scaleerrors ${factor}/" -i steer_all.txt \
		|| sed "s/.*scaleerrors .*/!scaleerrors ${factor}/" -i steer_all.txt

	# Iterate forward (calculate new alignment constants)
	nice -n 20 ./step.sh ${run} || { echo "Failed to make step. Exiting"; exit 1; }
	# Outputs: "m*.bin" "*.log" "NewlyAligned.txt"
	[ -s "NewlyAligned.txt" ] || { echo "Failed to produce newly aligned parameters. Exiting"; exit 1; }

	# Archive files
	mkdir -p iteration
	archive="iteration/${iteration}"
	mkdir -p ${archive}
	for milleFile in m*.bin; do
		mv ${milleFile} "${archive}"
	done
	for millepede in millepede.*; do
		mv ${millepede} "${archive}"
	done
	for logFile in *.log; do
		mv ${logFile} "${archive}"
	done
	mv "NewlyAligned.txt" "${archive}"

	# Update alignment constants and iterate
	cp "${archive}/NewlyAligned.txt" "${EMPHATIC_SOFT}/ConstBase/Align/SSDAlign_1c_${run}.txt"
	iteration=$((${iteration} + 1))
	[ $iteration -eq 50 ] && { echo "Failed to converge within ${iteration} iterations. Exiting"; exit 1; }
done
