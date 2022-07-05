export MERGER_DIR=`pwd`

export MERGER_INPUT_DIR=/emph/data/data/June2022/SSD/
export MERGER_OUTPUT_DIR=`pwd`/

#===== Final PATH definitions
export PATH=$ROOTSYS/bin:${PATH}
export LD_LIBRARY_PATH=$ROOTSYS/lib

source /cvmfs/fermilab.opensciencegrid.org/products/artdaq/setup
setup cmake v3_14_3
setup root v6_22_08d -q e20:p392:prof
#----------------------------------------------------------------------------------------------------

