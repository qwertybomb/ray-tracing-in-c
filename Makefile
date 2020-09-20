cc = clang
flags = -Wall -Wl,-s -O3 -fopenmp -march=native
linker_flags = -lmingw32 -lSDL2main -lSDL2 -l.\libomp random.o 

cpp = g++
cpp_flags = -Wall -O3 -march=native -c

all: ray_trace.c random.o
	$(cc) $(flags) ray_trace.c -o main $(linker_flags)

random.o: random.cc
	$(cpp) $(cpp_flags) random.cc -o random.o

clean:
	rm *.o *.exe 
