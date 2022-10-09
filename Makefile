CC = gcc
CXX = clang++
# CXX = g++-12
CFLAGS = -pipe -Wall -Wextra -Wpedantic -Wsign-conversion -Wshadow -O3 -march=native -flto -I/usr/include/apr-1.0 
LDLIBS  = -lapr-1

objects = build/binarytrees.gcc-2.gcc_run \
 build/binarytrees.gcc-3.gcc_run \
 build/binarytrees.rust \
 build/binarytrees.gpp-9.run \
 build/binarytrees.gpp-10.run \
 build/binarytrees.gpp-11.run


clean:
	rm -f $(objects)
	rm -rf target/*

build/binarytrees.gcc-2.gcc_run: src/binarytrees/binarytrees.gcc-2.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

build/binarytrees.gcc-3.gcc_run: src/binarytrees/binarytrees.gcc-3.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

build/binarytrees.gpp-9.run: src/binarytrees/binarytrees.gpp-9.cpp
	$(CXX) $(CFLAGS) -std=c++20 $< -o $@ $(LDLIBS)

build/binarytrees.gpp-10.run: src/binarytrees/binarytrees.gpp-10.cpp
	$(CXX) $(CFLAGS) -static -std=c++20 $< -o $@

build/binarytrees.gpp-11.run: src/binarytrees/binarytrees.gpp-11.cpp src/binarytrees/arena.h
	$(CXX) $(CFLAGS) -std=c++20 $< -o $@ -ltbb

# clang++ -pipe -Wall -O3 -fomit-frame-pointer -march=native -flto -I/usr/include/apr-1.0 --std=c++20 src/binarytrees/binarytrees.gpp-10.cpp -o build/binarytrees.gpp-10.run -ltbb -lapr-1


build/binarytrees.rust: src/binarytrees/binarytrees.rs
	cargo build --release
	cp target/release/binarytrees build/binarytrees.rust

all: $(objects)
