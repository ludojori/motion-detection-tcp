cmake_minimum_required(VERSION 3.7)

set(This motion_detection)

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(Headers
    src/communication/server_communication.h
    src/communication/image_processor.h
    src/client_directory/jpg-receive-save-utils.h
    src/button/button.h
)

set(Sources
    src/communication/server_communication.cpp
    src/communication/image_processor.cpp
    src/client_directory/jpg-receive-save-utils.cpp
    src/button/button.cpp
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
  test/ServerCommunicationTest.cpp
)
target_link_libraries(
  hello_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)

# add_executable(${This} ${Sources} ${Headers})