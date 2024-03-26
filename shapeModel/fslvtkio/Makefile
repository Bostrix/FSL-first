include ${FSLCONFDIR}/default.mk

PROJNAME = fslvtkio
LIBS     = -lfsl-meshclass -lfsl-miscmaths

all: libfsl-vtkio.so

libfsl-vtkio.so: fslvtkio.o
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ ${LDFLAGS}
