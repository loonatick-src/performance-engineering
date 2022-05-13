#ifndef INPUT_H
#define INPUT_H

#include <unistd.h>
#include <stdlib.h>

const int default_thread_count = 16;
const int default_image_width = 600;
const int default_image_height = 600;
const int default_samples_per_pixel = 100;
const int default_max_depth = 50;


typedef struct {
    int thread_count;
    int image_width;
    int image_height;
    int samples_per_pixel;
    int max_depth;
} input_parameters;

input_parameters create_default_parameters() {
    return input_parameters {
        default_thread_count,
        default_image_width,
        default_image_height,
        default_samples_per_pixel,
        default_max_depth
    };
}

input_parameters read_input(int argc, char *argv[]) {
    auto params = create_default_parameters();
    char ch;
    while ((ch = getopt(argc, argv, "t:w:h:d:s:")) != -1 ) {
        switch (ch) {
        case('t'):
            params.thread_count = (int) strtol(optarg, 0, 10);
            break;
        case('w'):
            params.image_width = (int) strtol(optarg, 0, 10);
            break;
        case('h'):
            params.image_height = (int) strtol(optarg, 0, 10);
            break;
        case('d'):
            params.max_depth = (int) strtol(optarg, 0, 10);
            break;
        case('s'):
            params.samples_per_pixel = (int) strtol(optarg, 0, 10);
            break;
        }
    }

    return params;
}
#endif
