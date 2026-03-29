# 32-bit Modular Arithmetic and SIMD Vectorisation

## Overview

This project proposes an implementation of 32-bit modular multiplications using several computational techniques and enables performance comparison by measuring their execution times.

## Prerequisites

- `doxygen` to generate the documentation
- `gnuplot` to visualize benchmark results

Installation on Ubuntu:

    sudo apt install doxygen gnuplot

## Documentation

For the documentation of this project, run

    doxygen && cd latex && make

There will be a file named `refman.pdf` will be generated.

## Building from source

This project assumes that [FLINT](https://flintlib.org/) is already installed. Otherwise, you can install it following the steps on the website.

### The Makefile

The Makefile automatically detects the host architecture and enables appropriate SIMD instructions (AVX2/AVX512) for optimal performance on x86_64 or ARM-based systems (NEON).

The project includes unit tests located in the `tests/` directory. To build and run all tests, run:

    make check

To remove all generated files (object files, the main executable, and test binaries), run

    make clean

To build the project, run

    make

## Use

To execute the project, run

    ./pcca7

### Command line flags

These flags are parsed by the `set_options` function and control the generation of the modulus `p` and the multiplier `b` used in the modular arithmetic benchmarks.

| Flag      | Argument  | Description                                                                    |
| --------- | --------- | ------------------------------------------------------------------------------ |
| `-p`      | `<prime>` | Sets the prime modulus `p` directly. Must be a prime number.                   |
| `-b`      | `<value>` | Sets the multiplier `b` directly. Must be less than `p`.                       |
| `-p_bits` | `<bits>`  | Generates a random prime modulus `p` with the given number of bits.            |
| `-b_bits` | `<bits>`  | Generates a random multiplier `b` with the given number of bits.               |
| `-scale`  | `<int>`   | Scaling factor for the number of points in the generated graphs (default = 1). |
| `-pts`    | `<int>`   | Number of measurement points (default = 100).                                  |

### Parameter Generation Rules

- If `-p` is given, a random `b` is generated (less than `p`) if `-b` isn't given.
- If `-b` is given, a random prime `p` is generated (greater than `b`) if `-p` isn't given.
- If `-p_bits` is given, `p` is generated with the specified number of bits with a random `b` (less than `p`).
- If `-b_bits` is given, `b` is generated with the specified number of bits with a random prime `p` (greater than `b`).
- If no parameter flags are supplied, random parameters are used.
- If an option is given several times, only the first one will be considered
- If `-x` and `-x_bits` with `x=a` or `x=b` are given, only the first one will be considered

For example, running

    ./pcca7 -p 1231 -b 2 -scale 10 -pts 100

will generate a graph for `p=1231` and `b=2` with 100 points at scale 10.

Running

    ./pcca7 -p 191 -p_bits 12

the program will only consider `191`. Therefore, the program will generate a graph for `p=191` and a random `b`.
