#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c) {
    if(x > im.w) {x = im.w;}
    else if(y > im.h) {y = im.h;}
    else if(c > im.c) {c = im.c;}
    else if(x < 0) {x = 0;}
    else if(y < 0) {y = 0;}
    else if(c < 0) {c = 0;}

    int index = (c * (im.h * im.w)) + (y * im.w) + x;      
    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v) {
    if(x > im.w || y > im.h || c > im.c || x < 0 || y < 0 || c < 0) {
        return;
    }
    else {
        int index = (c * (im.h * im.w)) + (y * im.w) + x;      
        im.data[index] = v;
    }
}

image copy_image(image im) {
    image copy = make_image(im.w, im.h, im.c);
    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < im.w; x++) {
            for(int y = 0; y < im.h; y++) {
                float value = get_pixel(im, x, y, c); 
                set_pixel(copy, x, y, c, value);
            }
        }
    }
    return copy;
}

image rgb_to_grayscale(image im) {
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            float r = get_pixel(im, x, y, 0); 
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);
            float value = (0.299 * r) + (0.587 * g) + (0.114 * b);
            set_pixel(gray, x, y, 0, value);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v) {
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            float value = get_pixel(im, x, y, c) + v;
            set_pixel(im, x, y, c, value);
        }
    }
}

void clamp_image(image im) {
    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x < im.w; x++) {
            for(int y = 0; y < im.h; y++) {
                float value = get_pixel(im, x, y, c);
                if(value > 1.0) {value = 1;}
                else if(value < 0.0) {value = 0;}
                set_pixel(im, x, y, c, value);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c) {
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c) {
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im) { // FIX: Probably not right :) results are terrible. 
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            // Get rgb values
            float r = get_pixel(im, x, y, 0);
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);
            
            // Difference
            float c = three_way_max(r, g, b) - three_way_min(r, g, b);

            // Value
            float v = three_way_max(r, g, b);
            
            // Saturation
            float s;
            if((r + g + b) == 0.0) {
                s = 0.0;
            }
            else {
                s = c / v;
            }

            // Hue
            float h;
            if(r == g && r == b) {h = 0.0;}
            else if(v == r) {h = ((g - b) / c) / 6;}
            else if(v == g) {h = (((b - r) / c) + 2) / 6;}
            else if(c == b) {h = (((r - g) / c) + 4) / 6;}
            fmod(h, 1.0);
            if(h < 0) {h++;}

            // Set hsv values
            set_pixel(im, x, y, 0, h);
            set_pixel(im, x, y, 1, s);
            set_pixel(im, x, y, 2, v);
        }
    }
}

void hsv_to_rgb(image im) { // FIX 
    // https://github.com/python/cpython/blob/2.7/Lib/colorsys.py
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            // Get hsv values
            float h = get_pixel(im, x, y, 0);
            float s = get_pixel(im, x, y, 1);
            float v = get_pixel(im, x, y, 2);

            float r;
            float g;
            float b;

            if(s == 0.0) {
                set_pixel(im, x, y, 0, 0.0);
                set_pixel(im, x, y, 1, 0.0);
                set_pixel(im, x, y, 2, 0.0);
                return;
            }

            int i = (int) (h * 6.0);
            float f = (h * 6.0) - i;
            float p = v * (1.0 - s);
            float q = v * (1.0 - s * f);
            float t = v * (1.0 - s * (1.0 - f));
            i = i % 6;
            if(i == 0) {
                set_pixel(im, x, y, 0, v);
                set_pixel(im, x, y, 1, t);
                set_pixel(im, x, y, 2, p);
                return;
            }
            else if(i == 1) {
                set_pixel(im, x, y, 0, q);
                set_pixel(im, x, y, 1, v);
                set_pixel(im, x, y, 2, p);
                return;
            }
            else if(i == 2) {
                set_pixel(im, x, y, 0, p);
                set_pixel(im, x, y, 1, v);
                set_pixel(im, x, y, 2, t);
                return;
            }
            else if(i == 3) {
                set_pixel(im, x, y, 0, p);
                set_pixel(im, x, y, 1, q);
                set_pixel(im, x, y, 2, v);
                return;
            }
            else if(i == 4) {
                set_pixel(im, x, y, 0, t);
                set_pixel(im, x, y, 1, p);
                set_pixel(im, x, y, 2, v);
                return;
            }
            else if(i == 5) {
                set_pixel(im, x, y, 0, v);
                set_pixel(im, x, y, 1, p);
                set_pixel(im, x, y, 2, q);
                return;
            }
        }
    }
}

void scale_image(image im, int c, float v) {
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            float value = get_pixel(im, x, y, c) * v;
            set_pixel(im, x, y, c, value);
        }
    }
}
