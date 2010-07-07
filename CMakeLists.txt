cmake_minimum_required(VERSION 2.6)

include(UsePkgConfig)

project(zerobot)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wswitch -Werror")

pkgconfig(sqlite3
	SQLITE3_INCLUDE_DIR
	SQLITE3_LINK_DIR
	SQLITE_LINK_FLAGS
	SQLITE_CFLAGS)
include_directories(${SQLITE3_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SQLITE_CFLAGS}")

add_executable(zerobot
	src/main.cpp
	src/rand.cpp
	src/socket.cpp
	src/zerobot.cpp
	lib/irc/log_sqlite.cpp
	lib/irc/message.cpp
	lib/irc/parser.cpp
	plugins/base.cpp
	plugins/connect.cpp
	plugins/disconnect.cpp
	plugins/join.cpp
	plugins/log.cpp
	plugins/pingpong.cpp
	)
set_target_properties(zerobot PROPERTIES LINK_FLAGS ${SQLITE_LINK_FLAGS})

add_executable(testcase/parserTest
	testcase/parserTest.cpp
	lib/irc/message.cpp 
	lib/irc/parser.cpp
	)
