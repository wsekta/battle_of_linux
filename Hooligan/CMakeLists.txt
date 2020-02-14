cmake_minimum_required(VERSION 3.5)

project(Hooligan LANGUAGES C)

set(SOURCES
        main.c
)

set(HEADERS)

add_executable(Hooligan ${SOURCES} ${HEADERS})
target_link_libraries(Hooligan rt)