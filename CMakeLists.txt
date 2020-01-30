cmake_minimum_required(VERSION 3.5)

project(battle_of_linux LANGUAGES C)

add_executable(Serwer server.c)

add_executable(Klient client.c)

add_executable(Hooligan hooligan.c)

add_executable(Arbiter arbiter.c)