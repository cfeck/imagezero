project(ImageZero)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE "Release")

set(iz_SRCS
    file.cpp
    libiz.cpp
    main.cpp
    portableimage.cpp
    table.cpp
)

add_definitions(-DHAVE_MMAP)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-O1,-s,--no-export-dynamic")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mmmx -mtune=atom -Winline -W -Wall -fomit-frame-pointer -fno-rtti -fno-exceptions")

add_executable(iz ${iz_SRCS})

