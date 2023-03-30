#!/bin/bash
# Run this script on a daq machine emphatic-daq-0*
echo usage: ./RunSummary [run number]
run=$1
#DIR=/data2/OutputData/
DIR=/home/user/OnMon_test_data/OutputData
#DIR=/home/user/OnMon_test_data/deleteMe_testingDataDir
nonew=0
spill=1

TMPFILE=${run}stats.log
if [ -e ${run}stats.log ] 
then
#  rm $TMPFILE
  spill=$(grep "Subrun" $TMPFILE | tail -n 1 | grep -o 'Subrun [0-9]*' | sed 's/.* //')
  echo Processed till Subrun $spill
  spill=$(( $spill +1 ))
  echo continue from $spill
fi

while [ $nonew -le 5 ]
do
spill_format=$(printf "%04d" $spill)
if test -n "$(find $DIR -maxdepth 1 -name "emdaq_otsdaq_rootOutput_r${run}_s${spill_format}*" -print -quit)"
then
  echo running "$DIR/emdaq_otsdaq_rootOutput_r${run}_s${spill_format}*"
  art -c rawEventDump_v1.fcl $DIR/emdaq_otsdaq_rootOutput_r${run}_s${spill_format}_*  | grep ID | tee -a $TMPFILE
  echo Extracting data from spill $run $spill 
  spill=$(( $spill + 1 ))
  nonew=0
else
  echo No new files yet... 
  nonew=$(( $nonew +1 ))
fi
sleep 30
done

./analyze_log.sh $run 
#./logfile_analyzer.py $run 
