#
# CENTAUR
# by oldteam & lomaster
# license GPL-3.0
#   Сделано от души 2023.
#

cmake_minimum_required(VERSION 3.18.4)
set(CMAKE_CXX_STANDARD 20)

project(centaur)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
    "CMakeCache.txt;CMakeFiles;Makefile;cmake_install.cmake;resources/data")

file(GLOB SRCS *.cc)
file(GLOB C_SRCS *.c)

add_executable(centaur ${SRCS} ${C_SRCS})
target_link_libraries(centaur pthread ssl crypto)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
