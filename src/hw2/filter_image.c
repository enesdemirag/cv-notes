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
            set_pixel(box_filter, x, y, 0, (1.0 / (w * w)));
        }
    }

    return box_filter;
}

image convolve_image(image im, image filter, int preserve) {
    // Co: number of channel in convolved image
    // imx, imy, imc: indexes of image pixels
    // fix, fiy: indexes of filter pixels

    assert(filter.c == im.c || filter.c == 1);
    int Co = preserve ? im.c : 1;
    
    image convolved_img = make_image(im.w, im.h, Co);
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
                        if(filter.c == 1) {
                            value += get_pixel(filter, fix, fiy, 0) * get_pixel(im, (imx - oxo + fix), (imy - xox + fiy), imc);
                        } else {
                            value += get_pixel(filter, fix, fiy, imc) * get_pixel(im, (imx - oxo + fix), (imy - xox + fiy), imc);
                        }
                    }
                }
                if(preserve) {
                    set_pixel(convolved_img, imx, imy, imc, value);
                } else {
                    set_pixel(convolved_img, imx, imy, 0, value);
                }
            }
        }
    }
    return convolved_img;
}

image make_highpass_filter() {
    image highpass_filter = make_image(3, 3, 1);
    set_pixel(highpass_filter, 0, 0, 0, 0);
    set_pixel(highpass_filter, 0, 1, 0, -1);
    set_pixel(highpass_filter, 0, 2, 0, 0);
    set_pixel(highpass_filter, 1, 0, 0, -1);
    set_pixel(highpass_filter, 1, 1, 0, 4);
    set_pixel(highpass_filter, 1, 2, 0, -1);
    set_pixel(highpass_filter, 2, 0, 0, 0);
    set_pixel(highpass_filter, 2, 1, 0, -1);
    set_pixel(highpass_filter, 2, 2, 0, 0);
    return highpass_filter;
}

image make_sharpen_filter() {
    image sharpen_filter = make_highpass_filter();
    set_pixel(sharpen_filter, 1, 1, 0, 5);
    return sharpen_filter;
}

image make_emboss_filter() {
    image emboss_filter = make_image(3, 3, 1);
    set_pixel(emboss_filter, 0, 0, 0, -2);
    set_pixel(emboss_filter, 0, 1, 0, -1);
    set_pixel(emboss_filter, 0, 2, 0, 0);
    set_pixel(emboss_filter, 1, 0, 0, -1);
    set_pixel(emboss_filter, 1, 1, 0, 1);
    set_pixel(emboss_filter, 1, 2, 0, 1);
    set_pixel(emboss_filter, 2, 0, 0, 0);
    set_pixel(emboss_filter, 2, 1, 0, 1);
    set_pixel(emboss_filter, 2, 2, 0, 2);
    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: We should use preserve in sharpen and emboss because the total sum of the kernel values is 1 and we expect colored outputs.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Yes. In highpass filter, there will be problems on edges. 

image make_gaussian_filter(float sigma) {
    int kernel_size = ceil(6.0 * sigma) == 6.0 * sigma ? ceil(6.0 * sigma) + 1 : ceil(6.0 * sigma);
    image gaussian_filter = make_image(kernel_size, kernel_size, 1);
    for(int x = 0; x < kernel_size; x++) {
        for(int y = 0; y < kernel_size; y++) {
            float value = (1.0 / (TWOPI * sigma * sigma)) * exp((x*x + y*y) / (-2 * sigma * sigma));
            set_pixel(gaussian_filter, x, y, 0, value);
        }
    }
    l1_normalize(gaussian_filter);
    return gaussian_filter;
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
