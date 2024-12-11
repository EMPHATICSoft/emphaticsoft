#!/bin/bash
#submitGenerator.sh: Submit an ART job that takes no input to the grid

#Help text
if [[ $1 == "-h" || $1 == "--help" || $# < 2 ]]; then
  echo "Usage: submitGenerator.sh template.fcl generator.sh nJobs"
  echo "- generator.sh takes template.fcl as an input, updates things like"
  echo "run number and random seed based on the grid environment, and"
  echo "prints the result to stdout."
  echo "- nJobs is an integer number of jobs to run in parallel."
  exit 1
fi

#Get arguments from the command line
templateConfig=$1
generatorScript=$2
nJobs=$3

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

#Add generator-specific lines to wrapper
echo "bash \${CONDOR_DIR_INPUT}/$(basename ${generatorScript}) \${CONDOR_DIR_INPUT}/$(basename ${templateConfig}) > config_\${PROCESS}.fcl || exit 2" >> ${gridScriptName}
echo "echo \"***** finished generating template config file *****\"" >> ${gridScriptName}
echo "art -c config_\${PROCESS}.fcl -o ${outFileName} || exit 3" >> ${gridScriptName}
echo "echo \"***** finished ART job *****\"" >> ${gridScriptName}

jobsub_submit -N ${nJobs} -f dropbox://$(pwd)/${generatorScript} -f dropbox://$(pwd)/${templateConfig} $jobsubArgs file://$(pwd)/${gridScriptName}
#echo "jobsub_submit -N ${nJobs} -f dropbox://${generatorScript} -f dropbox://${templateConfig} $jobsubArgs file://$(pwd)/${gridScriptName}"
