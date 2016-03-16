app_INCLUDES += -I $$HOME/projects/lammps/src
ADDITIONAL_LIBS := -L$$HOME/projects/lammps/src -llammps_mpi $$HOME/projects/lammps/lib/atc/libatc.a  -lblas -llapack -lpthread -lgfortran
