#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char ch;
    float ratio;
} CharRatio;

int compare(const void *a, const void *b) {
    float diff = ((CharRatio *)a)->ratio - ((CharRatio *)b)->ratio;
    return (diff > 0) - (diff < 0);
}

int main(void) {
    const char *characters = " !@#$%^&*()-=_+[]{};':\"\\|,./<>?0O";
    FILE *f = fopen("CascadiaCode-Bold.ttf", "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *font_buf = malloc(size);
    fread(font_buf, 1, size, f);
    fclose(f);

    stbtt_fontinfo font;
    stbtt_InitFont(&font, font_buf, 0);
    float scale = stbtt_ScaleForPixelHeight(&font, 16);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int cell_h = (int)((ascent - descent) * scale);

    int n = strlen(characters);
    CharRatio *results = malloc(n * sizeof(CharRatio));

    int w, h, xoff, yoff;
    for (int i = 0; i < n; i++) {
        char ch = characters[i];
        unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, ch, &w, &h, &xoff, &yoff);

        float sum = 0.0f;
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                sum += bitmap[y * w + x] / 255.0f;

        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, ch, &advance, &lsb);
        int cell_w = (int)(advance * scale);

        results[i].ch = ch;
        results[i].ratio = (cell_w * cell_h) > 0 ? sum / (cell_w * cell_h) : 0;

        stbtt_FreeBitmap(bitmap, NULL);
    }

    qsort(results, n, sizeof(CharRatio), compare);

    for (int i = 0; i < n; i++)
        printf("%c: %f\n", results[i].ch, results[i].ratio);

    printf("\nRAMP: \"");
    for (int i = 0; i < n; i++)
        printf("%c", results[i].ch);
    printf("\"\n");

    free(results);
    free(font_buf);
    return 0;
}
