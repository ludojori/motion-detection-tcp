if(EXISTS "D:/Documents/Bosh C++ Camp/motion-detection-tcp/build/JpegReceiveTest[1]_tests.cmake")
  include("D:/Documents/Bosh C++ Camp/motion-detection-tcp/build/JpegReceiveTest[1]_tests.cmake")
else()
  add_test(JpegReceiveTest_NOT_BUILT JpegReceiveTest_NOT_BUILT)
endif()
