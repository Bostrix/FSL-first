# Specify the default compiler
CXX = g++

# Specify the -fpic flag
CXXFLAGS += -fpic

# Define source files
SRCS = first.cc first_mult_bcorr.cc first_utils.cc

# Additional LDFLAGS for znzlib library
ZNZLIB_LDFLAGS = -L/home/ubuntu/first/znzlib -lfsl-znz
# Additional LDFLAGS for meshclass library
MESHCLASS_LDFLAGS = -L/home/ubuntu/first/meshclass -lfsl-meshclass
# Additional LDFLAGS for first_lib library
FIRSTLIB_LDFLAGS = -L/home/ubuntu/first/first_lib -lfsl-first_lib

# Define object files
OBJS = $(SRCS:.cc=.o)

# Define library source files and directories
LIB_DIRS = shapeModel first_lib fslvtkio meshclass newimage miscmaths NewNifti cprob znzlib utils 
LIB_SRCS = $(foreach dir,$(LIB_DIRS),$(wildcard $(dir)/*.cc))
LIB_OBJS = $(LIB_SRCS:.cc=.o)

# Define targets
all: first first_utils first_mult_bcorr

# Compile the final executables
first: libraries first.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ first.o $(LIB_OBJS) $(LDFLAGS) $(ZNZLIB_LDFLAGS) $(MESHCLASS_LDFLAGS) $(FIRSTLIB_LDFLAGS) -lblas -llapack -lz

first_utils: libraries first_utils.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ first_utils.o $(LIB_OBJS) $(LDFLAGS) $(ZNZLIB_LDFLAGS) $(MESHCLASS_LDFLAGS) $(FIRSTLIB_LDFLAGS) -lblas -llapack -lz

first_mult_bcorr: libraries first_mult_bcorr.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ first_mult_bcorr.o $(LIB_OBJS) $(LDFLAGS) $(ZNZLIB_LDFLAGS) $(MESHCLASS_LDFLAGS) $(FIRSTLIB_LDFLAGS) -lblas -llapack -lz


# Rule to build object files
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Phony target to build all libraries
.PHONY: libraries
libraries:
	@for dir in $(LIB_DIRS); do \
	$(MAKE) -C $$dir CXX=$(CXX) CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDFLAGS)'; \
	done

# Clean rule
clean:
	rm -f first $(OBJS) $(LIB_OBJS)
