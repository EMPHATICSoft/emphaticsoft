#!/bin/bash
#submitReconstruction.sh: Submit an ART job that takes no input to the grid

#Help text
if [[ $1 == "-h" || $1 == "--help" ]]; then
  echo "Usage: submitReconstruction.sh template.fcl input.root [moreInput.root...]"
  echo "Usage: submitReconstruction.sh template.fcl #Reads file list from stdin"
  exit 1
fi

#Get arguments from the command line
configFile=$1
fileList="fileList.txt"

#Make list of .root files to process on the grid, one per job
#while read fileName
#do
#  if [[ ! $fileName =~ "/pnfs/emphatic/persistent/users/*" ]]; then
#    echo "Got file named $fileName that is not in /pnfs/emphatic/persistent/users/.  Grid jobs can only see files on /pnfs!"
#    exit 2
#  fi
#  echo $fileName >> fileList.txt
#done < /dev/stdin

cat - > $fileList
nJobs=$(wc -l $fileList | sed -E 's/([[:digit:]]+) (.*)/\1/g')

#Convert file names to xrootd paths
sed -i 's/^\/pnfs\/emphatic\/\(.*\)\.root/root:\/\/fndca1.fnal.gov:1094\/\/\/pnfs\/fnal.gov\/usr\/emphatic\/\1\.root/g' $fileList

#Figure out where code comes from and where to put temporary files
outFileName="testReconstruction.root"
gridScriptName="basicReconstruction.sh"
hostOutDir="/pnfs/emphatic/persistent/users/${USER}/testReconstruction" #TODO: Set this based on the user's environment with a fallback in case it's not on /pnfs
#codeDir="/exp/emph/app/users/aolivier/batchSubmissionDevelopment"
cd $(dirname $BASH_SOURCE)/../..
codeDir=$(pwd)
cd -

#Prepare files needed for grid submission
source $codeDir/emphaticsoft/GridSubmission/gridSubFunctions.sh
makeOutputDirectory $hostOutDir
makeTarball $codeDir $hostOutDir
makeWrapperBoilerplate $codeDir > ${gridScriptName} #Overwrite grid script if it already exists from a previous job submission
jobsubArgs=$(getBasicJobsubArgs $hostOutDir)

#Add reconstruction-specific lines to wrapper
echo "INPUT_FILE=\$(head -n \$((PROCESS+1)) \${CONDOR_DIR_INPUT}/$(basename $fileList) | tail -n -1) || exit 2" >> ${gridScriptName}
echo "echo \"***** finished finding input file *****\"" >> ${gridScriptName}
echo "art -c $(basename ${configFile}) -o ${outFileName} \${INPUT_FILE} || exit 3" >> ${gridScriptName}
echo "echo \"***** finished ART job *****\"" >> ${gridScriptName}

jobsub_submit -N ${nJobs} -f dropbox://$(pwd)/${configFile} -f dropbox://$(pwd)/${fileList} $jobsubArgs file://$(pwd)/${gridScriptName}
