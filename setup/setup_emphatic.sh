# EMPHATIC software environment setup script
# to be sourced.  Note: hacked from DUNE setup script.

# Look for obsolete lines in login scripts
checkthesefiles=
for file in ~/.bash_profile ~/.bash_login ~/.profile ~/.shrc ~/.bashrc;
do
    if [[ -f $file ]]; then
        found=0
        while IFS= read -r line; do
            if [[ $line == *"setup login"* && $line != *"#"* ]]; then
                #echo $line in $file
                found=1
            fi
            if [[ $line == *"setup shrc"* && $line != *"#"* ]]; then
                #echo $line in $file
                found=1
            fi
        done < "$file"
        #echo $found
        if [[ $found == 1 ]]; then
            #echo file $file
            checkthesefiles="$file $checkthesefiles"
            #echo checkthesefiles $checkthesefiles
        fi
    fi
done
if [[ -n $checkthesefiles ]]; then
    echo -e "\033[0;31mIMPORTANT! We detect obsolete lines in your login scripts $checkthesefiles \033[0m"
    echo "Please look for the following lines or something similar in $checkthesefiles and comment those lines out and relogin. Those lines may cause a conflict with cvmfs later."
    echo "pa=/grid/fermiapp/products/common/etc"
    echo "if [  -r \"$pa/setups.sh\" ]"
    echo "then"
    echo "  . \"$pa/setups.sh\" "
    echo "  if ups exist login"
    echo "  then"
    echo "      setup login"
    echo "  fi"
    echo "fi"
    echo or
    echo "pa=/grid/fermiapp/products/common/etc"
    echo "if [ -r \"$pa/setups.sh\" ]"
    echo "then"
    echo " . \"$pa/setups.sh\" "
    echo "fi"
    echo ""
    echo "if ups exist shrc"
    echo "then"
    echo " setup shrc"
    echo "fi"
    echo 
    echo "For more information, please check https://cdcvs.fnal.gov/redmine/issues/15028 or contact Tingjun (tjyang@fnal.gov) or Tom (trj@fnal.gov)."
fi

# Source this file to set the basic configuration needed by LArSoft 
# and for the DUNE-specific software that interfaces to LArSoft.

# current location of emphaticsoft in CVMFS

FERMIOSG_EMPHATICSOFT_DIR="/cvmfs/emphatic.opensciencegrid.org/products/"

#FERMIOSG_EMPH_DIR="/emph/app/setup"

#EMPHATIC_BLUEARC_DATA="/emph/data"

# Set up ups for EMPHATIC

for dir in $FERMIOSG_EMPHATICSOFT_DIR; #REPLACE WITH EMPHATIC EVENTUALLY
do
  if [[ -f $dir/setup ]]; then
    echo "Setting up emphatic UPS area... ${dir}"
    source $dir/setup
    break
  fi
done

# need also the common db in $PRODUCTS

#for dir in $FERMIOSG_LARSOFT_DIR 
#do
#  if [[ -f $dir/setup ]]; then
#    common=`dirname $dir`/common/db
#    if [[ -d $common ]]; then
#      export PRODUCTS=`dropit -p $PRODUCTS common/db`:`dirname $dir`/common/db
#    fi
#    break
#  fi
#done


# Set up ups for DUNE

#for dir in $FERMIOSG_DUNE_DIR 
#do
#  if [[ -f $dir/setup ]]; then
#    echo "Setting up DUNE UPS area... ${dir}"
#    source $dir/setup
#    break
#  fi
#done

# Add current working directory (".") to FW_SEARCH_PATH

if [[ -n "${FW_SEARCH_PATH}" ]]; then
  FW_SEARCH_PATH=`dropit -e -p $FW_SEARCH_PATH .`
  export FW_SEARCH_PATH=.:${FW_SEARCH_PATH}
else
  export FW_SEARCH_PATH=.
fi

# Add DUNE data path to FW_SEARCH_PATH
#
#if [[ -d "${DUNE_BLUEARC_DATA}" ]]; then

#    if [[ -n "${FW_SEARCH_PATH}" ]]; then
#      FW_SEARCH_PATH=`dropit -e -p $FW_SEARCH_PATH ${DUNE_BLUEARC_DATA}`
#      export FW_SEARCH_PATH=${DUNE_BLUEARC_DATA}:${FW_SEARCH_PATH}
#    else
#      export FW_SEARCH_PATH=${DUNE_BLUEARC_DATA}
#    fi

#fi

# Set up the basic tools that will be needed
#
if [ `uname` != Darwin ]; then

  # Work around git table file bugs.

  export PATH=`dropit git`
  export LD_LIBRARY_PATH=`dropit -p $LD_LIBRARY_PATH git`
  setup git
fi

setup gitflow
#setup mrb

# this is now setup by jobsub_client. setup pycurl

# Define the value of MRB_PROJECT. This can be used
# to drive other set-ups. 
# We need to set this to 'larsoft' for now.

#export MRB_PROJECT=larsoft

# Define environment variables that store the standard experiment name.

export JOBSUB_GROUP=emph
export EXPERIMENT=emph     # Used by ifdhc
export SAM_EXPERIMENT=emph

# For Art workbook

#export ART_WORKBOOK_OUTPUT_BASE=/dune/data/users
#export ART_WORKBOOK_WORKING_BASE=/dune/app/users
#export ART_WORKBOOK_QUAL="s2:e5:nu"

# For database

#export DBIWSPWDFILE=/
#export DBIWSURL=http://dbdata0vm.fnal.gov:8116/LBNE35tCon/app/
#export DBIWSURLINT=http://dbdata0vm.fnal.gov:8116/LBNE35tCon/app/
#export DBIWSURLPUT=http://dbdata0vm.fnal.gov:8117/LBNE35tCon/app/
#export DBIQEURL=http://dbdata0vm.fnal.gov:8122/QE/dune35t/prod/app/SQ/
#export DBIHOST=ifdbprod.fnal.gov
#export DBINAME=dune35t_prod
#export DBIPORT=5442
#export DBIUSER=dune_reader
#export DBIPWDFILE='~jpaley/dune/db/proddbpwd'

# For ifbeam

export SSL_CERT_FILE=""

# to ensure compatibility on OSG nodes missing OS libraries

#setup dune_oslibs v1_0_0

# set up gdb and ninja out of ups

#setup gdb
setup ninja
#setup geant4 v4_10_6_p01c -q e19:prof
#setup root v6_22_06a -q e19:p383b:prof
setup libwda v2_29_1
setup art_cpp_db_interfaces v1_4_0

#setup cmake v3_18_2
#setup art v3_06_03 -q prof:e19
