cmake_minimum_required(VERSION 3.12)
project(SO2_Game C)

set(CMAKE_C_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(SO2_Game main.c Server.c Server.h state.c state.h board.c board.h player.c player.h Client.c Client.h beast.c beast.h)
find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
target_include_directories(SO2_Game PRIVATE ${CURSES_INCLUDE_DIRS} )
target_link_libraries(SO2_Game PRIVATE ${CURSES_LIBRARIES} PRIVATE Threads::Threads m)