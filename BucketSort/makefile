eCFLAGS=-Wall
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm

qs_para: clean
	gcc $(CFLAGS) $(INCLUDES) -o bsort bs_para.c ./graphics/graphics.c  $(LDFLAGS) -Xpreprocessor -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include

qs_para_priv: clean
	gcc $(CFLAGS) $(INCLUDES) -o bsort bs_para_priv.c ./graphics/graphics.c  $(LDFLAGS) -Xpreprocessor -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include
	
clean:
	rm -f ./bsort *.o

#FILENAME // N_ELEMS // N_BUCKETS // nTHREADS // isGRAPHIC
test_c: qs_para
	./bsort 10000 10 16 1
	
test_p: qs_para_priv
	./bsort 10000 10 16 1
