#!/bin/bash

mpdir=$(pwd)

begin=$1
mkdir -p $begin
cd $begin

run=$2
declare -i it=0
cont="true"
nominal="SSDAlign_1c.txt"

emphdir=$3

path="/pnfs/emphatic/scratch/users/rchirco/textfiles"
textfile="${path}/all${run}.txt"
echo "Using $textfile"
logfile="prcj${run}.out"

while [ "$cont" == "true" ]; do
  cd $begin

  if [ $it -gt 0 ]; then
    sn="shift${it}"
    mkdir $sn
    cd $sn
  fi

  [ -f "$logfile" ] && rm "$logfile"
  [ -f "m004.bin" ] && rm "m004.bin"

  ( art -c prod_reco_caf_job.fcl -S $textfile > $logfile ) &
  wait
  echo "Done waiting..."

  newname="m${run}_s${it}.bin"
  mv m004.bin $newname

  mkdir -p "$mpdir/at_once"

  cp $newname "$mpdir/at_once"

  cd $mpdir

  sed -i "23c\at_once/$newname" steer_all.txt

  scale=10
  if [ $it -eq 0 ]; then
    sed -i "30c\scaleerrors $scale ! inflate errors" steer_all.txt
    ./pede steer_all.txt
  else
    sed -i "30c\!scaleerrors $scale ! inflate errors" steer_all.txt
    ./pede steer_all.txt
  fi
  
  mp=millepede.res

  mdir="m_${run}_all"
  ssddir="ssd_${run}_all"

  mkdir -p $mdir
  mkdir -p $ssddir

  cp $mp "$mdir/$mp-$run-$it.txt"

  source mp2emph.sh $mp true

  newalign="$nominal-$run-ATONCE-$it"
  if [ $it -gt 0 ]; then
    declare -i prev=$it-1;
    previous="$ssddir/$nominal-$run-ATONCE-$prev"

    source addalign.sh SSDAlign_1c.txt $previous

    mv AlignAdded.txt $newalign
  else
    mv SSDAlign_1c.txt $newalign
    echo "Made shift $newalign"
  fi  

  cp $newalign "$emphdir/emphaticsoft/ConstBase/Align/"
  cp $newalign $ssddir
  # Stopping case: loop through millepede.res and compare column 4 with 5 until all in 4 are less than respective value in 5

  while IFS=' ' read -r label par c3 c4 c5 c6; do
    if [[ -n "$c4" && -n "$c5" ]];then 
      c4value=$(awk "BEGIN {printf \"%.9f\", $c4}")
      c5value=$(awk "BEGIN {printf \"%.9f\", $c5}")

      abs_c4value=${c4value#-}
      cmp=$(echo "$abs_c4value < $c5value" | bc)
      if [[ "$cmp" -eq 1 ]]; then
        cont="false"
      else 
        echo "Not yet: $c4value > $c5value"
        cont="true"
	echo "Gonna break"
        break
      fi 
    fi

  done < <(tail -n +2 $mp)

  if [ $it -eq 50 ]; then
    echo "50th iteration reached...well!"
    cont="false"
  fi

  if [ "$cont" == "true" ]; then 
    echo "Iterating again..."
    mv $newalign "$emphdir/emphaticsoft/ConstBase/Align/SSDAlign_1c.txt"
  fi

  it=$it+1
done

echo "It's dooooone!"
