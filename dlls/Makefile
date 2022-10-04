PROJECT := win_glibc.dll

DIR_SRC += .


SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_SRC)))
#OBJ := $(filter %.o, $(SRC_C:.c=.o))
OBJ := $(patsubst %.c, %.o, $(SRC_C))
DLL := $(PROJECT)
DIR_INC=
CC_PREFIX := 
CC := $(CC_PREFIX)gcc

CLIB = -L .

.PHONY:all

all:$(OBJ) $(DLL) build

%.o: %.c
	    $(CC) $(CFLAG) $(DIR_INC) -c $< -o $@ 

$(DLL): $(OBJ)
	    $(CC) $(CFLAG) $(CLIB) $(OBJ) -shared -o $@ 

build: $(DLL)
		cp $(DLL) $(BUILD_DIR)/$(DLL)

clean:
	    rm -rf $(DLL) $(OBJ) 
