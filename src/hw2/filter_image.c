#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im) {
    float sum = 0;

    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < im.w; x++) {
            for(int y = 0; y < im.h; y++) {
                sum += get_pixel(im, x, y, c);
            }
        }
    }

    for(int c = 0; c < im.c; c++) {
        scale_image(im, c, (1 / sum));
    }
}

image make_box_filter(int w) {
    image box_filter = make_image(w, w, 1);

    for(int x = 0; x < w; x++) {
        for(int y = 0; y < w; y++) {
            set_pixel(box_filter, x, y, 0, (1 / (w * w)));
        }
    }

    return box_filter;
}

image convolve_image(image im, image filter, int preserve) {
    // Co: number of channel in convolved image
    // imx, imy, imc: indexes of image pixels
    // fix, fiy: indexes of filter pixels
    // rx, ry: relative indexes of pixels

    // assert(filter.c == im.c || filter.c == 1);
    // int Co = preserve ? im.c : 1;
    // image convolved_image = make_image(im.w, im.h, Co);
    
    image convolved_img = make_image(im.w, im.h, im.c);
    float value = 0;
    int oxo = floor(filter.w / 2);
    int xox = floor(filter.h / 2);
    // Convolution Loop
    for(int imc = 0; imc < im.c; imc++) {
        for(int imx = 0; imx < im.w; imx++) {
            for(int imy = 0; imy < im.h; imy++) {
                value = 0;
                for(int fix = 0; fix < filter.w; fix++) {
                    for(int fiy = 0; fiy < filter.h; fiy++) {
                        value += get_pixel(filter, fix, fiy, 0) * get_pixel(im, (imx - oxo + fix), (imy - xox + fiy), imc);
                    }
                }
                set_pixel(convolved_img, imx, imy, imc, value);
            }
        }
    }
    return convolved_img;
}

image make_highpass_filter() {
    // TODO
    return make_image(1,1,1);
}

image make_sharpen_filter() {
    // TODO
    return make_image(1,1,1);
}

image make_emboss_filter() {
    // TODO
    return make_image(1,1,1);
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma) {
    // TODO
    return make_image(1,1,1);
}

image add_image(image a, image b) {
    // TODO
    return make_image(1,1,1);
}

image sub_image(image a, image b) {
    // TODO
    return make_image(1,1,1);
}

image make_gx_filter() {
    // TODO
    return make_image(1,1,1);
}

image make_gy_filter() {
    // TODO
    return make_image(1,1,1);
}

void feature_normalize(image im) {
    // TODO
}

image *sobel_image(image im) {
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im) {
    // TODO
    return make_image(1,1,1);
}
