# emphaticsoft
Base repository for art-based code

**** Instructions for setting up the code for the first time on emphaticgpvm01.fnal.gov:

1.  Log into emphaticgpvm01.fnal.gov via kerberized ssh.  If you have never used kerberos before, it is easy to install on most platforms (and usually most Linux- and unix-based OSs have this pre-installed).  Replace the standard /etc/krb5.conf file with an appropriate file obtained from  https://authentication.fnal.gov/krb5conf/.

2.  After logging in, if it doesn't already exist, create the directory /emph/app/users/[username]:

```
mkdir /emph/app/users/[username]
```

3. Then execute:

```
source /emph/app/setup/setup_emphatic.sh
cd /emph/app/users/[username]
mkdir build
```

3.  If you haven't already done so, clone the git repository for EMPHATICSOFT/emphaticsoft in this same directory (/emph/app/users/[username])

```
git clone https://github.com/EmphaticSoft/emphaticsoft
```

4. execute:

```
cd /emph/app/users/[username]/build
source /emph/app/users/[username]/emphaticsoft/ups/setup_for_development -p
```

6. To compile the code (must be in the "build" directory!):

```
buildtool --generator=ninja
```

Note, once you've done this step, you can simply call "ninja" to compile changes.

---
## Instructions for running a Docker container on your own machine to build the code:
### Initial setup (only need to do these steps once):
0. (Mac only) Install [macFuse](https://osxfuse.github.io/).
1. Install [CVMFS](https://cernvm.cern.ch/fs/). The link has instructions for various operating systems.
   * Once CVMFS is installed, create `/etc/cvmfs/default.local` and add the following lines (you will likely need to edit this as root or use `sudo`):
      ```
      CVMFS_REPOSITORIES=emphatic.opensciencegrid.org
      CVMFS_HTTP_PROXY=DIRECT
      ```
   * You also need to create the areas where you will be mounting cvmfs:
      ```
      sudo mkdir -p /cvmfs/emphatic.opensciencegrid.org
      sudo mkdir -p /cvmfs/fermilab.opensciencegrid.org
      ```
2. Install [Docker](https://docs.docker.com/engine/install/).
   * Once Docker is installed, open it and go to preferences (the gear icon on the top right). Go to `Resources->File Sharing` and add `/cvmfs` to the list, then click `Apply & Restart`. (**Note:** Adding `/cvmfs` to File Sharing is only needed if we put the image on cvmfs rather than just pulling it from DockerHub.)
3. Clone the EMPHATICSoft/emphaticsoft repository onto your computer:
    ```
    git clone https://github.com/EmphaticSoft/emphaticsoft
    ```
4. Make a build directory on your computer (eg. `mkdir build`). Mounting this in the container will speed up the build process (after the first build).
### To run the container:
1. Anytime your computer reboots, you will need to remount cvmfs:
   ```
   sudo mount -t cvmfs emphatic.opensciencegrid.org /cvmfs/emphatic.opensciencegrid.org/
   sudo mount -t cvmfs fermilab.opensciencegrid.org /cvmfs/fermilab.opensciencegrid.org/
   ```
2. Start the container with:
   ```
   docker run --rm -it -v <path-to-your-local-repo>:/emphaticsoft -v /cvmfs:/cvmfs:cached -v <path-to-your-build-directory>:/build lackey32/emphaticsoft-sl7
   ```
3. To set up the environment and ups products, do:
   ```
   source emphaticsoft/setup/setup_emphatic.sh
   cd build
   source ../emphaticsoft/ups/setup_for_development -p
   ```
4. To compile the code:
   * for the first time (must be in the build directory):
   ```
   buildtool --generator=ninja
   ```
   * any subsequent builds (still have to be in the build directory):
   ```
   ninja
   ```
