# Maximal Exact Matching for DNA Sequences

## Instructions for Compiling and Running the Code

1. Use the `make` command to compile the code and create the executable named `mem`.
2. The `mem` executable requires three file arguments in the following order:
   - The filename of the BWT (Burrows-Wheeler Transform) of the text.
   - The filename of the reversed BWT of the text.
   - The filename of the pattern to search for.

## Output

The program will print:

- The start and end index of each match (or "mem").
- The total number of matches found in the text.
