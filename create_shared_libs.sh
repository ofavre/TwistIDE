for build in "Debug+Asserts" "Debug" "Release+Asserts" "Release"; do
	cd "$(dirname "$0")/llvm-build/$build"  2>/dev/null || continue
	mkdir -p shared-libs/
	for lib in $(find lib/ -name '*.a'); do
		lib="${lib#lib/}"
		lib="${lib%.a}"
		[ -f "shared-libs/$lib.so" ] && continue
		dir="$(dirname "$lib")"
		mkdir -p "shared-libs/$dir/"
		echo "Building $build/shared-libs/$lib.so..."
		g++ -g3 -fPIC -shared -Wl,--export-dynamic -o "shared-libs/$lib.so" -Wl,--whole-archive "lib/$lib.a" -Wl,--no-whole-archive
	done
done
