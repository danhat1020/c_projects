#include <fftw3.h>
#include <libgen.h>
#include <math.h>
#include <random.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void build_output_path(const char *input, const char *out_dir, char *out, int out_len) {
    // get a mutable copy since basename/dirname may modify the string
    char input_copy_a[1024], input_copy_b[1024];
    strncpy(input_copy_a, input, sizeof(input_copy_a) - 1);
    strncpy(input_copy_b, input, sizeof(input_copy_b) - 1);

    char *base = basename(input_copy_a);
    char *in_dir = dirname(input_copy_b);

    // strip extension
    char stem[1024];
    strncpy(stem, base, sizeof(stem) - 1);
    char *dot = strrchr(stem, '.');
    char ext[32] = "";
    if (dot) {
        strncpy(ext, dot, sizeof(ext) - 1); // e.g. ".wav"
        dot[0] = '\0';                      // stem is now "song"
    }

    // decide output dir
    const char *dir = out_dir ? out_dir : in_dir;

    // append "_out" if output dir matches input dir, or no dir was given
    int same_dir = (out_dir == NULL) || (strcmp(out_dir, in_dir) == 0);
    const char *suffix = same_dir ? "_out" : "";

    snprintf(out, out_len, "%s/%s%s%s", dir, stem, suffix, ext);
}

float spectral_flatness(float *mags, int bins) {
    float log_sum = 0.0f, sum = 0.0f;

    for (int i = 0; i < bins; i++) {
        float m = mags[i] + 1e-10f;
        log_sum += logf(m);
        sum += m;
    }
    float geometric = expf(log_sum / bins);
    float arithmetic = sum / bins;
    return geometric / arithmetic; // 0 = pitchy, 1 = noisy
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s input.wav [output_dir]\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *out_dir = argc >= 3 ? argv[2] : NULL;

    char output_path[1024];
    build_output_path(input_path, out_dir, output_path, sizeof(output_path));
    printf("Output: %s\n", output_path);

    SF_INFO info = {0};
    SNDFILE *in_file = sf_open(input_path, SFM_READ, &info);
    if (!in_file) {
        printf("Failed to open input file: %s\n", sf_strerror(NULL));
        return 1;
    }

    // read all samples
    int total = info.frames * info.channels;
    float *audio = malloc(sizeof(float) * total);
    if (!audio) {
        sf_close(in_file);
        return 1;
    }
    sf_read_float(in_file, audio, total);
    sf_close(in_file);

    int frames = info.frames;
    float *left = malloc(sizeof(float) * frames);
    float *right = malloc(sizeof(float) * frames);
    if (!left || !right) {
        free(audio);
        return 1;
    }

    for (int i = 0; i < frames; i++) {
        left[i] = audio[i * 2];
        right[i] = audio[i * 2 + 1];
    }
    free(audio);

    // FFT setup
    int frame_size = info.samplerate / 10; // ~200ms
    int bins = frame_size / 2 + 1;
    int num_frames = frames / frame_size;

    float *fft_in = fftwf_malloc(sizeof(float) * frame_size);
    fftwf_complex *fft_out = fftwf_malloc(sizeof(fftwf_complex) * bins);

    fftwf_plan fwd = fftwf_plan_dft_r2c_1d(frame_size, fft_in, fft_out, FFTW_ESTIMATE);
    fftwf_plan inv = fftwf_plan_dft_c2r_1d(frame_size, fft_out, fft_in, FFTW_ESTIMATE);

    // accumulate magnitudes across all frames
    float *avg_mags_left = calloc(bins, sizeof(float));
    float *avg_mags_right = calloc(bins, sizeof(float));
    if (!avg_mags_left || !avg_mags_right) {
        return 1;
    }

    float *frame_mags = malloc(sizeof(float) * bins);
    if (!frame_mags) {
        return 1;
    }

    float total_weight_left = 0.0f, total_weight_right = 0.0f;

    for (int f = 0; f < num_frames; f++) {
        if (f % 100 == 0) {
            printf("Processing frame %d / %d\r", f, num_frames);
        }

        // left
        memcpy(fft_in, &left[f * frame_size], sizeof(float) * frame_size);
        fftwf_execute(fwd);
        for (int i = 0; i < bins; i++) {
            float real = fft_out[i][0];
            float imag = fft_out[i][1];
            frame_mags[i] = sqrtf(real * real + imag * imag);
        }
        float weight_left = 1.0f - spectral_flatness(frame_mags, bins);
        total_weight_left += weight_left;
        for (int i = 0; i < bins; i++) {
            avg_mags_left[i] += frame_mags[i] * weight_left;
        }

        // right
        memcpy(fft_in, &right[f * frame_size], sizeof(float) * frame_size);
        fftwf_execute(fwd);
        for (int i = 0; i < bins; i++) {
            float real = fft_out[i][0];
            float imag = fft_out[i][1];
            frame_mags[i] = sqrtf(real * real + imag * imag);
        }
        float weight_right = 1.0f - spectral_flatness(frame_mags, bins);
        total_weight_right += weight_right;
        for (int i = 0; i < bins; i++) {
            avg_mags_right[i] += frame_mags[i] * weight_right;
        }
    }
    printf("\n");

    free(frame_mags);

    // divide by total weight to get weighted average
    for (int i = 0; i < bins; i++) {
        avg_mags_left[i] /= total_weight_left;
        avg_mags_right[i] /= total_weight_right;
    }

    // threshold pass
    float threshold = 0.3f;
    float dampen = 0.05f;
    float amplify = 2.0f;

    for (int i = 0; i < bins; i++) {
        if (avg_mags_left[i] < threshold) {
            avg_mags_left[i] *= dampen;
        } else {
            avg_mags_left[i] *= amplify;
        }
        if (avg_mags_right[i] < threshold) {
            avg_mags_right[i] *= dampen;
        } else {
            avg_mags_right[i] *= amplify;
        }
    }

    // Hann window to smooth frame edges
    float *window = malloc(sizeof(float) * frame_size);
    if (!window)
        return 1;
    for (int i = 0; i < frame_size; i++)
        window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (frame_size - 1)));

    int hop = frame_size * (1.0f - 0.75f);
    int out_secs = 15;
    int out_frames = info.samplerate * out_secs;

    float *left_out = calloc(out_frames, sizeof(float));
    float *right_out = calloc(out_frames, sizeof(float));
    if (!left_out || !right_out)
        return 1;

    int num_hops = out_frames / hop;

    for (int h = 0; h < num_hops; h++) {
        int offset = h * hop;

        // left
        for (int i = 0; i < bins; i++) {
            float phase = rng_range(0.0f, 2.0f * M_PI);
            fft_out[i][0] = avg_mags_left[i] * cosf(phase);
            fft_out[i][1] = avg_mags_left[i] * sinf(phase);
        }
        fftwf_execute(inv);
        for (int i = 0; i < frame_size && (offset + i) < out_frames; i++)
            left_out[offset + i] += (fft_in[i] / frame_size) * window[i];

        // right
        for (int i = 0; i < bins; i++) {
            float phase = rng_range(0.0f, 2.0f * M_PI);
            fft_out[i][0] = avg_mags_right[i] * cosf(phase);
            fft_out[i][1] = avg_mags_right[i] * sinf(phase);
        }
        fftwf_execute(inv);
        for (int i = 0; i < frame_size && (offset + i) < out_frames; i++)
            right_out[offset + i] += (fft_in[i] / frame_size) * window[i];
    }

    free(window);

    // normalize
    float peak = 0.0f;
    for (int i = 0; i < out_frames; i++) {
        if (fabsf(left_out[i]) > peak)
            peak = fabsf(left_out[i]);
        if (fabsf(right_out[i]) > peak)
            peak = fabsf(right_out[i]);
    }
    if (peak > 0.0f) {
        float scale = 0.9f / peak;
        for (int i = 0; i < out_frames; i++) {
            left_out[i] *= scale;
            right_out[i] *= scale;
        }
    }

    // interleave and write output
    float *interleaved = malloc(sizeof(float) * out_frames * 2);
    if (!interleaved) {
        free(left), free(right), free(left_out), free(right_out);
        return 1;
    }
    for (int i = 0; i < out_frames; i++) {
        interleaved[i * 2] = left_out[i];
        interleaved[i * 2 + 1] = right_out[i];
    }

    info.frames = out_frames;
    SNDFILE *out_file = sf_open(output_path, SFM_WRITE, &info);
    if (!out_file) {
        printf("Failed to open output file: %s\n", sf_strerror(NULL));
        free(left), free(right), free(left_out), free(right_out), free(interleaved);
        return 1;
    }
    sf_write_float(out_file, interleaved, out_frames * 2);
    sf_close(out_file);

    fftwf_destroy_plan(fwd);
    fftwf_destroy_plan(inv);
    fftwf_free(fft_in);
    fftwf_free(fft_out);
    free(left), free(right), free(left_out), free(right_out), free(interleaved);

    printf("Done.\n");

    return 0;
}
