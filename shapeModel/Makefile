#include ${FSLCONFDIR}/default.mk

PROJNAME = shapeModel
SOFILES  = libfsl-shapeModel.so
LIBS     = -lfsl-first_lib -lfsl-vtkio -lfsl-newimage

# Add the current directory and newimage directory to the include path
CXXFLAGS += -fpic -I$(CURDIR) -I$(CURDIR)/newimage

all: libfsl-shapeModel.so

libfsl-shapeModel.so: shapeModel.o
	${CXX} ${CXXFLAGS} -shared -o $@ $^ ${LDFLAGS}
