cmake_minimum_required(VERSION 3.7)

set(This ServerCommunicationTest)

set(Sources 
    ServerCommunicationTest.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    gtest_main
    motion_detection
)

add_test(
    NAME ${This}
    COMMAND ${This}
)