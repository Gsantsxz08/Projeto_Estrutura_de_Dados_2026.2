# SAT Solver (DPLL with binary tree)

SAT solver in C that reads DIMACS CNF files and uses the DPLL algorithm (unit propagation and backtracking). The search is represented by an explicit binary tree: the left child assigns the chosen variable to true, and the right child assigns it to false.

## Getting the code

```
git clone https://github.com/USER/REPOSITORY.git
cd REPOSITORY/"3- sat solver"
```

Or download the repository as a ZIP from GitHub (Code > Download ZIP), extract it, and enter the `3- sat solver` folder. The folder name has spaces, so keep the quotes in `cd`.

## Compilation

From inside the `3- sat solver` folder:

```
gcc -Wall -o sat sat.c
```

## Execution

```
./sat file.cnf
```

Output for a satisfiable formula (the first line reads `SAT!`, followed by the model, where a negative number indicates a false variable and `0` marks the end of the line):

```
SAT!
-1 2 3 0
```

Output for an unsatisfiable formula:

```
UNSAT!
```

## Tests

The `sat-testes/casos_sat/` directory contains 10 satisfiable instances, and `sat-testes/casos_unsat/` contains 10 unsatisfiable instances, sourced from [SATLIB](https://cs.ubc.ca/~hoos/SATLIB/benchm.html).

To run all 20 test cases (from inside the `3- sat solver` folder, after compiling):

```
for f in sat-testes/casos_sat/*.cnf sat-testes/casos_unsat/*.cnf; do echo "$f: $(./sat "$f" | head -1)"; done
```

Expected result: `SAT!` for the first 10 lines, and `UNSAT!` for the last 10.

## Requirements

- Linux or macOS with `gcc`.
- Not for Windows; please use WSL or MinGW.
