project(ImageZero)

cmake_minimum_required(VERSION 2.6)

set(iz_SRCS
    file.cpp
    libiz.cpp
    main.cpp
    portableimage.cpp
    table.cpp
)

add_definitions(-DHAVE_MMAP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -mmmx -Winline -fomit-frame-pointer -fno-rtti -fno-exceptions")

add_executable(iz ${iz_SRCS})

