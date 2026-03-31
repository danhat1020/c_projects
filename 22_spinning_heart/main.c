#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;

void handle_sigint(int sig) { running = 0; }

int main(void) {
  signal(SIGINT, handle_sigint);

  // clear screen
  printf("\x1b[2J");
  // hide cursor
  printf("\x1b[?25l");

  float t = 0;
  while (running) {
    float zb[100 * 40] = {0};
    float maxz = 0, c = cos(t), s = sin(t);

    for (float y = -0.5; y <= 0.5; y += 0.01) {
      // add beating effect
      float r = 0.4f + 0.05f * pow(0.5f + 0.5 * sin(t * 6 + y * 2), 8);
      for (float x = -0.5; x <= 0.5; x += 0.01) {
        // heart formula
        float z = -x * x - pow(1.2 * y - fabs(x) * 2 / 3, 2) + r * r;
        if (z < 0) { continue; }
        z = sqrt(z) / (2 - y);
        for (float tz = -z; tz <= z; tz += z / 6) {
          // rotate
          float nx = x * c - tz * s;
          float nz = x * s + tz * c;

          // add perspective
          float p = 1 + nz / 2;
          int vx = lroundf((nx * p + 0.5) * 80 + 10);
          int vy = lroundf((-y * p + 0.5) * 39 + 2);
          int idx = vx + vy * 100;
          if (zb[idx] <= nz) {
            zb[idx] = nz;
            if (maxz <= nz) { maxz = nz; }
          }
        }
      }
    }

    // move cursor to 0, 0
    printf("\x1b[H");
    for (int i = 0; i < 100 * 40; i++) {
      putchar(i % 100 ? " .,:-*o+=0&#$@"[lroundf(zb[i] / maxz * 13)] : 10);
    }

    t += 0.03;
    usleep(30000);
  }

  // show cursor
  printf("\x1b[?25h\n");

  return 0;
}
