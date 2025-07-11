#!/bin/bash
#-m: 0: w/o magnet; 1: w/ magnet
#-t: 0: w/o target; 1: graphite; 2: CH2
#-h: help

$(dirname $0)/generate_gdml.pl -o phase1c.$$.gdml -m 1 -t 1
$(dirname $0)/../make_gdml.pl -i phase1c.$$.gdml -o phase1c.gdml 
/bin/rm -f phase1c.$$.gdml phase1_*.gdml
