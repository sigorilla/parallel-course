# Lab 1
The solution of 1-D problem of convection–diffusion equation with scheme: `explicit left corner`.


# Usage
## Without *MPI*
Compile: `gcc <c-file> -o <execfile>`, `c-file` - your C code, `execfile` - name executable filename.

Execute: `./<execfile> <T> <X> <K> <M>`, `T` and `X` - maximum of time and coordinate, `K` and `M` - amount of time and coordinate.

*`T`, `X`, `K` and `M` are optional parameters.*

If the execution is successfully, the file with solution will be created in the same directory.

## With *MPI*
__TODO:: create file using MPI__
