#!/bin/bash
# #-h: help

MAGOPT=1
TARGET=1
ALIGN=0
OUTNAMESET=0

while getopts "o:t:m:a:h" opt; do
    case $opt in
	h) 
	    $(dirname $0)/generate_gdml.pl --help
	    exit 0
	    ;;
	o) 
	    OUTNAME=$OPTARG
	    OUTNAMESET=1
	    ;;
	t) 
	    TARGET=$OPTARG
	    ;;
	a)
	    ALIGN=$OPTARG
	    ;;
	m)
	    MAGOPT=$OPTARG
	    ;;
	\?)
	    echo "Invalid option: -$OPTARG" >& 2
	    exit 1
	    ;;
    esac
done

if [[ "$OUTNAMESET" == 0 ]]; then
    echo "output file name (-o option) missing"
    exit 1
fi

$(dirname $0)/generate_gdml.pl $@
/bin/mkdir tmpgdml
/bin/mv $OUTNAME tmpgdml/.
$(dirname $0)/../make_gdml.pl -i "tmpgdml/${OUTNAME}" -o $OUTNAME
/bin/rm -f tmp_*.gdml
/bin/rm -fr tmpgdml
