# OCR
 Optical Character Recognition


### Download and Installation 
* Download the file thank's to the github page. Or clone the project with the git link :
```shell
$ git clone https://github.com/DvDC13/OCR.git
```


* Run the script by using the Makefile :
At the makefile position
```shell
$ make clean
rm -f ocr_gui

$ make
cc ocr_gui.c -o ocr_gui -Wall -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
```

## Utilisation
You just need to run the script using this command after installation :

```shell
$ ./ocr_gui
```

### License
----
Developped by Stack Overflow group's
David Chemaly, Pierre Blaess, Pierre Crispi
Languages : C
Made for the Epita's 3rd semester project 
@Promo2024
