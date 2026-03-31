#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"

static const char *RAMP = " .',:_-\"^;!/\\*?<>|+()=][}{%$&#O0@";

typedef struct {
  float r, g, b, brightness;
} BlockInfo;

BlockInfo block_sample(unsigned char *pixels, int img_w, int channels, int bx, int by, int bw, int bh);
void set_raw_mode(int enable) {
  static struct termios original;
  if (enable) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &original);
    raw = original;
    raw.c_lflag &= ~(ICANON | ECHO); // disable line buffering and echo
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  } else {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
  }
}

struct winsize w;

int main(int argc, char **argv) {
  int RAMP_LEN = strlen(RAMP);
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <image.jpg>\n", argv[0]);
    return 1;
  }

  int img_w, img_h, channels;
  unsigned char *pixels = stbi_load(argv[1], &img_w, &img_h, &channels, 3);
  channels = 3; // we forced RGB above
  if (!pixels) {
    fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
    return 1;
  }

  printf("\033[2J");   // clear screen
  printf("\033[H");    // move cursor to top left
  printf("\033[?25l"); // hide cursor
  fflush(stdout);

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  int ROWS = w.ws_row > 0 ? w.ws_row - 1 : 40;
  int cell_w = w.ws_xpixel > 0 ? w.ws_xpixel / w.ws_col : 0;
  int cell_h = w.ws_ypixel > 0 ? w.ws_ypixel / ROWS : 0;
  float ratio = (cell_w > 0 && cell_h > 0) ? (float)cell_h / cell_w : 2.0f;

  const int COLS = ROWS * (ratio * (float)img_w / img_h);

  // 32 bytes per block is enough for the escape sequence + 2 chars + reset
  char *line = malloc(COLS * 32);
  if (!line) {
    fprintf(stderr, "Failed to allocate line buffer\n");
    stbi_image_free(pixels);
    return 1;
  }

  // Bold once up front — reset at end of each row clears it,
  // so re-emit at the start of each row inside the loop
  for (int row = 0; row < ROWS; row++) {
    int pos = 0;
    int prev_ri = -1, prev_gi = -1, prev_bi = -1;

    pos += sprintf(line + pos, ANSI_BOLD);

    for (int col = 0; col < COLS; col++) {
      int bx = (col * img_w) / COLS;
      int by = (row * img_h) / ROWS;
      int bw = ((col + 1) * img_w) / COLS - bx;
      int bh = ((row + 1) * img_h) / ROWS - by;

      BlockInfo b = block_sample(pixels, img_w, channels, bx, by, bw, bh);

      int ri = (int)(b.r * 255.0f);
      int gi = (int)(b.g * 255.0f);
      int bi = (int)(b.b * 255.0f);

      float bright = powf(b.brightness, 0.75);
      int ramp_idx = (int)(bright * (RAMP_LEN - 1));

      // Only emit a new color escape if the color changed
      if (ri != prev_ri || gi != prev_gi || bi != prev_bi) {
        pos += sprintf(line + pos, "\x1b[38;2;%d;%d;%dm", ri, gi, bi);
        prev_ri = ri;
        prev_gi = gi;
        prev_bi = bi;
      }

      // Write the character twice (wide cell) directly into the buffer
      line[pos++] = RAMP[ramp_idx];
    }

    pos += sprintf(line + pos, ANSI_RESET "\n");
    fwrite(line, 1, pos, stdout);
  }

  free(line);
  stbi_image_free(pixels);

  set_raw_mode(1);
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  set_raw_mode(0);

  printf("\033[?25h");     // show cursor
  printf("\033[2J\033[H"); // clear screen
  fflush(stdout);

  return 0;
}

BlockInfo block_sample(unsigned char *pixels, int img_w, int channels, int bx, int by, int bw, int bh) {
  long sum_r = 0, sum_g = 0, sum_b = 0;
  int count = 0;
  for (int y = by; y < by + bh; y++) {
    for (int x = bx; x < bx + bw; x++) {
      int idx = (y * img_w + x) * channels;
      sum_r += pixels[idx + 0];
      sum_g += pixels[idx + 1];
      sum_b += pixels[idx + 2];
      count++;
    }
  }
  if (count == 0) { return (BlockInfo){0}; }
  BlockInfo info;
  info.r = (float)sum_r / count / 255.0f;
  info.g = (float)sum_g / count / 255.0f;
  info.b = (float)sum_b / count / 255.0f;
  // Perceptual luminance
  info.brightness = 0.2126f * info.r + 0.7152f * info.g + 0.0722f * info.b;
  info.brightness = powf(info.brightness, 0.6);
  return info;
}
