cmake_minimum_required(VERSION 3.15...3.23)

project(MotionDetectionTcp
	VERSION 1.0
	DESCRIPTION "A simulation of a motion-detection camera communicating with other devices"
)

add_executable(client client.cpp)
add_executable(server server.cpp)