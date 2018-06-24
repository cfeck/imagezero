cmake_minimum_required(VERSION 2.8)

project(imagezero)

set(MAJOR_VERSION 0)
set(RELEASE_VERSION 2.4)
set(IZ_VERSION ${MAJOR_VERSION}.${RELEASE_VERSION})
include_directories(./include)

set(iz_SRCS
    src/main.cpp
)

set(libiz_SRCS
    src/file.cpp
    src/libiz.cpp
    src/portableimage.cpp
    src/table.cpp
)

add_definitions(-DHAVE_MMAP)

execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)

message(STATUS "Architecture: ${ARCHITECTURE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCH_SPECIFIC_FLAGS} -Winline -W -Wall -fomit-frame-pointer -fno-rtti -fno-exceptions")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-flto,-s")
#set(CMAKE_EXE_LINKER_FLAGS "-Wl,-O1,-s,--no-export-dynamic")

add_library(${PROJECT_NAME}
    SHARED
    ${libiz_SRCS}
  )
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${IZ_VERSION} SOVERSION ${MAJOR_VERSION})

add_executable(iz
    ${iz_SRCS}
  )

target_link_libraries(iz
    ${PROJECT_NAME}
  )
