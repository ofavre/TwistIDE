#!/bin/bash

cd "$(dirname "$0")"

target_dir="$(ls -d /usr/share/gcc-*/python/ 2>/dev/null | sort -g -r | head -n 1)"
if [ -z "$target_dir" ]; then
    echo "The target /usr/share/gcc-*/python/ folder is inexistent!"
    exit 1
fi

check_for_printer() {
    while [ "$#" -gt 0 ]; do
        if ! ls -d /usr/share/gcc-*/python/"$name" >/dev/null 2>&1; then
            return 1
        fi
        shift
    done
    return 0
}

get_stl_printers() {
    # Files already available
    if check_for_printer "libstdcxx"; then
        echo "STL pretty-printers already installed"
        return 0
    fi
    # Package already installed
    if dpkg --get-selections | grep -E 'libstdc\+\+6-.*-dbg'; then
        echo "The package looks already installed, but the expected files could not be found."
        echo "Ignoring STL printers..."
        return 2
    fi
    echo "Installing STL pretty-printers..."
    # Try to install the package
    # Take the most recent installed libstdc++6-*-dev package, transform it to -dbg
    candidate="$(dpkg --get-selections 'libstdc++6-*-dev' | sed -re 's/^([^ ]+)-dev\s+.*$/\1-dbg/' | sort -g -r | head -n 1)"
    # Check availability
    if [ "$(dpkg-query -l "$candidate" 2>/dev/null | wc -l)" -ne "0" ]; then
        if sudo apt-get install "$candidate"; then
            echo "STL pretty-printers installed with APT"
            # Success
            return 0
        fi
    fi
    # Try installing manually
    if svn co svn://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3/python stl-printers-python; then
        # Remove all .svn folders
        find stl-printers-python -type d -name '.svn' -exec rm -Rf {} +
        # Install
        sudo cp -R stl-printers-python/libstdcxx "$target_dir/"
        # Remove the checkout
        rm -Rf stl-printers-python/
        echo "STL pretty-printers installed with SVN checkout"
    fi
    echo "Cannot install STL pretty-printers!"
    return 1
}

install_llvm_printers() {
    if check_for_printer "llvm"; then
        echo "LLVM pretty-printers already installed"
        return 0
    fi
    echo "Installing LLVM pretty-printers..."
    sudo cp -R python/llvm "$target_dir/"
    echo "LLVM pretty-printers installed"
}

install_gdbinit() {
    echo "Modifying ~/.gdbinit to load the pretty-printers..."
    echo "" >> ~/.gdbinit
    echo "" >> ~/.gdbinit
    echo "#######################################################################" >> ~/.gdbinit
    echo "# Following is the automatic installation of STL+LLVM pretty-printers #" >> ~/.gdbinit
    echo "# done by TwistIDE/utils/gdb/setup.sh                                 #" >> ~/.gdbinit
    echo "#######################################################################" >> ~/.gdbinit
    echo "" >> ~/.gdbinit
    cat gdbinit >> ~/.gdbinit
    echo "" >> ~/.gdbinit
    echo "#######################################################################" >> ~/.gdbinit
    echo "# End of automatic installation                                       #" >> ~/.gdbinit
    echo "#######################################################################" >> ~/.gdbinit
    echo "" >> ~/.gdbinit
    echo "Done."
}

get_stl_printers
install_llvm_printers
install_gdbinit
