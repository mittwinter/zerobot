cmake_minimum_required(VERSION 2.8)

project(zerobot)

SET(CMAKE_BUILD_TYPE Debug)

add_executable(zerobot
	src/main.cpp
	src/rand.cpp
	src/socket.cpp
	src/zerobot.cpp
	irc/message.cpp
	irc/parser.cpp
	plugins/base.cpp
	plugins/connect.cpp
	plugins/disconnect.cpp
	plugins/pingpong.cpp
	)
add_executable(testcase/parserTest testcase/parserTest.cpp irc/message.cpp irc/parser.cpp)
