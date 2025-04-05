SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := build
CC := gcc

EXE := $(BIN_DIR)/server
SRC := $(wildcard $(SRC_DIR)/*.c)

OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CPPFLAGS := -Iinclude			 # -I is a preprocessor flag, not a compiler flag
CFLAGS   := -Wall -g3
LDFLAGS  := -Llib                                # -L is a linker flag
LDLIBS   := -lsrt 

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
		$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
		mkdir -p $@

clean:
		@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
