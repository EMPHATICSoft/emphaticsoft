#!/usr/bin/bash

USAGE="submitToGrid.sh configFile.yaml /path/to/playlist [/path/to/morePlaylists]..."

#Submit NucCCNeutrons event loops to run on the FermiGrid in parallel.
#Assumes all code is in ${PLOTUTILSROOT}/..
#Copies output files back to current working directory, so run it from /pnfs!
#Exit codes:
#0: success!
#1: Command line arguments failed validation
#2: Environment is not set up correctly
#3: Could not create tarball
#4: TODO: jobsub_submit failure

yamlFile=${1}
shift
playlistDirs=($@)
#TODO: options for --memory, --runtime, and --tarfile
#      This seems like a great starting point: http://mywiki.wooledge.org/BashFAQ/035
lifetime="3h"
memory="2000MB"

#Validate input parameters from the command line
if [ $# -lt 2 ]
then
  echo "Wrong number of command line arguments." >&2
  echo ${USAGE} >&2
  exit 1
fi

#TODO: Check that jobsub is set up.  If not, set it up from central Fermilab area on cvmfs.  Same thing for voms-proxy-init?

#Some derived parameters that will make my code more readable
timestamp=$(date +%m_%d_%y_%H_%M_%S) #$(date -Isec)
outdir=$(pwd)
codeDir=$(dirname ${PLOTUTILSROOT} | xargs dirname)
tarFileName="$(basename ${codeDir})_${timestamp}.tar.gz"
safeScratchDir="/minerva/app/users/$USER"
wrapperFileName="runNucCCNeutronsOnGrid_${timestamp}.sh"
gridNodeOutDir="HISTS"

if [ ${#playlistDirs[@]} -lt 1 ]
then
  echo "No target directories given on the command line." >&2
  echo ${USAGE} >&2
  exit 1
fi

if [ ! -f ${yamlFile} ]
then
  echo "YAML configuration file, ${yamlFile}, is not in the current directory, ${outdir}.  Read ProcessAnaTuples --help for more information on format." >&2
  echo ${USAGE} >&2
  exit 1
fi

#I could even run a small test job here to make sure YAML file is valid

#Sanity checks on output directories.  Protect dCache!
if [[ ! ${outdir} =~ "/pnfs/minerva/persistent/users/${USER}" ]]
then
  echo "Output directory, ${outdir}, is not on /pnfs.  It must be visible to grid nodes.  Try running again from /pnfs/minerva/persistent/users/${USER} this time." >&2
  exit 2
fi

for targetDir in ${playlistDirs[@]}
do
  targetOutDir=$(basename targetDir)
  if [ -d ${outdir}/${targetOutDir} ]
  then
    echo "${outdir}/${targetOutDir} already exists!  dCache cannot overwrite files.  Trying may crash file storage for the whole lab!  Move or delete this directory and try again." >&2
    exit 2
  fi

  if [ $(ls ${targetDir} | grep ".root" | wc -l) -lt 1 ]
  then
    echo "${targetDir} exists but has no .root files to process." >&2
    exit 2
  fi
done

#Make tarball
if [[ ! -n ${PLOTUTILSROOT} ]]
then
  echo "I can't find the code directory to tar up because PLOTUTILSROOT is not set!  Set up NucCCNeutrons and/or MAT-MINERvA and try again." >&2
  exit 2
fi

#Copy to resilient instead of outdir?
#Seems like I don't need to use resilient according to the jobsub documentation at https://cdcvs.fnal.gov/redmine/projects/jobsub/wiki/Jobsub_submit
if tar -czf ${safeScratchDir}/${tarFileName} -C ${codeDir} opt && cp ${safeScratchDir}/${tarFileName} ${outdir}/
then
  rm ${safeScratchDir}/${tarFileName}
else
  echo "Tarring up ${codeDir} into ${safeScratchDir}/${tarFileName} and/or copy to /pnfs failed!" >&2
  exit 3
fi

#Write directories to process into a file that I can read from on the grid.
#To use xrootd to stream files from /pnfs, change up playlistDir a little.
for playlistDir in ${playlistDirs[@]}
do
  echo "${playlistDir}" >> ${safeScratchDir}/playlists.txt
done

sed -i 's/^\/pnfs\/minerva\/\(.*\)/root:\/\/fndca1.fnal.gov:1094\/\/\/pnfs\/fnal.gov\/usr\/minerva\/\1/g' ${safeScratchDir}/playlists.txt

mv ${safeScratchDir}/playlists.txt ${outdir}/

#Write wrapper file
#Wouldn't it be better to provide at least a template file with my package?  Then, the wrapper would be more visible in version control.
tmpWrapperFile="${safeScratchDir}/${wrapperFileName}"
echo "#!/usr/bin/bash" > ${tmpWrapperFile}
echo "source /cvmfs/minerva.opensciencegrid.org/minerva/hep_hpc_products/setups" >> ${tmpWrapperFile}
echo "setup cmake v3_7_1" >> ${tmpWrapperFile}
echo "setup root v6_10_04d -q e14:prof" >> ${tmpWrapperFile}
echo "ls -lathr" >> ${tmpWrapperFile} #Debugging
echo "ls -lathr \$INPUT_TAR_DIR_LOCAL" >> ${tmpWrapperFile} #Debugging
echo "export MINERVA_PREFIX=\${INPUT_TAR_DIR_LOCAL}/opt" >> ${tmpWrapperFile}
echo "echo \${MINERVA_PREFIX}" >> ${tmpWrapperFile}
echo "source \${INPUT_TAR_DIR_LOCAL}/opt/bin/setup.sh" >> ${tmpWrapperFile}
echo "echo \${CONDOR_DIR_INPUT}" >> ${tmpWrapperFile} #Debugging
#echo "cd \${CONDOR_DIR_INPUT}" >> ${tmpWrapperFile}
#echo "ls -lathr" >> ${tmpWrapperFile} #Debugging
#echo "tar -xf $(basename ${tarFileName} .tar.gz)" >> ${tmpWrapperFile} #--tar_file_name seems to untar the file for me
echo "echo \${PLOTUTILSROOT}" >> ${tmpWrapperFile} #Debugging
echo "echo \${PATH}" >> ${tmpWrapperFile} #Debugging
echo "echo \${LD_LIBRARY_PATH}" >> ${tmpWrapperFile} #Debugging
echo "" >> ${tmpWrapperFile}
echo "PLAYLIST=\$(head -n \$((PROCESS + 1)) \${CONDOR_DIR_INPUT}/playlists.txt | tail -n -1)" >> ${tmpWrapperFile} #N.B.: PROCESS starts at 0, but head -n 0 won't display anything!
#echo "TUPLE_PREFIX=\$(basename \${PLAYLIST})" >> ${tmpWrapperFile}
echo "TUPLE_PREFIX=\$(echo \${PLAYLIST} | sed 's|.*/\(me[0-9][A-Z]\)/.*|\1|g')" >> ${tmpWrapperFile}
echo "cd \${CONDOR_DIR_${gridNodeOutDir}}" >> ${tmpWrapperFile}
echo "mv \${CONDOR_DIR_INPUT}/${yamlFile} ." >> ${tmpWrapperFile}
echo "ls -lathr" >> ${tmpWrapperFile} #Debugging
echo "pwd" >> ${tmpWrapperFile} #Debugging
echo "/usr/bin/time -v ProcessAnaTuples ${yamlFile} \${PLAYLIST}/*.root" >> ${tmpWrapperFile}
echo "for fileName in *.*" >> ${tmpWrapperFile}
echo "do" >> ${tmpWrapperFile}
echo "  mv \${fileName} \${TUPLE_PREFIX}_\${fileName}" >> ${tmpWrapperFile}
echo "done" >> ${tmpWrapperFile}

mv ${tmpWrapperFile} ${outdir}/${wrapperFileName}

#The main event: grid submission
jobsub_submit --group minerva --OS sl7 --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC --role=Analysis --expected-lifetime ${lifetime} --memory ${memory} --tar_file_name dropbox://${outdir}/${tarFileName} --use-cvmfs-dropbox -f ${outdir}/${yamlFile} -f ${outdir}/playlists.txt -d ${gridNodeOutDir} ${outdir} -N ${#playlistDirs[@]} file://${outdir}/${wrapperFileName}
