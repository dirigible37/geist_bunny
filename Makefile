CPP=g++
CFLAGS= -L/usr/lib6 -O2 -lX11 -lGL -lGLU -lGLEW -lglut -lm -lXmu

BINS = main
all: $(BINS)

main: main.cpp
	$(CPP) $(CFLAGS) -o main main.cpp
	
clean:
	rm $(BINS)
