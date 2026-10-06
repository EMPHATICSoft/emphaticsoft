#!/bin/sh

[ $# -ne 1 ] && exit 1
run=${1}
N_WORKERS=${N_WORKERS:=2}
EMPHATIC_SOFT=${EMPHATIC_SOFT:=/exp/emph/app/users/${USER}/emphaticsoft}
EMPHATIC_BUILD=${EMPHATIC_BUILD:=/exp/emph/app/users/${USER}/build}

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
	sed -i "s/MilleName: \"m.*.bin\"/MilleName: \"${milleName}\"/" "${EMPHATIC_BUILD}/fcl/SingleTrackAlignmentKalman.fcl"
	# Skip if bin already exists (continuing old job)
	# Add entry to the pede steering file
	if [ -s "${milleName}" ]; then
		sed "s/Cfiles/Cfiles\n${milleName}/" -i steer_all.txt
	else
		( nice -n 20 art -c prod_reco_caf_job.fcl $file | tee "${subrun}.log" \
				&& sed "s/Cfiles/Cfiles\n${milleName}/" -i steer_all.txt) &
		sleep 4
	fi
	pgrep -P $$ && [ "$(pgrep -P $$ | wc -l)" -gt ${N_WORKERS} ] && wait
done

# Pede time!
rm millepede.*
${MILLEPEDE_FQ_DIR}/bin/pede -i steer_all.txt
# Check that millepede succeeded
[ -z 'millepede.res' ] && { echo "Did not converge. Millepede failed."; exit 1; }

# Check if residuals are small enough
anyBigger='false'
FIFO=$(mktemp -u) # FIFO is necessary to return global variable ${anyBigger}
mkfifo ${FIFO} && trap "rm ${FIFO}" EXIT INT TERM
tail -n +2 millepede.res | tr -s ' ' | cut -d ' ' -f 5,6 > ${FIFO} &
while read -r VALUE ERROR; do
	[ -z ${VALUE} ] || [ -z ${ERROR} ] && continue;
	comparison="$(echo "${VALUE#-} > ${ERROR#-}" | bc -l 2>/dev/null)"
	[ ${comparison} -eq 1 ] && anyBigger='true' break
done < ${FIFO}
[ "${anyBigger}" = 'false' ] && { echo "Successfully converged. Exiting"; exit 1; }
echo "Did not converge. Try again."
unset anyBigger

# Convert residual to SSD alignment file
trap 'rm Shifts.txt' EXIT INT TERM
./mp2emph.sh millepede.res true "Shifts.txt"

# Add alignment constants to original alignment constants
# - why is Robert subtracting in addalign.sh?
./addalign.sh "${EMPHATIC_SOFT}/ConstBase/Align/SSDAlign_1c_${run}.txt" "Shifts.txt" "NewlyAligned.txt"
