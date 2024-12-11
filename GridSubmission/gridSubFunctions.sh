#!/bin/bash
#Functions that help generate files and check for common mistakes when submitting grid jobs

#Environment variables used by multiple functions
outDirTag="ROOT_OUTPUT"
tarFileName="myEmphaticsoft.tar.gz"
safeScratchDir="/exp/emph/app/users/${USER}"

#Make output directory.  It must not exist yet because overwriting files on /pnfs can take down /pnfs!
makeOutputDirectory()
{
  hostOutDir=$1

  if [[ -e ${hostOutDir} ]]; then
    echo "${hostOutDir} already exists.  You must delete it before you can send grid output there." >&2
    exit 1
  fi
  
  mkdir ${hostOutDir}
}

#Copy user's software development area to the grid
#Seems like I don't need to use resilient according to the jobsub documentation at https://cdcvs.fnal.gov/redmine/projects/jobsub/wiki/Jobsub_submit
makeTarball()
{
  codeDir=$1
  hostOutDir=$2

  #Try to update tarball if it already exists
  #TODO: Get tar --update working
  if [[ -f ${safeScratchDir}/${tarFileName} ]]
  then
  #  echo "Found existing tarball at ${safeScratchDir}/${tarFileName}.  Trying to update it for newer files!"
  #  #tarball must be unzipped to -u[pdate] it
  #  unpigz ${safeScratchDir}/${tarFileName}
  #  tar --update ${safeScratchDir}/$(basename ${safeScratchDir}/${tarFileName}).tar
  #  pigz ${safeScratchDir}/$(basename ${safeScratchDir}/${tarFileName}).tar
    rm ${safeScratchDir}/${tarFileName}
  fi
  #else
    echo "No tarball at ${safeScratchDir}/${tarFileName}, so creating a new one from ${codeDir}.  This usually takes a few minutes..."
    #Make new tarball from user's active development area
    tar -czf ${safeScratchDir}/${tarFileName} -C $(dirname ${codeDir}) $(basename ${codeDir})
  #fi
  #Put the tarball somewhere the grid can find it and document what code was used at the same time
  cp ${safeScratchDir}/${tarFileName} ${hostOutDir}/

  if [[ ! -f ${hostOutDir}/${tarFileName} ]]
  then
    echo "Tarring up ${codeDir} into ${safeScratchDir}/${tarFileName} and/or copy to /pnfs failed!" >&2
    exit 2
  fi
}

#Write the beginning of the bash script that runs on a grid node.
#Gets code from the tarball and sets up user's emphaticsoft area.
#PROCESS and CONDOR_DIR_* are provided by Fermilab's jobsub commands.
makeWrapperBoilerplate()
{
  codeDir=$1

  echo "#!/usr/bin/bash"
  echo "source \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/emphaticsoft/setup/setup_emphatic.sh || exit 1"
  echo "cd \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/opt/build"
  echo "echo \"***** finished setup_emphatic.sh *****\""
  echo "source \${INPUT_TAR_DIR_LOCAL}/$(basename ${codeDir})/emphaticsoft/setup/setup_for_grid.sh || exit 1"
  echo "echo \"***** finished setup_for_grid.sh *****\""
  echo "source setup_emphaticsoft || exit 1"
  echo "cd \${CONDOR_DIR_${outDirTag}}"
  echo "mkdir job_\${PROCESS}"
  echo "cd job_\${PROCESS}"
}

#Arguments to jobsub_submit for getting emphatic user code to the grid and running in SL7
getBasicJobsubArgs()
{
  hostOutDir=$1

  echo "-d ${outDirTag} ${hostOutDir} -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --tar_file_name dropbox://${hostOutDir}/${tarFileName} --use-cvmfs-dropbox"
}
