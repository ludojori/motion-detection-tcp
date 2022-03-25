stb_image_write = include/third-party/stb-image-write/
camera = include/third-party/camera/
motion_detection_tcp = include/motion_detection_tcp/


all: server client

server:
	g++ src/server.cpp ${camera}camera_arm32.o src/server-communication.cpp src/image-processor.cpp src/button.cpp -o server -lpthread -Wl,/usr/local/lib/libadafruit-bbio1.so

client:  
	g++ src/client.cpp src/jpg-receive-save-utils.cpp src/stb-image-write.cpp -o client

clean:
	rm -f server client
