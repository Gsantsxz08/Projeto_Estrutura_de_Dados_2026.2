## Compilation

```
gcc -Wall -o sat sat.c
```

## Execution

```
./sat file.cnf
```

Output for a satisfiable formula (the first line reads `SAT!`, followed by the model, where a negative number indicates a false variable assignment, and `0` marks the end of the line):

```
SAT!
-1 2 3 0
```

Output for an unsatisfiable formula:

```
UNSAT!
```

## Tests

The `casos_sat/` directory contains 10 satisfiable instances, and the `casos_unsat/` directory contains 10 unsatisfiable instances, sourced from [SATLIB](https://cs.ubc.ca/~hoos/SATLIB/benchm.html)

To run all 20 test cases (from the repository root, after compilation):

```
for f in casos_sat/*.cnf casos_unsat/*.cnf; do ./sat $f; done
``` do echo "$f: $(./sat "$f" | head -1)"; done
```

Expected result: `SAT!` for the first 10, and `UNSAT!` for the last 10.

## Requirements

- Linux or macOS with `gcc`.
- Not for Windows; please use WSL or MinGW.
