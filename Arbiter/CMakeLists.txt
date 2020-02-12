cmake_minimum_required(VERSION 3.5)

project(Arbiter LANGUAGES C)

set(SOURCES
        main.c
)

set(HEADERS)

add_executable(Arbiter ${SOURCES} ${HEADERS})