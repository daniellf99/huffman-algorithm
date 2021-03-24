# Huffman Algorithm

An implementation of the huffman algorithm in C++.

The program will compress a target file using the huffman algorithm to `./huffman.freq`.
After that, the program will decompress it and save the output to `./huffman.defreq`.

Please note that the tree structure is NOT save inside the `.freq` file.
However, this is intended behavior for a future version.

## Building the project

To build, we need [CMake](https://cmake.org/).

Simply run `cmake .` at the project root directory.

## Usage

After successfully building the project, you can now run:

`huffman /path/to/target`