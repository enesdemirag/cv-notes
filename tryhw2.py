from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
blur_without_preserve = convolve_image(im, f, 0)
save_image(blur, "dog-box7")
save_image(blur_without_preserve, "dog-box7-0")