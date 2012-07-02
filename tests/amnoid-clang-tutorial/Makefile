.PHONY: all distclean clean compile compile-debug debug
.DEFAULT_GOAL = all



#
# Tools configuration
#

CXX := g++
CXX_FLAGS := -c -Wall -Wextra `llvm-config --cxxflags`
CXX_FLAGS_RELEASE := -g1 -O2
CXX_FLAGS_DEBUG := -g3 -O0
LD := g++
LD_FLAGS := `llvm-config --ldflags`
LD_FLAGS_RELEASE := -g1
LD_FLAGS_DEBUG := -g3



#
# Targets and parts identification
#

# List each src/*.cpp file and keep the "*" part
TUTORIAL_PARTS := $(patsubst src/%.cpp, %, $(wildcard src/*.cpp))
# Derive objects and final program names
PROGS := $(addprefix dist/, $(patsubst %, %, $(TUTORIAL_PARTS)))
PROGS_DEBUG := $(addprefix dist/, $(patsubst %, %_d, $(TUTORIAL_PARTS)))
OBJS := $(addprefix build/, $(patsubst %, %.o, $(TUTORIAL_PARTS)))
OBJS_DEBUG := $(addprefix build/, $(patsubst %, %.o.d, $(TUTORIAL_PARTS)))

# Keep intermediate objects
.SECONDARY: $(OBJS) $(OBJS_DEBUG)

#
# Program specific configuration
#

# We use sed to generate shell commands that will convert "//! Libs: " and "//! LLVM-Components: " comments into the final values.
# It is important to preserve the order of the comments are ld(1) is order sensitive (every symbols must be defined before linked against).
custom_flags = \
	$(shell $(shell sed -nre '{ s/^\/\/! Libs: +(.+)$$/echo "\1";/p; t; s/^\/\/! LLVM-Components: +(.+)$$/llvm-config --libs \1;/p; }' src/$(1).cpp))


#
# General targets
#

all: compile compile-debug
compile: $(PROGS)
compile-debug: $(PROGS_DEBUG)

clean:
	rm -Rf build/ dist/
distclean: clean
	rm -Rf src/*~

# Folder creation
build:
	mkdir -p $@
dist:
	mkdir -p $@

# File building
dist/%_d: build/%.o.d | dist
	$(LD) $(LD_FLAGS) $^ -o $@ $(LD_FLAGS_DEBUG) $(call custom_flags,$*)
build/%.o.d: src/%.cpp | build
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(CXX_FLAGS_DEBUG)
dist/%: build/%.o | dist
	$(LD) $(LD_FLAGS) $^ -o $@ $(LD_FLAGS_RELEASE) $(call custom_flags,$*)
build/%.o: src/%.cpp | build
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(CXX_FLAGS_RELEASE)

