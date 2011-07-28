.PHONY: all compile compile-debug run debug clean dist-clean
.DEFAULT_GOAL = all
.SECONDARY: build/%.o build/%.o.d

PROG_NAME := embedding_gvim

CXX := g++
CXX_FLAGS := -c -Wall -Wextra
CXX_FLAGS_DEBUG := -g3 -O0
CXX_FLAGS_RELEASE := -g1 -O2
CXX_LIBS := `pkg-config --cflags gtkmm-2.4`
LN := g++
LN_FLAGS :=
LN_FLAGS_DEBUG := -g3
LN_FLAGS_RELEASE := -g1
LN_LIBS := `pkg-config --libs gtkmm-2.4` -lpthread

all: compile

run: dist/$(PROG_NAME)
	$<

debug: dist/$(PROG_NAME)_d
	gdb $<

compile: dist/$(PROG_NAME)

compile-debug: dist/$(PROG_NAME)_d

clean:
	rm -Rf dist build
dist-clean: clean
	rm -Rf src/*~


dist:
	mkdir -p dist
build:
	mkdir -p build



dist/$(PROG_NAME): build/main.o | dist
	$(LN) $(LN_FLAGS) $^ -o $@ $(LN_FLAGS_RELEASE) $(LN_LIBS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(CXX_FLAGS_RELEASE) $(CXX_LIBS)

dist/$(PROG_NAME)_d: build/main.o.d | dist
	$(LN) $(LN_FLAGS) $^ -o $@ $(LN_FLAGS_DEBUG) $(LN_LIBS)

build/%.o.d: src/%.cpp | build
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(CXX_FLAGS_DEBUG) $(CXX_LIBS)

