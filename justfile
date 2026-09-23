default: run

run: build
  ./build/rainbows test.rom

build:
  mkdir -p build
  cc -o build/rainbows src/main.c src/rainbows.c src/devices/console.c

clean:
  rm -rf build
