#!/bin/bash

$(dirname $0)/generate_gdml.pl -o phase1b.$$.gdml 
$(dirname $0)/../make_gdml.pl -i phase1b.$$.gdml -o phase1b.gdml 
/bin/rm -f phase1b.$$.gdml phase1_*.gdml
