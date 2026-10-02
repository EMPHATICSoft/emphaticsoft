#/bin/bash

# $1 is the alignment file
# $2 is a text file with the paths to all universes
#    you want to run over

file1=$1 # Align file
filename="${file1##*/}" # File name
path="${file1%/*}"      # Path
name="${filename%.*}"   # File name without extension

nom="$path/NOM_${name}.txt"
echo "cp $1 $nom"
cp $1 $nom

CFG=emphproduction/scripts/prod_reco_caf_prod6.01_new_job.fcl

# Loop through universes
while IFS= read -r line; do

    echo "Universe: $line"

    # Get universe from SSDAlign_1c_2408_u0.txt for ex.
    [[ $line =~ _u([0-9]+)\.txt$ ]]
    univ=${BASH_REMATCH[1]}

    if [[ -z "$univ" ]]; then 
      if [[ $line =~ _([^_\.]*)\.txt$ ]]; then
        univ="${BASH_REMATCH[1]}"
      fi    
    fi

    # Change alignment file
    echo "cp $line $file1"
    cp $line $file1 

    # Run reconstruction on each universe
    #echo "./exp/emph/app/users/rchirco/v07.00.00/emphprod/emphgridutils/bin/submit_emph_art.py reco /exp/emph/app/users/rchirco/v07.00.00/emphaticsoft/CAFMaker/grid_job.fcl --stdin < demoSimOutput.txt --output=/pnfs/emphatic/scratch/users/rchirco/${station}${plane}${sensor}${view}/Syst_${station}${plane}${sensor}${view}_${universe}"

    echo "./emphgridutils/bin/submit_emph_art.py reco "$CFG" data_r2408.txt --output /pnfs/emphatic/scratch/users/rchirco/SystTest/Univ${univ} --code-dir "$EMPH_CODE_DIR" --build-dir "$EMPH_BUILD_DIR" "
    #echo "CFG = $CFG"
    #echo "EMPH_CODE_DIR = $EMPH_CODE_DIR"
    #echo "EMPH_BUILD_DIR = $EMPH_BUILD_DIR"
    ./emphgridutils/bin/submit_emph_art.py reco "$CFG" data_r2408.txt --output /pnfs/emphatic/scratch/users/rchirco/SystTestZOnlyByMountMore/Univ${univ} --code-dir "$EMPH_CODE_DIR" --build-dir "$EMPH_BUILD_DIR"

    # Change back alignment file
    echo "cp $nom $file1"
    echo
    cp $nom $file1
done < $2

echo "終わりました!"
#echo "And...done...!"
