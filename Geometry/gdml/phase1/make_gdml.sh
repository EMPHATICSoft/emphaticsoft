#!/bin/bash

$(dirname $0)/generate_gdml.pl -o phase1.$$.gdml 
$(dirname $0)/../make_gdml.pl -i phase1.$$.gdml -o phase1-test.gdml 
/bin/rm -f phase1.$$.gdml phase1_*.gdml
