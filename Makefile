CC = clang
CFLAGS = -g -Wall -Wextra -pedantic -std=c11 -I./lib/raylib/src/ -I./lib/raygui/src/
LDFLAGS = -lraylib -L./lib/raylib/src/ -L/opt/homebrew/lib/ -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

$(BUILD_DIR)/main: $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

setup:
	@mkdir -p $(LIB_DIR)
	git clone --depth=1 https://github.com/raysan5/raylib $(LIB_DIR)/raylib
	git clone --depth=1 https://github.com/raysan5/raygui.git $(LIB_DIR)/raygui
	make -C $(LIB_DIR)/raylib/src

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean setup

