add_library(hashing
	hashing.cpp
	hashing.hpp)

add_executable(hashing.test
	catch_main.cpp
	hashing.test.cpp)

target_link_libraries(hashing.test hashing boost_contract boost_system)

add_test(NAME hashing.test COMMAND hashing.test)
