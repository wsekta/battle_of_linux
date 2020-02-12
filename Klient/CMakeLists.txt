cmake_minimum_required(VERSION 3.5)

project(Klient LANGUAGES C)

set(SOURCES
        main.c
)

set(HEADERS)

add_executable(Klient ${SOURCES} ${HEADERS})
target_link_libraries(Klient rt)