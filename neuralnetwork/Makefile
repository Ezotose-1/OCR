

CFLAGS += -Wall -Wextra
LDFLAGS += -lm

all: perceptron-one-hidden-layer


perceptron-one-hidden-layer: perceptron-one-hidden-layer.c
	gcc $(CFLAGS) -D_TRAINING_PHASE perceptron-one-hidden-layer.c -o perceptron-one-hidden-layer $(LDFLAGS) 


clean :
	rm -f perceptron-one-hidden-layer *.o
