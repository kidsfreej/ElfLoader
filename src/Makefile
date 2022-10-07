PROJECT := main.exe

DIR_SRC += .

SRC_ASM += $(wildcard $(addsuffix /*.asm, $(DIR_SRC)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_SRC)))
OBJ := $(patsubst %.c, %.o, $(SRC_C))
OBJ += $(patsubst %.asm, %.o, $(SRC_ASM))
EXE := $(PROJECT)
DIR_INC=
CC_PREFIX := 
CC := $(CC_PREFIX)gcc
CLIB = -L .

.PHONY:all

all:$(OBJ) $(EXE) build run

%.o: %.c
	    $(CC) $(CFLAG) $(DIR_INC) -c $< -o $@
%.o : %.asm
	nasm  -felf64 $< -o $@
$(EXE): $(OBJ) 
	    $(CC) $(CFLAG) $(CLIB) $(OBJ) -o $@ 

clean:
	    rm -rf $(EXE) $(OBJ) 
build: $(EXE)
		cp $(EXE) $(BUILD_DIR)/$(EXE)
run:
	$(BUILD_DIR)/$(EXE)
