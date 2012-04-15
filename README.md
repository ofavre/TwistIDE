TwistIDE
========

Yes another C-lang IDE… but with a Twist !

This IDE project is dedicated to the C languages family and it won't be coded using Java or any other resource hog, this leaving you more free memory and offering closer toolchain integration (that's right, this IDE's I will finally mean something) thanks to the LLVM project.

Why another IDE? / Why not improving X instead ?
------------------------------------------------

-   C/C++ IDEs are not quite well functional
-   Their support for the language is often approximate, and code completion is just a pain
-   There are beautiful libs out there that are particularly well suited for this language family, just waiting for someone to do something – at least other than Apple and XCode :)
-   And Java IDEs are just too heavy


What is TwistIDE made of?
-------------------------

We will be using the LLVM, LLDB and Clang libraries/APIs for the C language family support (and integrated debugging).


How to start working ?
----------------------

1.  Make sure you have enough free space
2.  Clone the repository
3.  Install llvm/clang (for compilation), python-dev and swig (necessary for LLDB's python interface generation), libeditline-dev
4.  Run “get_llvm.sh”, which will checkout all the needed sources and start building on Debug
5.  Have a coffee/tea while compiling :)

