set -e

FLAGS="-Wall -Wextra -Wpedantic -g -I./lib/raylib/src/ -lraylib -L./lib/raylib/src/ -L/opt/homebrew/lib/ -lm -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit"

make -C ./lib/raylib/src
clang src/main.c $FLAGS -o build/main