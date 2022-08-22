# Project Description
This project aims at solving the traveling salesman problem (TSP) using the Held-Karp Algorithm. Xianqi Wang is responsible for creating the OpenMP files; Zemeng Han is responsible for creating the Pthreads files.

# How to Use the Programs
Go to OpenMP or Pthreads directory. Type "make build" to build all programs in the directory. The syntax to run a program is:<br/>
./<name_of_program> <input_file> <number_of_thread><br/>
Type "make clean" to remove all programs.

# Input File Format
A vaild input file should have the following structure:<br/>
<number_of_vertices><br/>
<x_coordinate_for_vertex_0> <y_coordinate_for_vertex_0><br/>
<x_coordinate_for_vertex_1> <y_coordinate_for_vertex_1><br/>
...<br/>
<x_coordinate_for_vertex_n-1> <y_coordinate_for_vertex_n-1><br/>

To ensure the correctness of the program, make sure that every line has at most 50 characters, there is exactly 1 space between x coordinate and y coordinate, and the input file must be in the same directory as the program. Test_Case directory contains several sample input files. 
