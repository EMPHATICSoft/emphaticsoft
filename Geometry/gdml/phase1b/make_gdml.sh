#!/bin/bash
#-m: 0: w/o magnet; 1: w/ magnet
#-t: 0: w/o target; 1: graphite; 2: Be; 3: CH2

$(dirname $0)/generate_gdml.pl -o phase1b.$$.gdml -m 1 -t 1
#ls

# Specify the input wildcard pattern
input_pattern="phase1_*.gdml"

# Process each input file
find .. -name 'phase1_*.gdml' -print0 | while IFS= read -r -d '' input_file; do
	echo $input_file
	# Extract version tag from input filename using sed
	version_tag=$(echo "$input_file" | sed 's/.*-\([[:alnum:]]*\)\.gdml/\1/')
	echo $version_tag
	
	output_file="phase1b-${version_tag}.gdml"
	echo $output_file
	
	$(dirname $0)/../make_gdml.pl -i phase1b.$$.gdml -o $output_file
	
	break
done

/bin/rm -f phase1b.$$.gdml phase1_*.gdml
