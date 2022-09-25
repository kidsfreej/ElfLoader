PROJECT := main.exe

DIR_SRC += .


SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_SRC)))
#OBJ := $(filter %.o, $(SRC_C:.c=.o))
OBJ := $(patsubst %.c, %.o, $(SRC_C))
EXE := $(PROJECT)
DIR_INC=
CC_PREFIX := 
CC := $(CC_PREFIX)gcc
CFLAG = -gdwarf-2
CLIB = -L .

.PHONY:all

all:$(OBJ) $(EXE) run

%.o: %.c
	    $(CC) $(CFLAG) $(DIR_INC) -c $< -o $@ 

$(EXE): $(OBJ)
	    $(CC) $(CFLAG) $(CLIB) $(OBJ) -o $@ 

clean:
	    rm -r $(EXE) $(OBJ) 

run:
	./$(EXE)
