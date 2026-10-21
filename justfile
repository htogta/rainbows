default: build

build:
  mkdir -p build
  cc src/main.c src/rainbows.c src/devices/*.c \
    -I src/raylib/include \
    src/raylib/lib/libraylib.a \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -o build/rainbows

clean:
  rm -rf build
