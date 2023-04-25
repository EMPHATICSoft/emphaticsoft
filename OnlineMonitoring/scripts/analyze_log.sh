#!/bin/bash
run=$1
TMPFILE=${run}stats.log
n=$(grep SeqID $TMPFILE |wc -l)

# echo
# echo Run $run summary
# echo $n spills total
# echo Run $run summary >> $TMPFILE
# echo $n spills total >> $TMPFILE
ids=( "0 " "1 " "2 " "3 " "4 " "5 " "101 " "102 " "103 " "104 ")
for id in "${ids[@]}"
do
  counts=$(grep "ID $id" $TMPFILE |cut -f 6 -d'=' | cut -f 1 -d',')
  real_spills=$(grep "ID $id" $TMPFILE |wc -l)
  if [ $real_spills -lt 1 ]; then
      continue
  fi
  echo -n " ID: $id "
  sum=0
  for count in $counts
  do
    sum=$((sum+count))
  done
  average=$((sum/real_spills))
  echo -n "Total counts: $sum "
  echo Average count per spill: $average over $real_spills spills
  echo -n " ID: $id " >> $TMPFILE
  echo -n "Total counts: $sum " >> $TMPFILE
  echo Average count per spill: $average over $real_spills spills >> $TMPFILE
done
