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

    int oxo = filter.w / 2;
    int xox = filter.h / 2;
    
    // Convolution Loop
    for(int imc = 0; imc < im.c; imc++) {
        for(int imx = 0; imx < im.w; imx++) {
            for(int imy = 0; imy < im.h; imy++) {
                value = 0; // get_pixel(convolved_img, imx, imy, imc);
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
    clamp_image(convolved_img);
    return convolved_img;
}

image make_highpass_filter() {
    image highpass_filter = make_image(3, 3, 1);
    set_pixel(highpass_filter, 0, 1, 0, -1);
    set_pixel(highpass_filter, 1, 0, 0, -1);
    set_pixel(highpass_filter, 1, 1, 0, 4);
    set_pixel(highpass_filter, 1, 2, 0, -1);
    set_pixel(highpass_filter, 2, 1, 0, -1);
    return highpass_filter;
}

image make_sharpen_filter() {
    image sharpen_filter = make_image(3, 3, 1);
    set_pixel(sharpen_filter, 0, 1, 0, -1);
    set_pixel(sharpen_filter, 1, 0, 0, -1);
    set_pixel(sharpen_filter, 1, 1, 0, 5);
    set_pixel(sharpen_filter, 1, 2, 0, -1);
    set_pixel(sharpen_filter, 2, 1, 0, -1);
    return sharpen_filter;
}

image make_emboss_filter() {
    image emboss_filter = make_image(3, 3, 1);
    set_pixel(emboss_filter, 0, 0, 0, -2);
    set_pixel(emboss_filter, 0, 1, 0, -1);
    set_pixel(emboss_filter, 1, 0, 0, -1);
    set_pixel(emboss_filter, 1, 1, 0, 1);
    set_pixel(emboss_filter, 1, 2, 0, 1);
    set_pixel(emboss_filter, 2, 1, 0, 1);
    set_pixel(emboss_filter, 2, 2, 0, 2);
    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: We should use preserve in sharpen and emboss because the total sum of the kernel values is 1 and we expect colored outputs.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Yes. In highpass filter, there will be problems on edges. 

float compute_gaussian(int x, int y, float sigma) {
    return (1.0 / (TWOPI * sigma * sigma)) * exp(-(x*x + y*y) / (2 * sigma * sigma));
}

image make_gaussian_filter(float sigma) {
    int kernel_size = ceil(6.0 * sigma) == 6.0 * sigma ? ceil(6.0 * sigma) + 1 : ceil(6.0 * sigma);
    image gaussian_filter = make_image(kernel_size, kernel_size, 1);
    for(int x = 0; x < kernel_size; x++) {
        for(int y = 0; y < kernel_size; y++) {
            float value = compute_gaussian(kernel_size / 2 - x, kernel_size / 2 - y, sigma);
            set_pixel(gaussian_filter, x, y, 0, value);
        }
    }
    l1_normalize(gaussian_filter);
    return gaussian_filter;
}

image add_image(image a, image b) {
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image new_image = make_image(a.w, a.h, a.c);
    for(int c = 0; c < a.c; c++) {
        for(int x = 0; x < a.w; x++) {
            for(int y = 0; y < a.h; y++) {
                float v = get_pixel(a, x, y, c) + get_pixel(b, x, y, c);
                set_pixel(new_image, x, y, c, v);
            }
        }
    }
    // clamp_image(new_image);
    return new_image;
}

image sub_image(image a, image b) {
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image new_image = make_image(a.w, a.h, a.c);
    for(int c = 0; c < a.c; c++) {
        for(int x = 0; x < a.w; x++) {
            for(int y = 0; y < a.h; y++) {
                float v = get_pixel(a, x, y, c) - get_pixel(b, x, y, c);
                set_pixel(new_image, x, y, c, v);
            }
        }
    }
    // clamp_image(new_image);
    return new_image;
}

image make_gx_filter() {
    image gx = make_image(3, 3, 1);
    set_pixel(gx, 0, 0, 0, -1);
    set_pixel(gx, 2, 0, 0, 1);
    set_pixel(gx, 0, 1, 0, -2);
    set_pixel(gx, 2, 1, 0, 2);
    set_pixel(gx, 0, 2, 0, -1);
    set_pixel(gx, 2, 2, 0, 1);
    return gx;
}

image make_gy_filter() {
    image gy = make_image(3, 3, 1);
    set_pixel(gy, 0, 0, 0, -1);
    set_pixel(gy, 0, 2, 0, 1);
    set_pixel(gy, 1, 0, 0, -2);
    set_pixel(gy, 1, 2, 0, 2);
    set_pixel(gy, 2, 0, 0, -1);
    set_pixel(gy, 2, 2, 0, 1);
    return gy;
}

void feature_normalize(image im) {
    // Find min and max pixel values
    float min = +INFINITY;
    float max = -INFINITY;
    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < im.w; x++) {
            for(int y = 0; y < im.h; y++) {
                min = fmin(min, get_pixel(im, x, y, c));
                max = fmax(max, get_pixel(im, x, y, c));
            }
        }
    }

    // Rescaling
    float v = 0;
    float range = max - min;
    // assert(range == 0); // Cannot divide to zero 
    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < im.w; x++) {
            for(int y = 0; y < im.h; y++) {
                v = (get_pixel(im, x, y, c) - min) / range;
                set_pixel(im, x, y, c, v);
            }
        }
    }
}

image *sobel_image(image im) {
    image *mag_dir = calloc(2, sizeof(image)); // Magnitude and Direction
    
    mag_dir[0] = make_image(im.w, im.h, 1);
    mag_dir[1] = make_image(im.w, im.h, 1);
    
    image Gx = convolve_image(im, make_gx_filter(), 0);
    image Gy = convolve_image(im, make_gy_filter(), 0);
    
    float px, py = 0;
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            px = get_pixel(Gx, x, y, 0);
            py = get_pixel(Gy, x, y, 0);

            set_pixel(mag_dir[0], x, y, 0, sqrt(px * px + py * py));
            set_pixel(mag_dir[1], x, y, 0, atan2(py, px));
        }
    }

    free_image(Gx);
    free_image(Gy);
    
    return mag_dir;
}

image colorize_sobel(image im) {
    // TODO
    return make_image(1,1,1);
}
