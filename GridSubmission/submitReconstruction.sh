#!/bin/bash
#submitReconstruction.sh: Submit an ART job that takes no input to the grid

#Help text
if [[ $1 == "-h" || $1 == "--help" || $# < 2 ]]; then
  echo "Usage: submitReconstruction.sh template.fcl input.root [moreInput.root...]"
  exit 1
fi

#Get arguments from the command line
configFile=$1
shift

#Make list of .root files to process on the grid, one per job
for fileName in $@
do
  echo $fileName >> fileList.txt
done

#TODO: Convert file names to xrootd paths

#Figure out where code comes from and where to put temporary files
outFileName="testSimulation.root"
gridScriptName="basicSimulation.sh"
codeDir="/exp/emph/app/users/aolivier/batchSubmissionDevelopment" #TODO: Get this from CETPKG_SOURCE once I have this working from SL7 container

#Prepare files needed for grid submission
source $codeDir/emphaticsoft/GridSubmission/gridSubFunctions.sh
makeOutputDirectory
makeTarball $codeDir
makeWrapperBoilerplate $codeDir > ${gridScriptName} #Overwrite grid script if it already exists from a previous job submission
jobsubArgs=$(getBasicJobsubArgs)

#Add reconstruction-specific lines to wrapper
echo "INPUT_FILE=\$(head -n \$(PROCESS+1) fileList.txt | tail -n -1) || exit 2" >> ${gridScriptName}
echo "echo \"***** finished finding input file *****\"" >> ${gridScriptName}
echo "art -c $(basename ${configFile}) -o ${outFileName} ${INPUT_FILE} || exit 3" >> ${gridScriptName}
echo "echo \"***** finished ART job *****\"" >> ${gridScriptName}

jobsub_submit -N ${nJobs} -f dropbox://$(pwd)/${configFile} -f dropbox://$(pwd)/fileList.txt $jobsubArgs file://$(pwd)/${gridScriptName}
#echo "jobsub_submit -N ${nJobs} -f dropbox://${generatorScript} -f dropbox://${templateConfig} $jobsubArgs file://$(pwd)/${gridScriptName}"
