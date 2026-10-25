# Huffman Coding

Implementation in C of Huffman coding, a lossless data compression technique. The program counts the frequency of each byte of a file, builds a binary tree by repeatedly merging the two least frequent nodes (using a min-heap), and gives shorter binary codes to more frequent bytes. The data structures (heap, binary tree and code table) are generic, implemented with `void*` pointers.

The compressed `.huff` file starts with a 2-byte header (3 bits with the number of unused bits in the last byte and 13 bits with the size of the tree), followed by the tree in pre-order and then the encoded data.

## Getting the code

```
git clone https://github.com/Gsantsxz08/Projeto_Estrutura_de_Dados_2026.2.git
cd Projeto_Estrutura_de_Dados_2026.2/"2 - huffman"
```

Or download the repository as a ZIP from GitHub (Code > Download ZIP), extract it, and enter the `2 - huffman` folder. The folder name has spaces, so keep the quotes in `cd`.

## Compilation

From inside this folder:

```
gcc -Wall -o huffman huffman.c
```

## Execution

Compress:

```
./huffman c input_file [output_file]
```

Decompress:

```
./huffman d input_file.huff [output_file]
```

If the output file is omitted, compression writes `input_file.huff`, and decompression removes the `.huff` extension (or adds `.out` if there is none).

Example (with any text file named `text.txt` in this folder):

```
./huffman c text.txt
./huffman d text.txt.huff text_restored.txt
cmp text.txt text_restored.txt && echo "identical"
```

## Requirements

- Linux or macOS with `gcc`.
- Not for Windows; please use WSL or MinGW.
