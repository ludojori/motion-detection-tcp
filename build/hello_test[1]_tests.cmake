add_test( ServerCommunicationTest.TRY [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build/Debug/hello_test.exe]==] [==[--gtest_filter=ServerCommunicationTest.TRY]==] --gtest_also_run_disabled_tests)
set_tests_properties( ServerCommunicationTest.TRY PROPERTIES WORKING_DIRECTORY [==[D:/Documents/Bosh C++ Camp/motion-detection-tcp/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( hello_test_TESTS ServerCommunicationTest.TRY)
