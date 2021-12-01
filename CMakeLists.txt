cmake_minimum_required(VERSION 3.6)

PROJECT(voxellium)

# For Platform
if(NOT PLATFORM)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PLATFORM 64)
  else()
    set(PLATFORM 32)
  endif()
endif()

# For Libraries
find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# For Sources
file(GLOB_RECURSE header RELATIVE ${CMAKE_SOURCE_DIR} "src/*.h")
file(GLOB_RECURSE engineheader RELATIVE ${CMAKE_SOURCE_DIR} "Engine/*.h")

file(GLOB_RECURSE src RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE enginesrc RELATIVE ${CMAKE_SOURCE_DIR} "Engine/*.cpp")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_executable(voxellium ${src} ${header} ${engineheader} ${enginesrc})

target_include_directories(voxellium PUBLIC "Engine/")


# For Linking
target_link_libraries(voxellium ${CONAN_LIBS} Vulkan::Vulkan Threads::Threads )

target_compile_features(voxellium PRIVATE cxx_std_20)
