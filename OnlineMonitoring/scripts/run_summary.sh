#!/bin/bash
run=$1
DIR=/emph/data/data/June2022/OutputData
TMPFILE=${run}stats.log
LOGFILE=mylog.log
n=$(ls $DIR | grep _r${run}_ |wc -l)
# echo Processing $n spills in run $run
rm $TMPFILE
for spill in $(seq $n)
do
  art -c /emph/app/users/twester/build/fcl/rawEventDump_v1.fcl $DIR/emdaq_otsdaq_rootOutput_r${run}_s${spill}_* | grep ID > $TMPFILE 
  echo Extracting data from spill $run $spill >> $LOGFILE
  ./analyze_log.sh $run >> $LOGFILE
done

