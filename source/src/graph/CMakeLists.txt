add_library(graph
	graph.cpp
	graph.hpp)

add_executable(graph.test
	catch_main.cpp
	graph.test.cpp)

target_link_libraries(graph.test graph boost_contract boost_system)
