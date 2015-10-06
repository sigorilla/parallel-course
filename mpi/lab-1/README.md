# Lab 1
The solution of 1-D problem of convection–diffusion equation with scheme: `explicit left corner`.

__[Source of the problem](http://mipt.ru/drec/upload/7c2/lab1-arpgyfe1u5e.pdf)__

# Make

`make`

Then you have two executable files:

1. `lab1.simple.c` — not parallel program
2. `lab1.parallel.c` — parallel program

# Usage
## Without *MPI*
Compile: `gcc <c-file> -o <execfile>`, `c-file` - your C code, `execfile` — name executable filename.

Execute: `./<execfile> <T> <X> <K> <M>`, `T` and `X` — maximum of time and coordinate, `K` and `M` — amount of time and coordinate.

*`T`, `X`, `K` and `M` are optional parameters.*

If the execution is successfully, the file with solution will be created in the same directory.

## With *MPI*
__Parameters__
* `h` — step for X
* `L` — length of X 
* `tau` — step for T
* `T` — length of T

With `graph.py` you can plot figure in Python.
