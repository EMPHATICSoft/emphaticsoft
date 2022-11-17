#!/bin/bash
#-m: 0: w/o magnet; 1: w/ magnet
#-t: 0: w/o target; 1: graphite; 2: CH2

$(dirname $0)/generate_gdml.pl -o phase1b.$$.gdml -m 1 -t 1
$(dirname $0)/../make_gdml.pl -i phase1b.$$.gdml -o phase1b.gdml 
/bin/rm -f phase1b.$$.gdml phase1_*.gdml
