#!/bin/bash

# Input files
file1="$1"
file2="$2"
name="$3"

if [ -z "$name" ]; then
  name="output.txt"
fi

# Skip the first line and process the rest
{
    # Rewrite output file and write header row
    elements=("#station" "plane" "sensor" "dx" "dy" "dz" "dphi" "detheta" "dpsi")
    printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" "${elements[@]}" > $name
    echo "" >> $name

    # Process the remaining lines
    while IFS= read -r line1 && IFS= read -r line2 <&3; do
        # Extract the first three columns (columns 1-3)
        col1=$(echo "$line1" | awk '{print $1}')
        col2=$(echo "$line1" | awk '{print $2}')
        col3=$(echo "$line1" | awk '{print $3}')

        col4_1=$(echo "$line1" | awk '{print $4}')
        col4_2=$(echo "$line2" | awk '{print $4}')
        col5_1=$(echo "$line1" | awk '{print $5}')
        col5_2=$(echo "$line2" | awk '{print $5}')
        col6_1=$(echo "$line1" | awk '{print $6}')
        col6_2=$(echo "$line2" | awk '{print $6}')
        col7_1=$(echo "$line1" | awk '{print $7}')
        col7_2=$(echo "$line2" | awk '{print $7}')
        col8_1=$(echo "$line1" | awk '{print $8}')
        col8_2=$(echo "$line2" | awk '{print $8}')
        col9_1=$(echo "$line1" | awk '{print $9}')
        col9_2=$(echo "$line2" | awk '{print $9}')	

        sum4=$(echo "$col4_1 + $col4_2" | bc)
        sum5=$(echo "$col5_1 + $col5_2" | bc)
        sum6=$(echo "$col6_1 + $col6_2" | bc)
        sum7=$(echo "$col7_1 + $col7_2" | bc)
        sum8=$(echo "$col8_1 + $col8_2" | bc)
        sum9=$(echo "$col9_1 + $col9_2" | bc)

        printf "%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n" $col1 $col2 $col3 $sum4 $sum5 $sum6 $sum7 $sum8 $sum9 >> $name
    done < <(tail -n +2 "$file1" | grep -v '^$') 3< <(tail -n +2 "$file2" | grep -v '^$')
} 

echo "Result saved to $name"

