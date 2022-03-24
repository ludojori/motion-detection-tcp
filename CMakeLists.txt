cmake_minimum_required(VERSION 3.7)

set(This motion_detection)

project(${This} C CXX
#    VERSION 1.0
#    DESCRIPTION "A simulation of a motion-detection camera communicating with other devices"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(Headers
    include/motion_detection-tcp/server-communication.h
    include/motion_detection-tcp/image-processor.h
    include/motion_detection-tcp/jpg-receive-save-utils.h
    include/motion_detection-tcp/button.h
    include/motion_detection-tcp/tcp-protocol.h
    include/third-party/camera/camera.h
    include/third-party/stb-image-write/stb-image-write.h
)

set(Sources
    src/server-communication.cpp
    src/image-processor.cpp
    src/jpg-receive-save-utils.cpp
    src/button.cpp
)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  hello_test
  tests/ServerCommunicationTest.cpp
)
target_link_libraries(
  hello_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)

add_executable(${This} ${Sources} ${Headers})