#include ${FSLCONFDIR}/default.mk

PROJNAME = first_lib
LIBS     = -lfsl-newimage -lfsl-miscmaths
SOFILES  = libfsl-first_lib.so

all: libfsl-first_lib.so

libfsl-first_lib.so: first_mesh.o first_newmat_vec.o
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ ${LDFLAGS}
