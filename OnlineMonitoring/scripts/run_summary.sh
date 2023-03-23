#!/bin/bash
# Run this script on a daq machine emphatic-daq-0*
run=$1
DIR=/data2/OutputData/
TMPFILE=${run}stats.log

nonew=0
spill=1
while [ $nonew -le 5 ]
do
spill_format=$(printf "%04d" $spill)
if test -n "$(find $DIR -maxdepth 1 -name "emdaq_otsdaq_rootOutput_r${run}_s${spill_format}*" -print -quit)"
then
  echo running "$DIR/emdaq_otsdaq_rootOutput_r${run}_s${spill_format}*"
  art -c rawEventDump_v1.fcl $DIR/emdaq_otsdaq_rootOutput_r${run}_s${spill_format}_*  | grep ID | tee $TMPFILE
  echo Extracting data from spill $run $spill 
  spill=$(( $spill + 1 ))
  nonew=0
else
  echo No new files yet... 
  nonew=$(( $nonew +1 ))
fi
sleep 3
done

./analyze_log.sh $run 
