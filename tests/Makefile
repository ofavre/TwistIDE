# Define the main program name
PROG_NAME := embedding_gvim

# Folder definitions
INCLUDE_DIR := include
SRC_DIR := src
BUILD_DIR := build
DIST_DIR := dist


CXX := g++
CXX_FLAGS_COMPILATION := -c -Wall -Wextra
CXX_FLAGS_INCLUDE_FOLDER := -I$(INCLUDE_DIR)
CXX_FLAGS_LIBS := `pkg-config --cflags gtkmm-2.4`
CXX_FLAGS := $(CXX_FLAGS_COMPILATION) $(CXX_FLAGS_INCLUDE_FOLDER) $(CXX_FLAGS_LIBS)
CXX_FLAGS_DEBUG := -g3 -O0
CXX_FLAGS_RELEASE := -g1 -O2
LN := g++
LN_FLAGS := 
LN_FLAGS_DEBUG := -g3
LN_FLAGS_RELEASE := -g1
LN_LIBS := `pkg-config --libs gtkmm-2.4` -lpthread

# General extension definitions
PROG_EXT :=
PROG_EXT_DEBUG := _d
OBJ_EXT := o
OBJ_EXT_DEBUG := o.d

# Build final program file names
PROG_DEBUG := $(DIST_DIR)/$(PROG_NAME)$(PROG_EXT_DEBUG)$(PROG_EXT)
PROG := $(DIST_DIR)/$(PROG_NAME)$(PROG_EXT)

# List source file (and strip path)
SRC_FN := $(patsubst $(SRC_DIR)/%, %, $(wildcard $(SRC_DIR)/*.cpp))
# Derive object file names
OBJ_FN := $(patsubst %.cpp, %.$(OBJ_EXT), $(filter %.cpp,$(SRC_FN)))
OBJ_DEBUG_FN := $(patsubst %.cpp, %.$(OBJ_EXT_DEBUG), $(filter %.cpp,$(SRC_FN)))
# Construct final file names by prepending the folder path
SRC := $(addprefix $(SRC_DIR)/, $(SRC_FN))
OBJ := $(addprefix $(BUILD_DIR)/, $(OBJ_FN))
OBJ_DEBUG := $(addprefix $(BUILD_DIR)/, $(OBJ_DEBUG_FN))

# Template defining header dependencies for a source file
define TEMPLATE_SOURCE_HEADER_DEPENDENCIES
# We have to strip the slashes and newlines, because they are taken for escaped spaces and count for a dependency!
$(eval $(shell $(CXX) $(CXX_FLAGS_INCLUDE_FOLDER) -MM $(SRC_DIR)/$(1).cpp -MT $(BUILD_DIR)/$(1).$(OBJ_EXT) -MT $(BUILD_DIR)/$(1).$(OBJ_EXT_DEBUG) | tr -d '\\\n'))
endef
# Each source file depends on its include dependencies
$(foreach src_fn,$(patsubst %.cpp,%,$(SRC_FN)), $(eval $(call TEMPLATE_SOURCE_HEADER_DEPENDENCIES,$(src_fn))))



# General make targets configuration
.DEFAULT_GOAL = all
.PHONY: all compile compile-debug run debug clean dist-clean
.SECONDARY: $(OBJ) $(OBJ_DEBUG)



#
# All targets of this makefile
#

# Default target
all: compile

# Running targets
run: dist/$(PROG_NAME)
	$<

debug: dist/$(PROG_NAME)_d
	gdb $<

# Compile targets
compile: $(PROG)

compile-debug: $(PROG_DEBUG)

# Householding targets
clean:
	rm -Rf $(OBJ) $(OBJ_DEBUG) $(PROG) $(PROG_DEBUG)
dist-clean: clean
	rm -Rf $(DIST_DIR) $(BUILD_DIR)
	find -name '*~' -exec rm -f {} \;



#
# File targets
#

# Ensure creation of necessary folders
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
$(DIST_DIR):
	mkdir -p $(DIST_DIR)



# Compilation of the main program
$(PROG): $(OBJ) | $(DIST_DIR)
	$(LN) $(LN_FLAGS) $(LN_FLAGS_RELEASE) $(LN_LIBS) -o $@ $^

$(PROG_DEBUG): $(OBJ_DEBUG) | $(DIST_DIR)
	$(LN) $(LN_FLAGS) $(LN_FLAGS_DEBUG) $(LN_LIBS) -o $@ $^

# Object creation for the main program
$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_RELEASE) -o $@ $<

$(BUILD_DIR)/%.$(OBJ_EXT_DEBUG): $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) -o $@ $<
