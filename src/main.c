#define OK_IMPLEMENTATION
#include "ok.h"
#include "raylib.h"
#include "rainbows.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TICKS_PER_FRAME (50000) // TODO tune this

// wraps the device's framebuffer as an Image and uploads it to the GPU
static Texture2D make_screen_texture(const RbScreen* s) {
  Image img;
  img.data = s->fb;
  img.width = s->width;
  img.height = s->height;
  img.mipmaps = 1;
  img.format = PIXELFORMAT_UNCOMPRESSED_R5G6B5;

  // don't UnloadImage, fb belongs to the device
  Texture2D t = LoadTextureFromImage(img);  
  SetTextureFilter(t, TEXTURE_FILTER_POINT);
  return t;
}

static Rainbows bus;

uint8_t ok_mem_read(size_t address) {
  return rb_read(&bus, address);
}

void ok_mem_write(size_t address, uint8_t val) {
  rb_write(&bus, address, val);
}

uint8_t ok_fetch(size_t address) {
  return bus.program[address];
}

int main(int argc, char* argv[]) {
  RbScreen screen = {0};
  BlockFile* bf = NULL;
  char default_path[4096];
  char* bf_path = NULL;
  size_t capacity = 65536;

  if (argc < 2) {
    fprintf(stderr, 
    "usage: rainbows program.rom [-c capacity] [-d file.blocks | -n file.blocks]\n");
    return 1;
  }

  for (int i = 2; i < argc; i++) {
    if (!strcmp(argv[i], "-d")) {
      bf_path = argv[++i];
      bf = blockfile_open(bf_path);
    } else if (!strcmp(argv[i], "-n")) {
      bf_path = argv[++i];
      bf = blockfile_create(bf_path, capacity);
    } else if (!strcmp(argv[i], "-c")) {
      capacity = strtoul(argv[++i], NULL, 10);
    } else {
      fprintf(stderr, "unknown option: %s\n", argv[i]);
      return 1;
    }
  }

  if (bf_path == NULL) {
    snprintf(default_path, sizeof(default_path), "%s/.blocks", getenv("HOME"));
    bf_path = default_path;
  
    if (access(bf_path, F_OK) == 0) {
      bf = blockfile_open(bf_path);
      if (capacity != 65536) {
        fprintf(stderr, "warning: %s already exists; ignoring -c\n", bf_path);
      }
    } else {
        bf = blockfile_create(bf_path, capacity);
    }
  }
  
  if (!bf) {
    printf("Failed to load disk from .blocks file\n");
    return 1;
  }

  uint8_t* ram = calloc(OK_MEM_SIZE, 1);
  uint8_t* program = calloc(OK_MEM_SIZE, 1);
  if (!ram || !program) {
    printf("Failed to init rainbows\n");
    blockfile_close(bf);
    return 1;
  }
  
  if (!ok_load_file(program, 0, argv[1])) {
    printf("Failed to load file %s\n", argv[1]);
    goto had_error;
  }

  // initialize the console
  RbConsole console;
  if (!rb_console_init(&console, argc, argv)) {
    printf("Failed to init console device\n");
    goto had_error;
  }

  // initialize the math device
  RbArith arith;
  if (!rb_arith_init(&arith)) {
    printf("Failed to init arithmetic device\n");
    goto had_error;
  }

  // initialize the disk device
  RbDisk disk;
  if (!rb_disk_init(&disk, bf)) {
    printf("Failed to init disk device\n");
    goto had_error;
  }

  // initialize the time device
  RbDatetime dt;
  if (!rb_datetime_init(&dt)) {
    printf("Failed to init datetime device\n");
    goto had_error;
  }

  // initialize the float device
  RbFloat flt;
  if (!rb_float_init(&flt)) {
    printf("Failed to init float device\n");
    goto had_error;
  }

  // initialize the screen device
  if (!rb_screen_init(&screen)) {
    printf("Failed to init screen device\n");
    goto had_error;
  }

  rb_init(&bus, ram, program, &console, &arith, &disk, &dt, &flt, &screen);

  // TODO remove
  // printf("arith base = %x\n", RB_ARITH_BASE);
  // printf("screen base = %x\n", RB_SCREEN_BASE);
  // printf("console base = %x\n", RB_CONSOLE_BASE);
  // printf("datetime base = %x\n", RB_DATETIME_BASE);
  // printf("disk base = %x\n", RB_DISK_BASE);
  // printf("fpu base = %x\n", RB_FLOAT_BASE);

  // raylib stuff
  SetTraceLogLevel(LOG_ERROR); 
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screen.width, screen.height, "rainbows");
  SetTargetFPS(60);

  Texture2D tex = {0};

  OkState vm;
  ok_init(&vm);

  while (!WindowShouldClose() && vm.status == OK_RUNNING) {
    // run this frame's slice of vm time
    for (int i = 0; i < TICKS_PER_FRAME && vm.status == OK_RUNNING; i++) {
      ok_tick(&vm);
    }

    // vblank
    screen.frame_counter++;

    // sync with framebuffer
    if (screen.resized) {
      screen.resized = 0;
      if (tex.id) UnloadTexture(tex);
      tex = make_screen_texture(&screen);
    } else {
      UpdateTexture(tex, screen.fb);
    }

    BeginDrawing();
      ClearBackground(BLACK);
      DrawTexturePro(tex,
        (Rectangle){0, 0, screen.width, screen.height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
  }

  int status = vm.status;
  if (tex.id) UnloadTexture(tex);
  CloseWindow();

  free(ram);
  free(program);
  blockfile_close(bf);
  rb_screen_free(&screen);
  return status != OK_HALTED && status != OK_RUNNING;

  had_error:
    free(ram);
    free(program);
    blockfile_close(bf);
    rb_screen_free(&screen);
    return 1;
}
