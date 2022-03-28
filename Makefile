stb_lib = libs/third-party/stb-image-write/
camera_lib = libs/third-party/camera/
client_directory = src/client_directory/
objects = server.o client.o ${stb_lib}stb_image_write.o
server_exe = bin/server
client_exe = bin/client
communication_dir = src/communication/
button_dir = src/button/
src_dir = src/

executables = server_exe client_exe


all: server client

server: ${src_dir}server.cpp ${camera_lib}camera_arm32.o
	g++ ${camera_lib}camera_arm32.o ${src_dir}server.cpp ${communication_dir}server_communication.cpp ${communication_dir}image_processor.cpp ${button_dir}button.cpp -o server -lpthread -Wl,/usr/local/lib/libadafruit-bbio1.so
	
# server.o: server.cpp
# 	g++ -c server.cpp -o server.o

client:  
	g++ ${client_directory}client.cpp ${client_directory}/jpg-receive-save-utils.cpp ${src_dir}stb_image_write.cpp -o client

# client.o: client.cpp
# 	g++ -c client.cpp -o client.o

stb_image_write.o: ${src_dir}stb_image_write.cpp
	g++ -c ${src_dir}stb_image_write.cpp -o ${stb_lib}stb_image_write.o

jpg-receive-save-utils.o:
	g++ -c ${client_directory}jpg-receive-save-utils.cpp -o ${client_directory}jpg-receive-save-utils.o

clean:
	rm -f ${objects} ${executables}
