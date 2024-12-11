#Usage: submitToGrid config.fcl input.root [more.root...]
#       submitToGrid template.fcl generatorScript.sh nJobs

outDirTag="ROOT_OUTPUT"
outFileName="testSimulation.root"
gridScriptName="basicSimulation.sh"
hostOutDir="/pnfs/emphatic/persistent/users/${USER}/testSimulation"
safeScratchDir="/exp/emph/app/users/${USER}"
tarFileName="myEmphaticsoft.tar.gz"
codeDir="/exp/emph/app/users/aolivier/batchSubmissionDevelopment" #TODO

#Figure out whether this is a reconstruction/analysis or a simulation job based on the command line
fclFile=$1
shift

nJobs=$#
firstFile=""
if [[ $1 =~ *.root ]]; then
  for fileName in $@; do
    echo ${fileName} >> inputFiles.txt
  done
else
  generatorScript=$2
  nJobs=$3
fi
#It would be useful to check that .fcl files are running a geenrator module and check that all files after the second file have the same extension one day
echo "nJobs = $nJobs"
#TODO: If looping over many .fcl files, should I just make a separate tarball for them?

#Ship list of input files to the grid.  Put them in a file so I can use head + tail.  Maybe just set nJobs to number of lines in that file?
for fileName in $@; do
  echo ${fileName} >> inputFiles.txt
done
#I could transform any /pnfs paths to xrootd at this point.

#Make output directory.  It must not exist yet because overwriting files on /pnfs can take down /pnfs!
if [[ -e ${hostOutDir} ]]; then
  echo "${hostOutDir} already exists.  You must delete it before you can send grid output there." >&2
  exit 1
fi

mkdir ${hostOutDir}

#Copy user's software development area to the grid
#Seems like I don't need to use resilient according to the jobsub documentation at https://cdcvs.fnal.gov/redmine/projects/jobsub/wiki/Jobsub_submit
if tar -czf ${safeScratchDir}/${tarFileName} -C $(dirname ${codeDir}) $(basename ${codeDir}) && cp ${safeScratchDir}/${tarFileName} ${hostOutDir}/
then
  rm ${safeScratchDir}/${tarFileName}
else
  echo "Tarring up ${codeDir} into ${safeScratchDir}/${tarFileName} and/or copy to /pnfs failed!" >&2
  exit 2
fi

#Write the "wrapper" script that runs on the grid
echo "#!/usr/bin/bash" > ${gridScriptName} #Overwrite grid script if it already exists
echo "source \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/emphaticsoft/setup/setup_emphatic.sh" >> ${gridScriptName}
echo "cd \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/opt/build" >> ${gridScriptName}
echo "echo \"***** finished setup_emphatic.sh *****\"" >> ${gridScriptName}
echo "source \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/emphaticsoft/setup/setup_for_grid.sh" >> ${gridScriptName}
echo "echo \"***** finished setup_for_grid.sh *****\"" >> ${gridScriptName}
echo "source setup_emphaticsoft" >> ${gridScriptName}
echo "cd \${CONDOR_DIR_${outDirTag}}" >> ${gridScriptName}
echo "mkdir job_\${PROCESS}" >> ${gridScriptName}
echo "cd job_\${PROCESS}" >> ${gridScriptName}
echo "inputFile=\$(head -n \$(PROCESS+1) inputFiles.txt | tail -n -1)" >> ${gridScriptName}
echo "art -c ${firstFile} \${inputFile} -o ${outFileName}" >> ${gridScriptName}
echo "exit" >> ${gridScriptName}

#TODO: I could submit from a regular emphatic working environment (e.g. SL7) by starting an AL9 container for the jobsub job.  This might let me use environment variables to figure out ${codeDir}.

jobsub_submit -N ${nJobs} -f inputFiles.txt -f ${firstFile} -d ${outDirTag} ${hostOutDir} -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --tar_file_name dropbox://${hostOutDir}/${tarFileName} --use-cvmfs-dropbox file://$(pwd)/${gridScriptName}
