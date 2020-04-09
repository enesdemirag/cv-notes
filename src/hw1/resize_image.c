#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int nn_x = round(x);
    int nn_y = round(y);
    return get_pixel(im, nn_x, nn_y, c);
}

image nn_resize(image im, int w, int h)
{
    image new_img = make_image(w, h, im.c);
    float ration_x = w / im.w;
    float ration_y = h / im.h;

    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                int value = nn_interpolate(im, (x / ration_x), (y / ration_y), c);
                set_pixel(new_img, x, y, c, value);
            }
        }
    }
    return new_img;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int x1 = floor(x);
    int x2 = ceil(x);
    
    int y1 = floor(y);
    int y2 = ceil(y);
    
    float v11 = get_pixel(im, x1, y1, c);
    float v12 = get_pixel(im, x1, y2, c);
    float v21 = get_pixel(im, x2, y1, c);
    float v22 = get_pixel(im, x2, y2, c);

    float q1 = abs(x - x1) * v12 + abs(x - x2) * v11;
    float q2 = abs(x - x1) * v21 + abs(x - x2) * v22;
    float value = abs(y - y1) * q2 + abs(y - y2) * q1;

    return value;
}

image bilinear_resize(image im, int w, int h)
{
    image new_img = make_image(w, h, im.c);
    float ration_x = w / im.w;
    float ration_y = h / im.h;

    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                int value = bilinear_interpolate(im, (x / ration_x), (y / ration_y), c);
                set_pixel(new_img, x, y, c, value);
            }
        }
    }
    return new_img;
}

