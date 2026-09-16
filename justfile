default: run

run: build
  ./build/rainbows

build:
  mkdir -p build
  cc -o build/rainbows src/main.c

clean:
  rm -rf build
