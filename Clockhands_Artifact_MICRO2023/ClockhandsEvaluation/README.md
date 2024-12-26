# Experimental setup

## build musl-gcc

To assemble for RISC-V, you need musl-gcc.
This procedure may take a long time.

```
cd A-riscv/
git clone https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain/

git checkout 2022.01.17
sudo apt install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev
CFLAGS="-O2 -static" ./configure --prefix=$(echo ~/riscv_gcc111) --with-arch=rv64g
make linux -j$(nproc)
make -j$(nproc)
cd ../

cd musl/
CC=~/riscv_gcc111/bin/riscv64-unknown-linux-gnu-gcc CROSS_COMPILE=~/riscv_gcc111/bin/riscv64-unknown-linux-gnu- ./configure --prefix=$(echo ~/musl-gcc) --target=riscv64
make -j$(nproc)
make install
cd ../../
```

## get clang 12.0.1

To compile for RISC-V, you need clang.

```
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.1/clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz
tar xf clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz
mv clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu- clang+llvm-12.0.1-x86_64-linux-gnu-ubuntu-16.04
```

## build assemblers

To assemble STRAIGHT and Clockhands, you need our go-based assembler.
We provide pre-built binaries.
You can build assemblers on your own, but you may need some old `go` compiler (we used go1.10.4).

## compile CoreMark

```
cd A-riscv/coremark/
make
cd ../../

cd B-straight/toolchain/Test/coremark/
make
cd ../../../../

cd C-clockhands/coremark/
make
cd ../../
```

## compile Onikiri2

We used a cycle-accurate simulator Onikiri2 for evaluation.
The compilation takes ~2 minutes.

```
cd onikiri2/project/gcc/
make -j
cd ../../../
```

## Setup

Transfer binaries created above.

```
cp A-riscv/coremark/rvbin/coremark.rvbin evaluation/0.coremark
cp B-straight/toolchain/Test/coremark/stbin/coremark.stbin evaluation/0.coremark
cp C-clockhands/coremark/chbin/coremark.chbin evaluation/0.coremark
cp onikiri2/project/gcc/onikiri2/a.out evaluation/onikiri2
```

# Experiment

Each simulation of 10-iteration CoreMark takes <5 minutes.

```
cd evaluation/
make -j
```

# Visualize

See instructions in the Excel file "ClockhandsEvaluation.xlsx."
