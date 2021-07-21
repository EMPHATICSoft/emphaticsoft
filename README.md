# emphaticsoft
Base repository for art-based code

Instructions for developing/building code on emphaticgpvm01.fnal.gov:

1.  After logging in, execute:

> source /emph/app/setup/setup_emphatic.sh

2.  You should develop code in the /emph/app/users/[username] directory.

3.  If you haven't already done so, clone the git repository for EMPHATICSOFT/emphaticsoft.

4.  If you haven't already done so, create a build area outside of emphaticsoft, then "cd" to that directory, eg, "cd [build_area]".

5. execute:

> source /emph/app/users/[username]/emphaticsoft/ups/setup_for_development -p

6. To compile the code:

> buildtool --generator=ninja

Note, once you've done this step, you can simply call "ninja" to compile changes.