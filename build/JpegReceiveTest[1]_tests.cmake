add_test( JpgReceiveSave.ReceiveConfigPacket [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build/Debug/JpegReceiveTest.exe]==] [==[--gtest_filter=JpgReceiveSave.ReceiveConfigPacket]==] --gtest_also_run_disabled_tests)
set_tests_properties( JpgReceiveSave.ReceiveConfigPacket PROPERTIES WORKING_DIRECTORY [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( JpegReceiveTest_TESTS JpgReceiveSave.ReceiveConfigPacket)
