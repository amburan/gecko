# Gecko #
=====

Gecko is a MOOSE based application for coupling molecular dynamics simulations with continuum scale simulations. 
Lammps needs to be built before building gecko. For more information about downloading and building lammps, refer to the ReadMe.md file in https://bitbucket.org/amburan/lammps (moose-integration branch)


## FAQ ##
===
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