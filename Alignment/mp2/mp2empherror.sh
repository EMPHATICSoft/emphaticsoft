#!/bin/bash
file=$1
name=$3
#u=$3
trk3=$2

if [ -z "$trk3" ]; then
  trk3=false
fi

if [ -z "$name" ]; then
  name="SSDAlign_1c_errors.txt"
fi

echo "Writing $name..."

# Rewrite output file and write header row
elements=("#station" "plane" "sensor" "dx" "dy" "dz" "dphi" "detheta" "dpsi")
printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" "${elements[@]}" > $name
echo "" >> $name

dx=""
dy=""
dz=""
dphi=""
c=0

while IFS=' ' read -r label par c3 c4 c5 c6; do 

  # Parse label
  station=""
  plane=""
  sensor=""

  if [ ${#label} -eq 4 ]; then
    station=${label:0:1}
    plane=${label:1:1}
    sensor=${label:2:1} 
  elif [ ${#label} -eq 3 ]; then
    station="0"
    plane=${label:0:1}
    sensor=${label:1:1}
  else
    station="0"
    plane="0"
    sensor="0"
  fi

  if [ -z "$c5" ]; then
    c5=0
  fi

  if [ ${label: -1} -eq 1 ]; then 
    dx=$(awk "BEGIN { print 1.*$c5 }")
    c=$((c+1))
  elif [ ${label: -1} -eq 2 ]; then
    dy=$(awk "BEGIN { print 1.*$c5 }")
    c=$((c+1))
  elif [ ${label: -1} -eq 3 ]; then
    dz=$c5
    c=$((c+1))
  else 
    dphi=$(awk -v var1=$c5 'BEGIN { printf "%.10f\n", 180/3.14159265359*var1 }')
    c=0
    printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" $station $plane $sensor $dx $dy $dz $dphi 0 0 >> $name
    if [ "$trk3" = "false" ]; then
      if [ $station = "5" ] || [ $station = "6" ] || [ $station = "7" ]; then
        printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" $station $plane 1 $dx $dy $dz $dphi 0 0 >> $name
      fi
    fi
  fi

done < <(tail -n +2 $file)

if [ "$trk3" = "false" ]; then
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 5 0 0 0 0 0 0 0 0  >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 5 0 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 5 1 0 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 5 1 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 5 2 0 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 5 2 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 6 0 0 0 0 0 0 0 0  >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 6 0 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 6 1 0 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 6 1 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 6 2 0 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 6 2 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 0 0 0 0 0 0 0 0  >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 0 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 1 0 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 1 1 0 0 0 0 0 0 >> $name
fi

if [ $c -ne 0 ]; then
  echo "No station 7...that's okay..."
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 0 0 0 0 0 0 0 0  >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 0 1 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 1 0 0 0 0 0 0 0 >> $name
  printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" 7 1 1 0 0 0 0 0 0 >> $name
fi

echo "...and done!"
