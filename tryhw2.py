from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

blur_without_preserve = convolve_image(im, f, 0)
save_image(blur_without_preserve, "dog-box7-0")


thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb-nn")

thumb = bilinear_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb-bilinear")

f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")

f = make_gaussian_filter(3)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss3")
