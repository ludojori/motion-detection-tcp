stb_lib = ../libs/third-party/stb-image-write/
camera_lib = ../libs/third-party/camera/
objects = server.o client.o ${stb_lib}stb_image_write.o
server_exe = ../bin/server
client_exe = ../bin/client
executables = server_exe client_exe


all: server client

server: server.cpp ${camera_lib}camera_arm32.o
	g++ ${camera_lib}camera_arm32.o server.cpp communication/server_communication.cpp communication/image_processor.cpp -o server -lpthread -Wl,/usr/local/lib/libadafruit-bbio1.so
	

# server.o: server.cpp
# 	g++ -c server.cpp -o server.o

client: client.cpp
	g++ client.cpp stb_image_write.cpp -o client

# client.o: client.cpp
# 	g++ -c client.cpp -o client.o

stb_image_write.o: stb_image_write.cpp
	g++ -c stb_image_write.cpp -o ${stb_lib}stb_image_write.o

clean:
	rm -f ${objects} ${executables}
