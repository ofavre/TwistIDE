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
LD_FLAGS :=
LD_FLAGS_RELEASE := -g1
LD_FLAGS_DEBUG := -g3
LD_LIBS_LLVM := `llvm-config --ldflags --libs`



#
# Targets and parts identification
#

# List each src/*.cpp file and keep the "*" part
TUTORIAL_PARTS := $(patsubst src/%.cpp, %, $(wildcard src/*.cpp))
# Derive objects and final program names
OBJS := $(addprefix build/, $(patsubst %, %.o, $(TUTORIAL_PARTS)))
OBJS_DEBUG := $(addprefix build/, $(patsubst %, %.o.d, $(TUTORIAL_PARTS)))
PROGS := $(addprefix dist/, $(patsubst %, %, $(TUTORIAL_PARTS)))
PROGS_DEBUG := $(addprefix dist/, $(patsubst %, %_d, $(TUTORIAL_PARTS)))

.SECONDARY: $(OBJS) $(OBJS_DEBUG)

# Headers: everything not taken by TUTORIAL_PARTS
HEADERS := $(filter-out $(patsubst %,src/%.cpp,$(TUTORIAL_PARTS)),$(wildcard src/*))


#
# Program specific configuration
#

# Define LD_PROG_CUSTOM_FLAGS to the content
# of a line preceded by '//! Libs: ' in the
# beginning of the source file
define TEMPLATE_custom_flags
.PHONY: custom-flags-$(1)
custom-flags-$(1): src/$(1).cpp
custom-flags-$(1): $(eval LD_PROG_CUSTOM_FLAGS := $(shell sed -nre '1,20s/^\/\/! Libs: //p' src/$(1).cpp))
endef
# Now define those parts for each tutorial part program
$(foreach part,$(TUTORIAL_PARTS),$(eval $(call TEMPLATE_custom_flags,$(part))))


#
# General targets
#

all: compile
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
dist/%_d: build/%.o.d | dist custom-flags-%
	$(LD) $(LD_FLAGS) $^ -o $@ $(LD_FLAGS_DEBUG) $(LD_PROG_CUSTOM_FLAGS) $(LD_LIBS_LLVM)
build/%.o.d: src/%.cpp | build src/*.hpp src/*.inl src/*.tcc
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(CXX_FLAGS_DEBUG)
dist/%: build/%.o | dist custom-flags-%
	$(LD) $(LD_FLAGS) $^ -o $@ $(LD_FLAGS_RELEASE) $(LD_PROG_CUSTOM_FLAGS) $(LD_LIBS_LLVM)
build/%.o: src/%.cpp | build $(HEADERS)
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(CXX_FLAGS_RELEASE)

