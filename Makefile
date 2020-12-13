IMAGEPROCESSINGPATH = ImageProcessing/
GUIPATH = GUI/
NEURALPATH = neuralnetwork/



GTK_CFLAGS=$(shell pkg-config --cflags --libs gtk+-3.0)
CFLAGS+=-Wall $(GTK_CFLAGS) -g
	#ImageProcessing (SDL) flags
CFLAGS += $(shell sdl2-config --cflags)  -I$(NEURALPATH)
LDFLAGS += $(shell sdl2-config --libs) -lm
CFLAGS += -I include/




.PHONY: all

all: ocr_main

ocr_main: main.c ocr_gui.o imageProcessing.o perceptron-one-hidden-layer.o
	$(CC) $< -o $@ $(CFLAGS) ocr_gui.o imageProcessing.o perceptron-one-hidden-layer.o $(LDFLAGS) $(GTK_CFLAGS)


ocr_gui.o: $(GUIPATH)ocr_gui.c
	$(CC) -c $< $(CFLAGS)


imageProcessing.o: $(IMAGEPROCESSINGPATH)imageProcessing.c
	$(CC) -c $(CFLAGS) $< $(LDFLAGS)


perceptron-one-hidden-layer.o: $(NEURALPATH)perceptron-one-hidden-layer.c
	$(CC) -c $<

.PHONY: clean
clean:
	rm -f *.o


