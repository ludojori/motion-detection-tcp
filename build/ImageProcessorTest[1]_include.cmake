if(EXISTS "C:/Users/Vladi/Documents/Bosch/motion-detection-tcp/build/ImageProcessorTest[1]_tests.cmake")
  include("C:/Users/Vladi/Documents/Bosch/motion-detection-tcp/build/ImageProcessorTest[1]_tests.cmake")
else()
  add_test(ImageProcessorTest_NOT_BUILT ImageProcessorTest_NOT_BUILT)
endif()