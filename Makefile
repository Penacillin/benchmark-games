build/binarytrees.gcc-2.gcc_run: src/binarytrees/binarytrees.gcc-2.c
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -flto -fopenmp -I/usr/include/apr-1.0 $< -o $@ -lapr-1

build/binarytrees.rust: src/binarytrees/binarytrees.rs
	cargo build --release
	cp target/release/binarytrees build/binarytrees.rust

all: build/binarytrees.gcc-2.gcc_run build/binarytrees.rust
	gcc -pipe -Wall -O3 -fomit-frame-pointer -march=native -flto -fopenmp src/fannkuchredux.gcc-5.c -o ./build/fannkuchredux.gcc-5.gcc_run
