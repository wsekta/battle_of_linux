cmake_minimum_required(VERSION 3.5)

project(Serwer LANGUAGES C)

set(SOURCES
        main.c
        tools.c timers.c)

set(HEADERS
        tools.h timers.h)

add_executable(Serwer ${SOURCES} ${HEADERS})
target_link_libraries(Serwer rt)