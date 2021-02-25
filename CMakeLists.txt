cmake_minimum_required(VERSION 3.6)

PROJECT(Basalt)



# For Platform
if(NOT PLATFORM)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PLATFORM 64)
  else()
    set(PLATFORM 32)
  endif()
endif()

# For Sources
file(GLOB_RECURSE header RELATIVE ${CMAKE_SOURCE_DIR} "src/*.h")
file(GLOB_RECURSE engineheader RELATIVE ${CMAKE_SOURCE_DIR} "Engine/*.h")

file(GLOB_RECURSE src RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE enginesrc RELATIVE ${CMAKE_SOURCE_DIR} "Engine/*.cpp")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(osrc ${src} "${CMAKE_SOURCE_DIR}/src/Main.cpp")
#set(osrc ${osrc} "${CMAKE_SOURCE_DIR}/Engine/glad/glad.c")
add_executable(Basalt ${src} ${header} ${engineheader} ${enginesrc})

# For Libraries
find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

if(EXISTS ${CMAKE_SOURCE_DIR}/Libraries/glm/CMakeLists.txt)
    include_directories(Libraries/glm)
else()
    find_package(glm REQUIRED)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/Libraries/glfw/CMakeLists.txt)
    add_subdirectory(Libraries/glfw)
	include_directories(Libraries/glfw/include)
else()
    find_package(glfw3 REQUIRED)
endif()


#target_compile_options(Basalt PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#target_link_options(Basalt PUBLIC -fsanitize=address)

# For Linking
target_link_libraries(Basalt PRIVATE Vulkan::Vulkan Threads::Threads glfw)




target_compile_features(Basalt PRIVATE cxx_std_17)
