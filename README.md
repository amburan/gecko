# Gecko #
=====

Gecko is a MOOSE based application for coupling molecular dynamics simulations with continuum scale simulations. 
Lammps needs to be built before building gecko. For more information about downloading and building lammps, refer to the ReadMe.md file in https://bitbucket.org/amburan/lammps (moose-integration branch)


## FAQ ##

1) I have other versions of python installed. When I try to launch peacock I get the following error. What do I do?
```
#!bash
$ peacock -i some_input_file.i 
Traceback (most recent call last):
  File "/Users/your_user_name/projects/moose/gui/peacock", line 12, in <module>
    raise ImportError("Cannot load either PyQt or PySide")
ImportError: Cannot load either PyQt or PySide
```
A. Run the following command.
```
#!bash
$ module purge; module load moose-dev-clang moose-tools
```

2) I have built lammps, but the lammps shared library is not loaded and I get the following error:
```
#!bash
$ error while loading shared libraries: liblammps_mpi.so: cannot open shared object file: No such file or directory
```
A. Folow #STEP 5 in lammps installation: Add paths to lammps and atc shared libraries so that the executable knows where to find them. This variable is operating system dependent.

For Mac add the following lines in .bash_profile
```
#!bash
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$HOME/lammps/lib/atc
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/opt/moose/gcc_4.9.1/lib
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$HOME/lammps/src
```

For Linux add the following lines in .bashrc
```
#!bash
export LD_LIBRARY_PATH=$HOME/lammps/src:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$HOME/lammps/lib/atc:$LD_LIBRARY_PATH
```
Now gecko can be built and run!