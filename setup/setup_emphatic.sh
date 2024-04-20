# EMPHATIC software environment setup script
# to be sourced.  Note: hacked from DUNE setup script.
# Source this file to set the basic configuration needed by emphaticsoft.

# Look for obsolete lines in login scripts
process_obsolete () {
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
        echo -e "\033[0;31mIMPORTANT! We detect obsolete lines in your login    scripts $checkthesefiles \033[0m"
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
    return 0
}

# Process the OS on machine we are sourcing this script.
# Acceptable OS are: al9 (Alma Liunx 9), sl7 (Scientific Linux 7)
# We need to know the OS to determine how to proceed with setup
process_os () {
    redhat=`cat /etc/redhat-release 2>/dev/null`
    if [[ "$redhat" =~ "AlmaLinux release 9." ]]; then
        os="al9"
    elif [[ "$redhat" =~ "Scientific Linux release 7." ]]; then
        os="sl7"
    else
        echo "Unsupported OS: $redhat"
        return 1
    fi
    return 0
}

# Set up the default environment for emphaticsoft
set_defaults () {
    # current location of emphaticsoft external products in CVMFS
    CVMFS_EMPHATIC_PRODUCTS_DIR="/cvmfs/emphatic.opensciencegrid.org/products"

    # fermilab common products
    CVMFS_FERMILAB_DIR="/cvmfs/fermilab.opensciencegrid.org/products/common/db"

    # Set up ups for EMPHATIC external produccts
    for dir in $CVMFS_EMPHATIC_PRODUCTS_DIR;
    do
      if [[ -f $dir/setup ]]; then
        echo "Setting up emphatic UPS area... ${dir}"
        source $dir/setup
        break
      fi
    done
    # need also the common db in $PRODUCTS
    export PRODUCTS=${PRODUCTS}:${CVMFS_FERMILAB_DIR}

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
}

# Main function
main () {
    process_os
    if [[ $? -ne 0 ]]; then
        return 1
    fi
    
    # Location of EMPHATICSOFT
    CVMFS_EMPHATICSOFT_DIR="/cvmfs/emphatic.opensciencegrid.org/emphaticsoft"
    
    # If we're on AL9, just add the SL7 container executable to the PATH.
    # In the longer term we'll set up a native AL9 ecosystem with Spack.
    if [[ "$os" == "al9" ]]; then
        al9bin=${CVMFS_EMPHATICSOFT_DIR}/setup
        if [[ ! "$PATH" =~ "$al9bin" ]]; then
            export PATH=$al9bin:$PATH
        fi
        echo
        echo -e "         \033[1;34m\033[4mWELCOME TO ALMA LINUX 9\033[0m     "
        echo
        echo -e "\e[00;95m Use sl7-emph command to start containerized Scientific Linux 7.\033[0m"
        echo -e "% \033[1;32msl7-emph -h;\033[m for additional information."
        echo
        return 0
    fi

    process_obsolete
    set_defaults

    return 0
}

main $*