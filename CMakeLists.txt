##
## EPITECH PROJECT, 2026
## Project - Lattice
## File description:
## Main project build file
##

cmake_minimum_required(VERSION 3.15)

project(Lattice VERSION 1.0
                DESCRIPTION "Lattice Project - 2026"
                HOMEPAGE_URL "https://github.com/Bard-Gaming/liblattice"
                LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(lattice SHARED
                    src/sockets/NonBlockingSocket/NonBlockingSocket.cpp
                    src/sockets/Socket/Socket.cpp)

target_include_directories(lattice PRIVATE include/)
