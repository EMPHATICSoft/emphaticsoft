#!/bin/sh

[ $# -ne 1 ] && exit 1
run=${1}
N_WORKERS=${N_WORKERS:=2}
EMPHATIC_SOFT=${EMPHATIC_SOFT:=/exp/emph/app/users/${USER}/emphaticsoft}
EMPHATIC_BUILD=${EMPHATIC_BUILD:=/exp/emph/app/users/${USER}/build}

# Temporary files (per-job fcls, FIFO, Shifts.txt) are removed on exit
tmpFiles=""
trap 'rm -f ${tmpFiles}' EXIT
# On interrupt, also stop the background art jobs (art runs as a child of each job's subshell).
# PIDs are tracked by hand since $(jobs -p) is empty in a subshell under dash
jobPids=""
trap 'for pid in ${jobPids}; do pkill -TERM -P ${pid}; kill ${pid}; done 2>/dev/null; exit 1' INT TERM HUP
# Count background art jobs that are still running
running() { n=0; for pid in ${jobPids}; do kill -0 ${pid} 2>/dev/null && n=$((n + 1)); done; echo ${n}; }

# Empty the pede steering file of mille bin file entries
sed "/m.*.bin/d" -i steer_all.txt

# Remove most recent round of bin files (assumed unfinished)
for file in $(ls -t m*.bin | head -n ${N_WORKERS}); do
	rm ${file}
done

# Dumb parallelization to make mille files
subrunList="textfiles/all${run}.txt"
for file in $(cat ${subrunList}); do
	# Get subrun from data filename
	subrun=$(printf ${file} | sed -e 's/.*_s//' -e 's/\..*//')
	# Adjust mille filename
	milleName="m${subrun}.bin"
	# Skip if bin already exists (continuing old job)
	if [ ! -s "${milleName}" ]; then
		# Per-job fcl so parallel jobs don't share a MilleName
		jobFcl=$(mktemp --suffix=.fcl "align_${subrun}_XXXXXX")
		tmpFiles="${tmpFiles} ${jobFcl}"
		printf '#include "prod_reco_align_kalman_job.fcl"\nphysics.producers.singletrackalignmentKalman.MilleName: "%s"\n' \
			"${milleName}" > "${jobFcl}"
		( nice -n 20 art -c "$(pwd)/${jobFcl}" $file | tee "${subrun}.log" ) &
		jobPids="${jobPids} $!"
	fi
	# Start the next job as soon as a worker slot frees up
	while [ "$(running)" -ge "${N_WORKERS}" ]; do sleep 5; done
done
# Wait for the last batch of mille jobs before running pede
wait

# Add the finished mille files to the pede steering file
for milleFile in m*.bin; do
	[ -s "${milleFile}" ] && sed "s/Cfiles/Cfiles\n${milleFile}/" -i steer_all.txt
done

# Pede time!
rm millepede.*
${MILLEPEDE_FQ_DIR}/bin/pede -i steer_all.txt
# Check that millepede succeeded
[ -s millepede.res ] || { echo "Millepede failed: no millepede.res. Exiting"; exit 1; }

# Check if residuals are small enough
anyBigger='false'
FIFO=$(mktemp -u) # FIFO is necessary to return global variable ${anyBigger}
mkfifo ${FIFO} && tmpFiles="${tmpFiles} ${FIFO}"
tail -n +2 millepede.res | tr -s ' ' | cut -d ' ' -f 5,6 > ${FIFO} &
while read -r VALUE ERROR; do
	[ -z ${VALUE} ] || [ -z ${ERROR} ] && continue;
	# Convert Fortran E-notation (e.g. 0.19E-02) to something bc understands
	comparison="$(echo "${VALUE#-} > ${ERROR#-}" | sed -e 's/E+/E/g' -e 's/[Ee]/*10^/g' | bc -l 2>/dev/null)"
	[ ${comparison} -eq 1 ] && anyBigger='true' break
done < ${FIFO}
if [ "${anyBigger}" = 'false' ]; then
	# Set by staircase.sh while errors are scaled
	[ -n "${IGNORE_CONVERGENCE}" ] || { echo "Successfully converged. Exiting"; exit 1; }
	echo "Converged within scaled errors. Continuing."
else
	echo "Did not converge. Try again."
fi
unset anyBigger

# Convert residual to SSD alignment file
tmpFiles="${tmpFiles} Shifts.txt"
./mp2emph.sh millepede.res true "Shifts.txt"

# Add alignment constants to original alignment constants
# - why is Robert subtracting in addalign.sh?
./addalign.sh "${EMPHATIC_SOFT}/ConstBase/Align/SSDAlign_1c_${run}.txt" "Shifts.txt" "NewlyAligned.txt"
