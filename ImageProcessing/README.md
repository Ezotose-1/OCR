

```
$ apt install imagemagick
```


```
$ jpegtopnm < image.jpg > image.pnm
jpegtopnm: WRITING PPM FILE
$ ppmtobmp < image.pnm > image.bmp
ppmtobmp: analyzing colors...
ppmtobmp: More than 256 colors found
ppmtobmp: Writing 24 bits per pixel truecolor (no pallette)
$ ./test-sdl image.bmp 
Image loaded! :-)
$ ./test-sdl image.bmp 
```




getpixel() & setpixel(): https://sdl.beuc.net/sdl.wiki/Pixel_Access



jpegtopnm < image.jpg | ppmtobmp > image.bmp

