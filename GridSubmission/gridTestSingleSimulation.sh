outDirTag="ROOT_OUTPUT"
outFileName="testSimulation.root"
gridScriptName="basicSimulation.sh"
hostOutDir="/pnfs/emphatic/persistent/users/${USER}/testSimulation"
safeScratchDir="/exp/emph/app/users/${USER}" #TODO: Does EMPHATIC need /pnfs/emphatic/resilient?
tarFileName="myEmphaticsoft.tar.gz"
codeDir="/exp/emph/app/users/aolivier/batchSubmissionDevelopment" #TODO
#nJobs=${1} #TODO: Submit multiple jobs at once.  Give them distinct PRNG seeds through ART.

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
#TODO: Put all stdout and sterr in a log file that gets copied back from the grid.  Can I use the log file that jobsub creates for me without having to run jobsub_fetchlog somehow?
echo "#!/usr/bin/bash" > ${gridScriptName} #Overwrite grid script if it already exists
echo "source \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/emphaticsoft/setup/setup_emphatic.sh" >> ${gridScriptName}
echo "cd \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/opt/build" >> ${gridScriptName}
echo "echo \"***** finished setup_emphatic.sh *****\"" >> ${gridScriptName}
echo "source \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/emphaticsoft/setup/setup_for_grid.sh" >> ${gridScriptName}
echo "echo \"***** finished setup_for_grid.sh *****\"" >> ${gridScriptName}
echo "source setup_emphaticsoft" >> ${gridScriptName}
echo "cd \${CONDOR_DIR_${outDirTag}}" >> ${gridScriptName}
echo "art -c g4gen_job.fcl -n 10 -o ${outFileName}" >> ${gridScriptName}
echo "exit" >> ${gridScriptName}

#TODO: I could submit from a regular emphatic working environment (e.g. SL7) by starting an AL9 container for the jobsub job.  This might let me use environment vairables to figure out ${codeDir}.

jobsub_submit -d ${outDirTag} ${hostOutDir} -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --tar_file_name dropbox://${hostOutDir}/${tarFileName} --use-cvmfs-dropbox file://$(pwd)/${gridScriptName}
