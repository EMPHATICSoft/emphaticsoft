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

# Source this file to set the basic configuration needed by EMPHATICSoft 

# current location of emphaticsoft in CVMFS
FERMIOSG_EMPHATICSOFT_DIR="/cvmfs/emphatic.opensciencegrid.org/products"

# fermilab common products
FERMIOSG_DIR="/cvmfs/fermilab.opensciencegrid.org/products/common/db"

# Set up ups for EMPHATIC
for dir in $FERMIOSG_EMPHATICSOFT_DIR;
do
  if [[ -f $dir/setup ]]; then
    echo "Setting up emphatic UPS area... ${dir}"
    source $dir/setup
    break
  fi
done

# need also the common db in $PRODUCTS
export PRODUCTS=${PRODUCTS}:${FERMIOSG_DIR}

# Add current working directory (".") to FW_SEARCH_PATH
if [[ -n "${FW_SEARCH_PATH}" ]]; then
  FW_SEARCH_PATH=`dropit -e -p $FW_SEARCH_PATH .`
  export FW_SEARCH_PATH=.:${FW_SEARCH_PATH}
else
  export FW_SEARCH_PATH=.
fi

# Set up the basic tools that will be needed
if [ `uname` != Darwin ]; then

  # Work around git table file bugs.
  export PATH=`dropit git`
  export LD_LIBRARY_PATH=`dropit -p $LD_LIBRARY_PATH git`
  setup git
fi

setup gitflow # do we need this?

# Define environment variables that store the standard experiment name.
export JOBSUB_GROUP=emph
export EXPERIMENT=emph     # Used by ifdhc
export SAM_EXPERIMENT=emph

# For ifbeam
export SSL_CERT_FILE=""

# For GL-based event display
export LIBGL_ALWAYS_INDIRECT=1

# Additional setup
setup ninja
setup art_cpp_db_interfaces v1_4_0
setup gdb v12_1
