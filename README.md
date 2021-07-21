# emphaticsoft
Base repository for art-based code

**** Instructions for setting up the code for the first time on emphaticgpvm01.fnal.gov:

1.  Log into emphaticgpvm01.fnal.gov via kerberized ssh.  If you have never used kerberos before, it is easy to install on most platforms (and usually most Linux- and unix-based OSs have this pre-installed).  Replace the standard /etc/krb5.conf file with an appropriate file obtained from  https://authentication.fnal.gov/krb5conf/.

2.  After logging in, execute:

> source /emph/app/setup/setup_emphatic.sh
> cd /emph/app/users/[username]
> mkdir build

3.  If you haven't already done so, clone the git repository for EMPHATICSOFT/emphaticsoft in this same directory (/emph/app/users/[username])

> git clone https://github.com/EmphaticSoft/emphaticsoft

4. execute:

> cd /emph/app/users/[username]/build
> source /emph/app/users/[username]/emphaticsoft/ups/setup_for_development -p

6. To compile the code (must be in the "build" directory!):

> buildtool --generator=ninja

Note, once you've done this step, you can simply call "ninja" to compile changes.
