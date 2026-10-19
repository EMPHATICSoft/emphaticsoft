# emphaticsoft
Base repository for art-based code

This is the `spack-dev` branch, which builds with Spack on top of the EMPHATIC
Spack release on CVMFS. For the UPS + mrb build, see the README on
[`main`](https://github.com/EMPHATICSoft/emphaticsoft/tree/main).

#### Setting up
* [Access to the EMPHATIC GPVMs](#gpvm) <br>
* [Building with Spack](#spack) <br>
* [Without an AlmaLinux 9 GPVM: the container](#container) <br>

[Running art](#run) <br>
[Generating documentation](#docs) <br>
[Developing EMPHATICSoft Code](#develop) <br>

---

<a name="gpvm"></a>
## Access to the EMPHATIC GPVMs

0.  Get a ticket for access EMPHATIC computer:
    * Go to Fermilab Service Desk (fermi.servicenowservices.com)    
    * Click on “Request Something”
    * Click on “Accounts and Access” in the menu on the right
    * Click on “Update my Affiliation/Experiment/Collaboration membership Request”
    * Fill in the information.  Select “EMPHATIC” in the Affiliation/Experiment” dropdown box

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

<a name="spack"></a>
## Building with Spack

Full instructions, with checks and troubleshooting, are on the wiki:
[Spack Developer Build](https://github.com/EMPHATICSoft/emphaticsoft/wiki/Spack-Developer-Build).

Only emphaticsoft is compiled, in about 4 minutes. art, ROOT, Geant4, Millepede,
PyTorch and the other dependencies are already installed on CVMFS:
`/cvmfs/emphatic.opensciencegrid.org/spack/v1.1.1`, environment `emphaticsoft-env`
(the current release, at present `emphaticsoft-v08_00_00`), with PyTorch and CUDA from LArSoft's
`/cvmfs/larsoft.opensciencegrid.org/spack-fnal-v1.1.1`. The build has the full
offline suite (`~daq +beamdb +align +ml`): alignment with Millepede, the ARICH
TorchML reconstruction, the raw-data unpackers, `RawDataMerger` and
`WebDisplay`. Online monitoring is not built.

Use an AlmaLinux 9 GPVM and a work area under `/exp/emph/app/users/$USER`.
First check that both CVMFS repositories are there. If the LArSoft one is
missing, Spack builds PyTorch and CUDA from source without saying why:

```bash
ls /cvmfs/emphatic.opensciencegrid.org/spack/v1.1.1/setup-env.sh
ls /cvmfs/larsoft.opensciencegrid.org/spack-fnal-v1.1.1/setup-env.sh
```

### Run the release

Nothing to build:

```bash
source /cvmfs/emphatic.opensciencegrid.org/spack/v1.1.1/setup-env.sh
spack env activate emphaticsoft-env
spack load emphaticsoft
```

### Develop: one-time setup

One command makes your own Spack instance on top of the release, copies the
release environment into it as `local_emphaticsoft-env` (with its lock, so every
installed package is reused), and checks out emphaticsoft `spack-dev`:

```bash
export DEV=/exp/emph/app/users/$USER/spack-dev
mkdir -p $DEV
source /cvmfs/emphatic.opensciencegrid.org/spack/v1.1.1/setup-env.sh
spack subspack --with-padding --local-env emphaticsoft-env \
  --dev-pkg emphaticsoft@spack-dev $DEV/spack
ln -s $DEV/spack/var/spack/environments/local_emphaticsoft-env/emphaticsoft $DEV/emphaticsoft
git -C $DEV/emphaticsoft remote set-url origin git@github.com:EMPHATICSoft/emphaticsoft.git

source $DEV/spack/setup-env.sh
spack env activate local_emphaticsoft-env
spack install -j 8
```

Only `emphaticsoft` should be listed as `Installing`. If Spack starts building
art, ROOT or Geant4, stop it and ask in `#emphatic_software`.

### Develop: every new shell

```bash
export DEV=/exp/emph/app/users/$USER/spack-dev
source $DEV/spack/setup-env.sh
spack env activate local_emphaticsoft-env
spack load emphaticsoft
```

### Rebuilding after a change

Edit the code in `$DEV/emphaticsoft`, then run `spack install` again. It
rebuilds only what changed (about 35 s after editing one `.cxx` file), and the
new build is used in the same shell. Unit tests:

```bash
spack build-env emphaticsoft -- ctest --test-dir $(spack location --build-dir emphaticsoft)
```

<a name="container"></a>
## Without an AlmaLinux 9 GPVM: the container

On an SL7 machine or a laptop with CVMFS, use the AlmaLinux 9 container described
on the wiki page
[Spack Container Image](https://github.com/EMPHATICSoft/emphaticsoft/wiki/Spack-Container-Image)
and run the same commands inside it.

<a name="run"></a>
## Running art

After `spack load emphaticsoft`, `art` and all EMPHATIC modules and fcl files
are on your paths, and `$CETPKG_SOURCE` points at the installed runtime data
(geometry, channel maps, calibrations). A short MC chain, in a scratch
directory:

```bash
art -c g4gen_job.fcl -n 20 -o sim.root
art -c digitization_job.fcl -s sim.root
art -c prod_reco_caf_job_mc.fcl -s sim.dig.root
```

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

On this branch, open pull requests into `spack-dev`, not `main`.


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
