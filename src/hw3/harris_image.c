#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n) {
    int i;
    for(i = 0; i < n; ++i){
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i) {
    int w = 5;
    descriptor d;
    d.p.x = i%im.w;
    d.p.y = i/im.w;
    d.data = calloc(w*w*im.c, sizeof(float));
    d.n = w*w*im.c;
    int c, dx, dy;
    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(dx = -w/2; dx < (w+1)/2; ++dx){
            for(dy = -w/2; dy < (w+1)/2; ++dy){
                float val = get_pixel(im, i%im.w+dx, i/im.w+dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p) {
    int x = p.x;
    int y = p.y;
    int i;
    for(i = -9; i < 10; ++i){
        set_pixel(im, x+i, y, 0, 1);
        set_pixel(im, x, y+i, 0, 1);
        set_pixel(im, x+i, y, 1, 0);
        set_pixel(im, x, y+i, 1, 0);
        set_pixel(im, x+i, y, 2, 1);
        set_pixel(im, x, y+i, 2, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n) {
    int i;
    for(i = 0; i < n; ++i){
        mark_spot(im, d[i].p);
    }
}

float compute_1d_gaussian(int x, float sigma) {
    float gaussian = (1 / (TWOPI * pow(sigma, 2))) * (exp(-(pow(x, 2)) / (2 * pow(sigma, 2))));
    return gaussian;
}

image transpose_image(image im) {
    image transposed_im = make_image(im.h, im.w, im.c);

    for(int c = 0; c < im.c; c++) {
        for(int x = 0; x<im.w; x++) {
            for(int y = 0; y<im.h; y++) {
                set_pixel(transposed_im, x, y, c, get_pixel(im, y, x, c));
            }
        }
    }

    return transposed_im;
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma) {
    int kernel_size = ceil(6.0 * sigma) == 6.0 * sigma ? ceil(6.0 * sigma) + 1 : ceil(6.0 * sigma);
    image gaussian_filter = make_image(kernel_size, 1, 1);
    for(int x = 0; x < kernel_size; x++) {
        float value = compute_1d_gaussian(kernel_size / 2 - x, sigma);
        set_pixel(gaussian_filter, x, 0, 0, value);
    }
    l1_normalize(gaussian_filter);
    return gaussian_filter;
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma) {
    // image g = make_gaussian_filter(sigma);
    // image s = convolve_image(im, g, 1);
    // free_image(g);
    // return s;

    image g1_x = make_1d_gaussian(sigma);
    image gx_1 = transpose_image(g1_x);
    image s = convolve_image(convolve_image(im, g1_x, 1), gx_1, 1);

    free_image(g1_x);
    free_image(gx_1);

    return s;
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma) {
    image S = make_image(im.w, im.h, 3);
    
    image Ix = convolve_image(im, make_gx_filter(), 0);
    image Iy = convolve_image(im, make_gy_filter(), 0);

    // | ΣiwiIx(i)Ix(i)    ΣiwiIx(i)Iy(i) |
    // | ΣiwiIx(i)Iy(i)    ΣiwiIy(i)Iy(i) |

    float IxIx, IyIy, IxIy = 0;
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            IxIx = pow(get_pixel(Ix, x, y, 0), 2);
            IyIy = pow(get_pixel(Iy, x, y, 0), 2);
            IxIy = get_pixel(Ix, x, y, 0) * get_pixel(Iy, x, y, 0);

            set_pixel(S, x, y, 0, IxIx);
            set_pixel(S, x, y, 1, IyIy);
            set_pixel(S, x, y, 2, IxIy);
        }
    }
    S = smooth_image(S, sigma);

    free_image(Ix);
    free_image(Iy);

    return S;
}

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
image cornerness_response(image S) {
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    image R = make_image(S.w, S.h, 1);
    float IxIx, IyIy, IxIy, det, trace, cornerness = 0;
    float alpha = 0.06;
    for(int x = 0; x < S.w; x++) {
        for(int y = 0; y < S.h; y++) {
            // det(S) = λ1*λ2
            // trace(S) = λ1+λ2
            // det(S) - α trace(S)2 = λ1*λ2 - α(λ1+λ2)2
            IxIx = get_pixel(S, x, y, 0);
            IyIy = get_pixel(S, x, y, 1);
            IxIy = get_pixel(S, x, y, 2);
            det = (IxIx * IyIy) - (IxIy * IxIy);
            trace = IxIx + IyIy;
            cornerness = det - alpha * pow(trace, 2);
            set_pixel(R, x, y, 0, cornerness);
        }
    }
    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w) {
    image r = copy_image(im);
    // TODO: perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    float pixel, neighbor;
    for(int c = 0; c < r.c; c++) {
        for(int x = 0; x < r.w; x++) {
            for(int y = 0; y < r.h; y++) {
                pixel = get_pixel(r, x, y, c);
                for(int i = -w; i < w; i++) {
                    for(int j = -w; j < w; j++) {
                        neighbor = get_pixel(r, x - i, y - j, c);
                        if(neighbor >= pixel) {
                            set_pixel(r, x, y, c, 0);
                        }
                    }
                }
            }
        }
    }
    return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n) {
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);

    int count = 0;
    for(int x = 0; x < Rnms.w; x++) {
        for(int y = 0; y < Rnms.h; y++) {
            if(get_pixel(Rnms, x, y, 0) > thresh) {count++;}
        }
    }
    
    *n = count; // <- set *n equal to number of corners in image.
    descriptor *d = calloc(count, sizeof(descriptor));
    // TODO: fill in array *d with descriptors of corners, use describe_index.
    for(int i = 0; i < Rnms.h * Rnms.w; i++) {
        if(Rnms.data[i] > thresh) {
            *d++ = describe_index(im, i);
        }
    }

    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms) {
    int n = 0;
    descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
