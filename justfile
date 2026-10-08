default: run

run: build
  ./build/rainbows test.rom

build:
  mkdir -p build
  cc -o build/rainbows src/main.c src/rainbows.c src/devices/console.c src/devices/arith.c src/devices/datetime.c src/devices/disk.c

clean:
  rm -rf build
