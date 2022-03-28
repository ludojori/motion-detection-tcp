<<<<<<< HEAD
stb_lib = ../libs/third-party/stb-image-write/
camera_lib = ../libs/third-party/camera/
jpeg_receive_save_lib = client_directory/
objects = server.o client.o ${stb_lib}stb_image_write.o
server_exe = ../bin/server
client_exe = ../bin/client
=======
stb_lib = libs/third-party/stb-image-write/
camera_lib = libs/third-party/camera/
client_directory = src/client_directory/
objects = server.o client.o ${stb_lib}stb_image_write.o
server_exe = bin/server
client_exe = bin/client
communication_dir = src/communication/
button_dir = src/button/

>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c
executables = server_exe client_exe


all: server client

server: server.cpp ${camera_lib}camera_arm32.o
<<<<<<< HEAD
	g++ ${camera_lib}camera_arm32.o server.cpp communication/server_communication.cpp communication/image_processor.cpp button/button.cpp -o server -lpthread -Wl,/usr/local/lib/libadafruit-bbio1.so
=======
	g++ ${camera_lib}camera_arm32.o server.cpp ${communication_dir}server_communication.cpp ${communication_dir}image_processor.cpp ${button_dir}button.cpp -o server -lpthread -Wl,/usr/local/lib/libadafruit-bbio1.so
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c
	
# server.o: server.cpp
# 	g++ -c server.cpp -o server.o

client:  
<<<<<<< HEAD
	g++ client_directory/client.cpp client_directory/jpg-receive-save-utils.cpp stb_image_write.cpp -o client
=======
	g++ ${client_directory}/client.cpp ${client_directory}/jpg-receive-save-utils.cpp stb_image_write.cpp -o client
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c

# client.o: client.cpp
# 	g++ -c client.cpp -o client.o

stb_image_write.o: stb_image_write.cpp
	g++ -c stb_image_write.cpp -o ${stb_lib}stb_image_write.o

jpg-receive-save-utils.o:
<<<<<<< HEAD
	g++ -c ${jpeg_receive_save_lib}jpg-receive-save-utils.cpp -o ${jpeg_receive_save_lib}jpg-receive-save-utils.o
=======
	g++ -c ${client_directory}jpg-receive-save-utils.cpp -o ${client_directory}jpg-receive-save-utils.o
>>>>>>> dd00e90a4bead35404aa78577f45065dfff9ac9c

clean:
	rm -f ${objects} ${executables}
