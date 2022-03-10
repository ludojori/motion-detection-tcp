stb_lib = ../libs/third-party/stb-image-write/
objects = server.o client.o 

all: server client

server: server.o
	g++ server.o camera_arm32.o -o ../apps/server

server.o: server.cpp
	g++ -c server.cpp -o server.o

client: client.o stb_image_write.o
	g++ client.o ${stb_lib}stb_image_write.o -o ../apps/client

client.o: client.cpp
	g++ -c client.cpp -o client.o

stb_image_write.o: stb_image_write.cpp
	g++ -c stb_image_write.cpp -o ${stb_lib}stb_image_write.o

clean:
	rm -f ${objects}
