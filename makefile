eCFLAGS=-Wall
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm

qs_para: clean
	gcc $(CFLAGS) $(INCLUDES) -o qsort qs_para.c ./graphics/graphics.c  $(LDFLAGS) -Xpreprocessor -fopenmp -lomp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include

clean:
	rm -f ./qsort *.o

#FILENAME // COL_TO_READ // ORDER // nTHREADS // isGRAPHIC
test: qs_para
	./qsort "cities_over_1000_population.csv" 1 1 4 1
