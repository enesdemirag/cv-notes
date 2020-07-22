#include <stdlib.h>
#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c) {
    int nn_x = round(x);
    int nn_y = round(y);
    return get_pixel(im, nn_x, nn_y, c);
}

image nn_resize(image im, int w, int h) {
    image new_img = make_image(w, h, im.c);
    float ratio_x = (float)w / im.w;
    float ratio_y = (float)h / im.h;

    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                float value = nn_interpolate(im, (x / ratio_x), (y / ratio_y), c);
                set_pixel(new_img, x, y, c, value);
            }
        }
    }
    return new_img;
}

float bilinear_interpolate(image im, float x, float y, int c) {
    int x1 = (int)x;
    int x2 = (int)x + 1;
    int y1 = (int)y;
    int y2 = (int)y + 1;

    float v11 = get_pixel(im, x1, y1, c);
    float v12 = get_pixel(im, x2, y1, c);
    float v21 = get_pixel(im, x1, y2, c);
    float v22 = get_pixel(im, x2, y2, c);

    float d1 = x - x1;
    float d2 = x2 - x;
    float d3 = y - y1;
    float d4 = y2 - y;

    float q1 = d1 * v12 + d2 * v11;
    float q2 = d1 * v22 + d2 * v21;
    return d3 * q2 + d4 * q1;
}

image bilinear_resize(image im, int w, int h) {
    image new_img = make_image(w, h, im.c);
    float ratio_x = (float)w / im.w;
    float ratio_y = (float)h / im.h;

    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                float value = bilinear_interpolate(im, (x / ratio_x), (y / ratio_y), c);
                set_pixel(new_img, x, y, c, value);
            }
        }
    }
    return new_img;
}