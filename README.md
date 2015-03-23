# OpenCL-Game-of-Life
An OpenCL/C++ implementation of the Conway's Game of Life

Marco Dos Santos Oliveira : marco.dossantosoliveira 'at' sunrise.ch

# Libraries
Minimum : OpenCL 1.2

# Compilation 

you can read the OpenCL official documentation for compilation or try to update this CLI based on your own compilation environment.

c++ -I /CUDAPATH/cuda/include/ -I /CUDAPATH/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc/ -L /CUDAPATH/cuda/lib64/ -l OpenCL *.cpp -o OpenCL_GOL
