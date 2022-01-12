#!/bin/bash

$(dirname $0)/generate_gdml.pl -o phase2.$$.gdml 
$(dirname $0)/../make_gdml.pl -i phase2.$$.gdml -o phase2-test.gdml 
/bin/rm -f phase2.$$.gdml phase2_*.gdml
