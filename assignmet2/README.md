clang -fPIC -shared c_interface.c -o c_interface.so

g++ hello.c -I. -L. -lc_interface -o hello

LD_LIBRARY_PATH=$PWD ./hello