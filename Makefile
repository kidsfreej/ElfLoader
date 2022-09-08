exec_name = ELFLoader.exe
objects = buffer.o main.o elfloader.o datastructures.o
all: build run

build: $(objects)  $(exec_name)
$(objects):%.o: %.c
	
	gcc -gdwarf-2 -static-libgcc -c $^ 
$(exec_name): *.o
	gcc -gdwarf-2 -static-libgcc $^ -o $(exec_name)
run: $(exec_name)
	${exec_name}
