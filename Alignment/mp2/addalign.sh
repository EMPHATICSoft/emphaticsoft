#!/bin/bash

file1=$1
file2=$2
name=$3

first=0

if [ -z "$name" ]; then
  name="AlignAdded.txt"
fi

if [ -f $name ]; then
    rm $name
fi

# Open file1 on file descriptor 3 and file2 on file descriptor 4
while IFS= read -r line1 <&3 && IFS= read -r line2 <&4; do

  if [ $first -le 1 ]; then
    printf "$line1\n" >> $name
  fi

  if [ $first -gt 1 ]; then
    x1=0
    y1=0
    z1=0
    dphi1=0
    detheta1=0
    dpsi1=0
    x2=0
    y2=0
    z2=0
    dphi2=0
    detheta2=0
    dpsi2=0

    sta=""
    sen=""
    pla=""

    # Add your processing logic here, using $line1 and $line2
    IFS=' '
    col1=0
    col2=0
    for word in $line1; do
      if [ $col1 -eq 0 ]; then
        sta=$word
      fi
      if [ $col1 -eq 1 ]; then
        sen=$word
      fi
      if [ $col1 -eq 2 ]; then
        pla=$word
      fi
      if [ $col1 -eq 3 ]; then 
        x1=$word
      fi
      if [ $col1 -eq 4 ]; then
        y1=$word
      fi
      if [ $col1 -eq 5 ]; then
        z1=$word
      fi
      if [ $col1 -eq 6 ]; then
        dphi1=$word
      fi
      if [ $col1 -eq 7 ]; then
        detheta1=$word
      fi
      if [ $col1 -eq 8 ]; then
        dpsi1=$word
      fi
      ((col1+=1))
    done
    for word in $line2; do
      if [ $col2 -eq 3 ]; then
        x2=$word
      fi
      if [ $col2 -eq 4 ]; then
        y2=$word
      fi
      if [ $col2 -eq 5 ]; then
        z2=$word
      fi
      if [ $col2 -eq 6 ]; then
        dphi2=$word
      fi
      if [ $col2 -eq 7 ]; then
        detheta2=$word
      fi
      if [ $col2 -eq 8 ]; then
        dpsi2=$word
      fi
      ((col2+=1))
    done
    xsum=$(echo "$x1 + $x2" | bc)
    ysum=$(echo "$y1 + $y2" | bc)
    zsum=$(echo "$z1 + $z2" | bc)
    dphisum=$(echo "$dphi1 + $dphi2" | bc)
    dethetasum=$(echo "$detheta1 + $detheta2" | bc)
    dpsisum=$(echo "$dpsi1 + $dpsi2" | bc)
    printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" $sta $sen $pla $xsum $ysum $zsum $dphisum $dethetasum $dpsisum  >> $name
  fi
  ((first+=1))
done 3<"$file1" 4<"$file2"
