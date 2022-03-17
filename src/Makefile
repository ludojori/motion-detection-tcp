camera_dir = ../libs/third-party/camera/
stb_dir = ../libs/third-party/stb-image-write/
objects = client_libraries.o filename_builder.o ${stb_dir}stb_image_write.o

all: client_libraries filename_builder gpio_input server client

server: server.o
	g++ server.o ${camera_dir}camera_arm32.o -o server

server.o: server.cpp
	g++ -c server.cpp -o server.o

client: client.o stb_image_write.o
	g++ client.o ${stb_dir}stb_image_write.o -o client

client.o: client.cpp
	g++ -c client.cpp -o client.o

stb_image_write.o: stb_image_write.cpp
	g++ -c ../src/stb_image_write.cpp -o ${stb_dir}stb_image_write.o

clean:
	rm -f ${objects} client_libraries filename_builder gpio_input
