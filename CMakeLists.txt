cmake_minimum_required(VERSION 3.5)
project(kernel)

set(SYSROOT sysroot)
SET(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS  ${CMAKE_C_FLAGS} " --sysroot=${SYSROOT}" )

include_directories("include")
include_directories("include/uapi")
include_directories("include/generated")
include_directories("arch/arm64/include")
include_directories("arch/arm64/include/uapi")
include_directories("arch/arm64/include/generated")
include_directories("arch/arm64/include/generated/uapi")

add_definitions(-D__KERNEL__)