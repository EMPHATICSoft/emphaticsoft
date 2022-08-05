#!/bin/bash

$(dirname $0)/generate_gdml.pl -o phase1a.$$.gdml 
$(dirname $0)/../make_gdml.pl -i phase1a.$$.gdml -o phase1a.gdml 
/bin/rm -f phase1a.$$.gdml phase1_*.gdml
