CC = g++ -g -std=c++17
header = ./include/
lib = lib
main = main
LIBS = `pkg-config --cflags --libs gstreamer-1.0`

all: hlspipeline.o hls.so exe 

hlspipeline.o:	$(lib)/hlspipeline.cpp 
	$(CC) -c $(lib)/hlspipeline.cpp $(LIBS) -fPIC -I $(header)

hls.so:	hlspipeline.o
	$(CC) -shared -o libhls.so hlspipeline.o $(LIBS)

exe:	$(main)/main.cpp 
	$(CC) -o exe $(main)/main.cpp -lhls $(LIBS) -I $(header) -L .

run:	
	./exe

clean:
	rm -rf *.o *.so exe *.ts *.m3u8
