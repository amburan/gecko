app_INCLUDES += -I $$HOME/lammps/src
ADDITIONAL_LIBS := -L$$HOME/lammps/src -llammps_mpi $$HOME/lammps/lib/atc/libatc.a  -lblas -llapack -lpthread -lgfortran
