#!/bin/bash

$(dirname $0)/generate_gdml.pl -o SSD.$$.gdml 
$(dirname $0)/../make_gdml.pl -i SSD.$$.gdml -o SSD-test.gdml 
/bin/rm -f SSD.$$.gdml SSD_*.gdml
