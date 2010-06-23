cmake_minimum_required(VERSION 2.8)

project(zerobot)

SET(CMAKE_BUILD_TYPE Debug)

include_directories(${ZEROBOT_SOURCE_DIR}/irc)

add_executable(zerobot main.cpp socket.cpp zerobot.cpp irc/message.cpp irc/parser.cpp)
add_executable(testcase/parserTest testcase/parserTest.cpp irc/message.cpp irc/parser.cpp)
