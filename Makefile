cc = clang
flags = -Wall  -fopenmp -Ofast -march=native
linker_flags = -lmingw32 -lSDL2main -lSDL2 -llibomp random.o 

cpp = clang++
cpp_flags = -Wall -Ofast -c

all: ray_trace.c random.o
	$(cc) $(flags) ray_trace.c -o main $(linker_flags)

random.o: random.cc
	$(cpp) $(cpp_flags) random.cc -o random.o
