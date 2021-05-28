include_directories (${TEST_SOURCE_DIR}/include()) 
add_executable (test1 test_main.cpp) 

add_test(NAME MyTest COMMAND test1)
