from uwimg import *
im = load_image("data/dog.jpg")

f = make_box_filter(7)
box7 = convolve_image(im, f, 1)
save_image(box7, "dog-box7")

box_without_preserve = convolve_image(im, f, 0)
save_image(box_without_preserve, "dog-box7-0")

f = make_box_filter(13)
bigbox = convolve_image(im, f, 1)
save_image(bigbox, "dog-box13")

f = make_highpass_filter()
high = convolve_image(im, f, 0)
save_image(high, "dog-highpass")

f = make_sharpen_filter()
sharp = convolve_image(im, f, 1)
save_image(sharp, "dog-sharp")

f = make_emboss_filter()
emboss = convolve_image(im, f, 1)
save_image(emboss, "dog-emboss")

f = make_gaussian_filter(2)
gauss2 = convolve_image(im, f, 1)
save_image(gauss2, "dog-gauss2")

f = make_gaussian_filter(3)
gauss3 = convolve_image(im, f, 1)
save_image(gauss3, "dog-gauss3")