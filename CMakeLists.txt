cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

project(zerobot)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wswitch -Werror")

set(LOCAL_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/local/build/include/")
set(LOCAL_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/local/build/lib/")
set(CMAKE_INCLUDE_PATH "${LOCAL_INCLUDE_PATH}")
set(CMAKE_LIBRARY_PATH "${LOCAL_LIBRARY_PATH}")
include_directories(${LOCAL_INCLUDE_PATH})
link_directories(${LOCAL_LIBRARY_PATH})
set(LOCAL_LIBRARY_FLAGS "-ltidy")

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(EXPAT REQUIRED)
include_directories(${EXPAT_INCLUDE_DIRS})

find_package(Sqlite REQUIRED)
include_directories(${SQLITE_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SQLITE_DEFINITIONS}")

find_package(Tidy REQUIRED)
include_directories(${TIDY_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/zerobot.cpp
	lib/irc/log_sqlite.cpp
	lib/irc/message.cpp
	lib/irc/parser.cpp
	lib/SecretStorage.cpp
	lib/rand.cpp
	lib/version.cpp
	plugins/base.cpp
	plugins/admin.cpp
	plugins/connect.cpp
	plugins/disconnect.cpp
	plugins/join.cpp
	plugins/log.cpp
	plugins/nickserv.cpp
	plugins/pingpong.cpp
	plugins/reconnect.cpp
	plugins/urltitle.cpp
	posix/client_socket.cpp
	posix/server_socket.cpp
	posix/socket.cpp
	)
set_target_properties(zerobot
	PROPERTIES LINK_FLAGS "${CURL_LIBRARIES} ${EXPAT_LIBRARIES} ${SQLITE_LIBRARIES} ${TIDY_LIBRARIES} ${LOCAL_LIBRARY_FLAGS}"
	)

