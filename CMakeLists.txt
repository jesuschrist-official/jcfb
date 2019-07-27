cmake_minimum_required(VERSION 3.0)
project(jcfb)

set(CMAKE_C_FLAGS "-Wall -Werror -Wfatal-errors -std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -flto")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(src)
add_subdirectory(sample)
add_subdirectory(benchmarks)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

