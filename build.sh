set -e

make -C ./lib/raylib/src
clang src/main.c -Wall -Wextra -Wpedantic -I./lib/raylib/src/ -lraylib -L./lib/raylib/src/ -L/opt/homebrew/lib/ -lm -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -o build/main