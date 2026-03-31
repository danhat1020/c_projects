#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <youtube_url> [output_dir] [filename]\n", argv[0]);
    return 1;
  }

  const char *url = argv[1];
  const char *out_dir = argc >= 3 ? argv[2] : ".";
  const char *filename = argc >= 4 ? argv[3] : "%(title)s";

  // create output directory if it doesn't exist
  char mkdir_cmd[1024];
  snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p \"%s\"", out_dir);
  system(mkdir_cmd);

  // build yt-dlp command
  char cmd[1024];
  snprintf(cmd, sizeof(cmd),
           // yt-dlp -x --audio-format wav -o "out_dir/filename.wav" "https://youtube.com"
           "yt-dlp -x --audio-format wav -o \"%s/%s.%%(ext)s\" \"%s\"",
           out_dir, filename, url);

  printf("Running: %s\n", cmd);
  int ret = system(cmd);
  if (ret != 0) {
    printf("yt-dlp failed with code %d\n", ret);
    return 1;
  }

  printf("Done.\n");
  return 0;
}
