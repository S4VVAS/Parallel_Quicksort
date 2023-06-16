eCFLAGS=-Wall
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm

bs_para: clean
	gcc $(CFLAGS) $(INCLUDES) -o bsort bs_para.c -O ./graphics/graphics.c  $(LDFLAGS) -Xpreprocessor -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include
	
clean:
	rm -f ./bsort *.o

#FILENAME // N_ELEMS // N_BUCKETS // nTHREADS // isGRAPHIC
test: bs_para
	./bsort 100000 0 8 0

