# LLVM12 prebuild binaries
clang12 := ../../../clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04/bin/clang
llvm-link12 := ../../../clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04/bin/llvm-link
opt12 := ../../../clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04/bin/opt
llc12 := ../../../clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04/bin/llc

../clockhands-assembler2/clockhands-assembler2 : ../clockhands-assembler2/*.*
	cd ../clockhands-assembler2; go build

../linker/linker : ../linker/*.*
	cd ../linker; make
