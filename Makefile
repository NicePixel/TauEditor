CC=gcc
FLAGS=-Wall -std=c11 -g -O3

LIBS_WINDOWS =-L./lib/ -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm

SRC_DIR=src
OBJ_DIR=obj

BIN_DIR=bin
BIN=./$(BIN_DIR)/editor
RES_FILE=./$(BIN_DIR)/winres.res

all: 
	-@mkdir $(OBJ_DIR)
	-@mkdir $(BIN_DIR)
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/main.c      -o $(OBJ_DIR)/main.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/io.c        -o $(OBJ_DIR)/io.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/mapview.c   -o $(OBJ_DIR)/mapview.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/toolbox.c   -o $(OBJ_DIR)/toolbox.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/textures.c  -o $(OBJ_DIR)/textures.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/text.c      -o $(OBJ_DIR)/text.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/statusbar.c -o $(OBJ_DIR)/statusbar.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/map.c       -o $(OBJ_DIR)/map.o
	$(CC) $(FLAGS) obj/main.o obj/io.o obj/mapview.o obj/toolbox.o obj/textures.o obj/text.o obj/statusbar.o obj/map.o -o $(BIN) $(LIBS)

windows:
	-@mkdir $(OBJ_DIR)
	-@mkdir $(BIN_DIR)
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/main.c -o obj/main.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/io.c -o obj/io.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/mapview.c -o obj/mapview.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/toolbox.c -o obj/toolbox.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/textures.c -o obj/textures.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/text.c -o obj/text.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/statusbar.c -o obj/statusbar.o
	$(CC) $(FLAGS) $(DEF_OGL) -c $(SRC_DIR)/map.c -o obj/map.o
	$(CC) $(FLAGS) obj/main.o obj/io.o obj/mapview.o obj/toolbox.o obj/textures.o obj/text.o obj/statusbar.o obj/map.o $(RES_FILE) -o $(BIN) $(LIBS_WINDOWS)

