add_test( ServerCommunicationTest.getAveragePixelsTest [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build/Debug/ServerCommunicationTest.exe]==] [==[--gtest_filter=ServerCommunicationTest.getAveragePixelsTest]==] --gtest_also_run_disabled_tests)
set_tests_properties( ServerCommunicationTest.getAveragePixelsTest PROPERTIES WORKING_DIRECTORY [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( ServerCommunicationTest.calculatePictureDifferenceTest [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build/Debug/ServerCommunicationTest.exe]==] [==[--gtest_filter=ServerCommunicationTest.calculatePictureDifferenceTest]==] --gtest_also_run_disabled_tests)
set_tests_properties( ServerCommunicationTest.calculatePictureDifferenceTest PROPERTIES WORKING_DIRECTORY [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( ServerCommunicationTest_TESTS ServerCommunicationTest.getAveragePixelsTest ServerCommunicationTest.calculatePictureDifferenceTest)
