cmake_minimum_required(VERSION 3.20)

set(VCPKG_NOTICE_OPTIONS "-quiet" CACHE STRING "" FORCE)
set(VCPKG_VERBOSE OFF CACHE BOOL "" FORCE)

project("CusEngine")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("Source/Engine")
add_subdirectory("Source/Modules/RHI_Vulkan") #TEMP
add_subdirectory("Source/Sandbox")