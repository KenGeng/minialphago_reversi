cmake_minimum_required(VERSION 3.9)
project(MiniAlphago_Reversi)

set(CMAKE_CXX_STANDARD 11)

add_executable(MiniAlphago_Reversi main.cpp MonteCarlo.h Board.h)