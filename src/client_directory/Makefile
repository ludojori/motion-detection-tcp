camera_dir = ../libs/third-party/camera/
stb_dir = ../libs/third-party/stb-image-write/
objects = jpg-receive-save-utils.o ${stb_dir}stb_image_write.o

all: server client

server: server.o
	g++ server.o ${camera_dir}camera_arm32.o -o server

server.o: server.cpp
	g++ -c server.cpp -o server.o

client: client.o jpg-receive-save-utils.o ${stb_dir}stb_image_write.o
	g++ client.o jpg-receive-save-utils.o ${stb_dir}stb_image_write.o -o client

client.o: client.cpp
	g++ -c client.cpp -o client.o

jpg-receive-save-utils.o: jpg-receive-save-utils.cpp
	g++ -c jpg-receive-save-utils.cpp -o jpg-receive-save-utils.o

stb_image_write.o: stb_image_write.cpp
	g++ -c stb_image_write.cpp -o ${stb_dir}stb_image_write.o


clean:
	rm -f ${objects}
