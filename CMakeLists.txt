cmake_minimum_required(VERSION 3.5)

project(battle_of_linux LANGUAGES C)

add_subdirectory(Serwer)

add_subdirectory(Arbiter)

add_subdirectory(Klient)

add_executable(Hooligan hooligan.c)