cc = clang
flags = -Wall -Ofast -fopenmp -march=native
linker_flags = -lSDL2 -lomp random.o 

cpp = clang++
cpp_flags = -Wall -Ofast -c

all: ray_trace.c random.o
	$(cc) $(flags) ray_trace.c -o main $(linker_flags)

random.o: random.cc
	$(cpp) $(cpp_flags) random.cc -o random.o
