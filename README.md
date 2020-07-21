# Matrices in C++

Yet another C++ implementation of matrix operations, with multithreading. Is a learning project. It uses the threadpool I made, which can be found [here](https://github.com/abhikjain360/threadpool).

Still WIP. Currently only has basic operations defined using threadpool, but those still outperform your typical ones.

Matrix library once made will act as a base for other things like optimization techniques, linear equations solver etc.

## TODO:

- [X] Basic operations
- [X] New class for submatrices
- [X] Normal implementation for comparision
- [X] Add strassen's algorithm for multiplication
- [X] Use templates
- [ ] Optimize for special cases like single row/column matrices, identity matrices etc.
- [ ] Shell scripts for automated testing & benchmarking
- [ ] Test with other threadpool libraries to see if performance improves
- [ ] GPU acceleration using OpenACC or OpenGL
- [ ] Linear Algebra operations like Echelon form, reduction etc.
- [ ] Linear equations solver
- [ ] Optimization Techniques implementation (Simplex method, dual-simplex method)
