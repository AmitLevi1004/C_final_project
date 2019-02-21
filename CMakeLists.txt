cmake_minimum_required(VERSION 3.10)
project(c)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(c
        Algorithm.c
        Algorithm.h
        AssemblyDS.c
        AssemblyDS.h
        FileManagement.c
        FileManagement.h
        HashTabel.c
        HashTabel.h
        main.c
        makefile
        ps.as
        ps.ent
        ps.ext
        ps.ob
        test.as)
