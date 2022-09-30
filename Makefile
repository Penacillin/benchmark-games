CC = clang
CXX = clang++
CFLAGS = -pipe -Wall -O3 -fomit-frame-pointer -march=native -flto -fopenmp -I/usr/include/apr-1.0
LDLIBS  = -lapr-1

objects = build/binarytrees.gcc-2.gcc_run \
 build/binarytrees.gcc-3.gcc_run \
 build/binarytrees.rust \
 build/binarytrees.gpp-9.run \
 build/binarytrees.gpp-10.run


clean:
	rm -f $(objects)
	rm -rf target/*

build/binarytrees.gcc-2.gcc_run: src/binarytrees/binarytrees.gcc-2.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

build/binarytrees.gcc-3.gcc_run: src/binarytrees/binarytrees.gcc-3.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

build/binarytrees.gpp-9.run: src/binarytrees/binarytrees.gpp-9.cpp
	$(CXX) $(CFLAGS) -static-libstdc++ $< -o $@ $(LDLIBS)

build/binarytrees.gpp-10.run: src/binarytrees/binarytrees.gpp-10.cpp
	$(CXX) $(CFLAGS) -static-libstdc++ --std=c++20 $< -o $@ -ltbb

build/binarytrees.rust: src/binarytrees/binarytrees.rs
	cargo build --release
	cp target/release/binarytrees build/binarytrees.rust

all: $(objects)
