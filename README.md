# emphaticsoft
Base repository for art-based code

#### Setting up
* [On emphaticgpvm01.fnal.gov over ssh](#gpvm) <br>
* [Locally in a Docker containter](#docker) <br>

[Running art](#run) <br>
[Generating documentation](#docs) <br>
[Developing EMPHATICSoft Code](#develop) <br>

---
<a name="gpvm"></a>
## Instructions for setting up the code for the first time on emphaticgpvm01.fnal.gov:

0.  Get a ticket for access EMPHATIC computer:
    * Go to Fermilab Service Desk (fermi.servicenowservices.com)    
    * Click on “Request Something”
    * Click on “Accounts and Access” in the menu on the right
    * Click on “Experiment/Project/Collaboration Computing Account”
    * Fill in the information.  Select “T-1396 EMPHATIC” in the Affiliation/Experiment” dropdown box
    * “Computing/IT Professional Access Only” option should be set to “No”.

1.  Log into emphaticgpvm01.fnal.gov via kerberized ssh.  If you have never used kerberos before, it is easy to install on most platforms (and usually most Linux- and unix-based OSs have this pre-installed).  Replace the standard /etc/krb5.conf file with an appropriate file obtained from  https://authentication.fnal.gov/krb5conf/.
```
kinit username@FNAL.GOV
klist
ssh -Y username@emphaticgpvm0X.fnal.gov
```
Where "X" should be replaced with "1", "2" or "3", to login to one of emmphatic's three virtual machines.

If you are using Linux you need to include another flag:
```
ssh -K -Y username@emphaticgpvm0X.fnal.gov
```
Note to Mac users: add the following lines to your $HOME/.ssh/config file:
```
Host *.fnal.gov
Protocol 2
GSSAPIAuthentication yes
GSSAPIDelegateCredentials yes
ForwardAgent yes
ForwardX11 yes
ForwardX11Trusted yes
ServerAliveInterval 60 #For some users these lines need to be commented out
```
Mac users may sometimes need additional environment variable:

``` 
export KRB5CCNAME=FILE:/tmp/krb5cc_`id -u` 
```
You should add that export command to your bash login so you don't have to do it or remember it every time. 

2.  After logging in, if it doesn't already exist, create the directory /exp/emph/app/users/[username]:

```
mkdir /exp/emph/app/users/[username]
```
3.  If you haven't already done so, clone the git repository for EMPHATICSOFT/emphaticsoft in this same directory (/exp/emph/app/users/[username]), or a directory of your choosing.

```
git clone https://github.com/EmphaticSoft/emphaticsoft
```
You will prompted to enter your github username and a password.  The password should be your personal access token, which you should obtain by following the instructions here: https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token.

3. Then execute:

```
source /exp/emph/app/users/[username]/<directory-you-chose>/emphaticsoft/setup/setup_emphatic.sh
sl7-emph
cd /exp/emph/app/users/[username]/<directory-you-chose>/
mkdir build
```

4. execute:

```
cd /exp/emph/app/users/[username]/<directory-you-chose>/build
source /exp/emph/app/users/[username]/<directory-you-chose>/emphaticsoft/ups/setup_for_development -p

Note: You must run this "source" command within the build directory
```

5. To compile the code (must be in the "build" directory!):

```
buildtool --generator=ninja
```

Note, once you've done this step, you can simply call "ninja" to compile changes.

---
<a name="docker"></a>
## Instructions for running a Docker container on your own machine to build the code:

Docker is a commercial software (free for now) that allows EMPHATIC to release the software we use, including all the dependencies and for users to run it on a variety of operating systems.  Docker images of EMPHATIC software are made available via a networked file system provided by CERN, namely CVMFS.  


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
1. Install [Docker](https://docs.docker.com/engine/install/).
   * Once Docker is installed, open it and go to preferences (the gear icon on the top right). Go to `Resources->File Sharing` and add `/cvmfs` to the list, then click `Apply & Restart`. (**Note:** Adding `/cvmfs` to File Sharing is only needed if we put the image on cvmfs rather than just pulling it from DockerHub.)
1. Clone the EMPHATICSoft/emphaticsoft repository onto your computer:
    ```
    git clone https://github.com/EmphaticSoft/emphaticsoft
    ```
1. Make a build directory on your computer (eg. `mkdir build`). Mounting this in the container will speed up the build process (after the first build).
### To run the container:
1. Anytime your computer reboots, you will need to remount cvmfs:
   ```
   sudo mount -t cvmfs emphatic.opensciencegrid.org /cvmfs/emphatic.opensciencegrid.org/
   sudo mount -t cvmfs fermilab.opensciencegrid.org /cvmfs/fermilab.opensciencegrid.org/
   ```
1. Start Docker. (Double-click on the application or however you usually open applications in your OS.)   
1. Start the container with:
   ```
   docker run --rm -it -v <path-to-your-local-repo>:/emphaticsoft -v /cvmfs:/cvmfs:cached -v <path-to-your-build-directory>:/build lackey32/emphaticsoft-sl7
   ```
1. To set up the environment and ups products, do:
   ```
   source emphaticsoft/setup/setup_emphatic.sh
   cd build
   source ../emphaticsoft/ups/setup_for_development -p
   ```
1. To compile the code:
   * for the first time (must be in the build directory):
   ```
   buildtool --generator=ninja
   ```
   * any subsequent builds (still have to be in the build directory):
   ```
   ninja
   ```
<a name="run"></a>
## Running run on emphaticsoft art modules. 

   * Now that you have build shared objects that could run under art, the real work can start, i.e., running art and your EMPHATIC shared objects to get some histograms that can be seen using root, or simple ASCII .csv files, that can be studied with any analysis tools you like.  These intructions are valid if  you are on emphaticgpvm0X.fnal.gov or your own machine.  First, go to the build directory, and if you have not done so, execute the two setups mentioned above. 
 
   For instance, for the username lebrun, one has:    
   ```
   -bash-4.2$ cd /exp/emph/app/users/lebrun/build
   -bash-4.2$ source ../emphaticsoft/setup/setup_emphatic.sh 
       Setting up emphatic UPS area... /cvmfs/emphatic.opensciencegrid.org/products/
     -bash-4.2$ source ../emphaticsoft/ups/setup_for_development -p

     The working build directory is /exp/emph/app/users/lebrun/build
     The source code directory is /exp/emph/app/users/lebrun/emphaticsoft

     ---------------------- check this block for errors ---------------------
     ------------------------------------------------------------------------

     To inspect build variable settings, execute /exp/emph/app/users/lebrun/build/cetpkg_info.sh

    Please use "buildtool" to configure and build emphaticsoft, e.g.:

      buildtool -I <ups-topdir> -vTl [-jN]

    See "buildtool --usage" (short usage help) or "buildtool -h|--help"
    (full help) for more details.
   ```
    * The FHICL_FILE_PATH is an environmental variable which holds multipath, the first one being the one 
      /exp/emph/app/users/lebrun/build/fcl, where our fcl files do reside.
       Then, art will find the top level (the one in the Unix command you invoke), and all the subsequent ones. 
       Such that the command: 
   ```
     -bash-4.2$ art -c onmonprod_job.fcl /exp/emph/app/users/lebrun/data/emphdata_r466_s1.root
   ```
     
     will work.. 

<a name="docs"></a>
## Generating documentation
Refer to [docs](https://github.com/EMPHATICSoft/emphaticsoft/tree/main/docs)

<a name="develop"></a>
## How to Develop EMPHATIC Code

In order to make changes to EMPHATIC software, you will need to create your own git Branch.  After you have pulled the emphaticsoft repository, you can create a new branch and switch to it by executing:

```
git branch <your_branch_name>
git checkout <your_branch_name>
```

where you should change <your_branch_name> to a name of your choosing.  You can then edit and commit changes to that branch as much as you want without affecting other EMPHATIC developers.  Dominic wrote some instructions for making changes to a git branch in DocDB (https://emphatic-docdb.fnal.gov/cgi-bin/sso/ShowDocument?docid=1428).  

When you create a branch, it will be a static copy of emphaticsoft at the time you created a branch.  But in general, it is good practice to merge in changes that others make to the main branch so your branch does not fall far behind the main branch.  You can do that by e.g.:

```
git checkout main
git pull
git checkout <your_branch_name>
git merge main
git push origin <your_branch_name>
```

Once your changes are thoroughly tested and your are ready for them to go into the main branch, you should execute a pull request.  To do this, first make sure your branch is up to date with changes from the main branch (see above), then go to the https://github.com/EMPHATICSoft/emphaticsoft/branches, find your branch, click the "new pull request" button next to your branch, and follow the resulting instructions to create a new pull request.  You should then ask another EMPHATICsoft developer to review your changes.  Contact Laura Fields, Jon Paley, and Gavin Davies if you aren't sure who should review it.  Once that person has signed off on your code, you should then merge the pull request and delete your branch.  Instructions here:  https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/incorporating-changes-from-a-pull-request/merging-a-pull-request. 


## Working with Git 

**This section explains how to:**

1. Make a branch in your home directory
2. Clone main into it
3. Check differences between the two branches 
4. Push changes into your local branch 
5. Add files from your branch to the main branch 
6.  Push changes into the main branch
7. Merge your branch with the main branch  


* Go to your user directory (ex: `/exp/emph/app/users/dhuerta/`)  
* Make a new directory (ex: `git_practice`)
* go into the new directory  
* Run: `git clone https://github.com/EmphaticSoft/emphaticsoft`   
	 * This clones the main branch into the directory we just made
* Go into the `emphaticsoft` directory  
* Run: `git checkout -b <new_branch_name>`  
	* This makes a new branch and switches into it
   * You can run: `git branch` to see what branch we are in (* on side) and the other branches in that directory listed below  
	* Run: `git checkout <branch_name>`  to change between branches
* Make necessary changes in **your branch**  
* Run `git add <1st_file_name> <2nd_file_name> ....` to add as many new files to the main branch as you want 
   * Files in green are the ones we are going to commit
	* Files in red where not added, therefore are being ignored for now
* Push your new branch, by runnning: `git push -u origin <branch_name>`  
	* Every time after that just use `git push` 
	* You can run: `git status` to see the differences between the branch you are in and main at any point   
* Go back to your  directory 
* Make a new build directory using: `mkdir build`
* Run : `source /exp/emph/app/users/<your_username>/git_practice/emphaticsoft/setup/setup_emphatic.sh`
* Go into the build directory (`cd /exp/emph/app/users/<your_username>/git_practice/build` ) 
* Run: `source /exp/emph/app/users/<your_username>/git_practice/emphaticsoft/ups/setup_for_development -p` 
* Compile (use `buildtool --generator=ninja`  the first time and just `ninja`  any time after that)  
	* Might get build error, run ninja and if you still have errors, fix now
* If all of your changes are made and you are ready to commit them into your branch, go to: `/exp/emph/app/users/<your_username>/git_practice/emphaticsoft`  
* Run: `git push`  
	- Use your GitHub username and your token as your password 
* Go to GitHub **online** 
* Click on view pull request 
* Request a reviewer and have them accept your changes 
* Once they have approved it the changes should be made to your branch, **not main**

**To merge your branch into main:**
* Run `git checkout main` to switch into the main branch 
* Run: `git merge <branch_name>` 
* GitHub may ask you to correct any overlaps or issues at this time 
* Once all problems have been resolved run: `git add <file_to_merge>` 
* Run `git push` to make changes to the main branch
*

**To merge main into your branch:**
* Run `git checkout main` to switch into the main branch
* Run `git pull`
* Run `git checkout <branch_name>`
* Run `git merge main`
* GitHub may ask you to correct any overlaps or issues at this time 
* Once all problems have been resolved run: `git add <file_to_merge>` 
* Run `git push` to make changes to your branch
* 

## Copyright and Licensing

Copyright © 2023 FERMI NATIONAL ACCELERATOR LABORATORY for the benefit of the EMPHATIC Collaboration.

This repository, and all software contained within, except where noted within the individual source files, is licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Copyright is granted to FERMI NATIONAL ACCELERATOR LABORATORY on behalf of the  Experiment to Measure the Production of Hadrons At a Test beam In Chicagoland (EMPHATIC) experiment. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
