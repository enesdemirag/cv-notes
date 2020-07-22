#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c) {
    if(x >= im.w) {x = im.w - 1;}
    else if(x < 0) {x = 0;}
    
    if(y >= im.h) {y = im.h - 1;}
    else if(y < 0) {y = 0;}
    
    if(c >= im.c) {c = im.c - 1;}
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

void rgb_to_hsv(image im) {
    float h, s, v, min, max, hue_prime, r, g, b;
    
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            // Get rgb values
            r = get_pixel(im, x, y, 0);
            g = get_pixel(im, x, y, 1);
            b = get_pixel(im, x, y, 2);
            
            max = three_way_max(r,g,b);
            min = three_way_min(r,g,b);
            v = max;
            float C = v - min;
            s = (C == 0) ? 0 : ((v == 0) ? 0 : C / v);
            hue_prime = (C == 0) ? 0 : ((v == r) ? (g - b) / C : ((v == g) ? ((b - r) / C) + 2.0 : ((r - g) / C) + 4.0));
            h = (hue_prime < 0) ? (hue_prime / 6.0) + 1 : hue_prime / 6.0;
            
            // Set hsv values
            set_pixel(im, x, y, 0, h);
            set_pixel(im, x, y, 1, s);
            set_pixel(im, x, y, 2, v);
        }
    }
}

// https://github.com/viplix3/The-Ancient-Secrets-of-Computer-Vision-Assignments/
void hsv_to_rgb(image im) {
	float r, g, b, h, s, v, hue_prime, C, max, min;

    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            // Get hsv values
            h = get_pixel(im, x, y, 0);
            s = get_pixel(im, x, y, 1);
            v = get_pixel(im, x, y, 2);

            max = v;
            C = s * v;
            min = max - C;
            hue_prime = h * 6.;
            float X = (1 - fabs(fmod(hue_prime, 2) - 1));

            // In this part of HSV hexagon, R component will have the maximum value, B will have the minimum, and G will have value somehwere between R and B
            if((hue_prime >= 0.0) && (hue_prime < 1.0)) {
                r = max;
                b = min;
                g = (C * X) + b;
            }
            // In this part of HSV hexagon, G component will have the maximum value, B will have the minimum, and R will have value somehwere between G and B
            else if((hue_prime >= 1.0) && (hue_prime < 2.0)) {
                g = max;
                b = min;
                r = (C * X) + b;
            }
            else if((hue_prime >= 2.0) && (hue_prime < 3.0)) {
                g = max;
                r = min;
                b = (C * X) + r;
            }
            else if((hue_prime >= 3.0) && (hue_prime < 4.0)) {
                b = max;
                r = min;
                g = (C * X) + r;
            }
            else if((hue_prime >= 4.0) && (hue_prime < 5.0)) {
                b = max;
                g = min;
                r = (C * X) + g;
            }
            else if((hue_prime >= 5.0) && (hue_prime < 6.0)) {
                r = max;
                g = min;
                b = (C * X) + g;
            }
            else {
                r = 0;
                g = 0;
                b = 0;
            }
            // Set rgb values
            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);
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
