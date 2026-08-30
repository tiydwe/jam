cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp)
target_sources(main PRIVATE
    src/Car.cpp
    src/Intersection.cpp
    src/Lane.cpp
    src/Road.cpp
    src/Simulation.cpp
    src/utility.cpp
    test/t1__1car1road.hpp
    test/t2__1car1intersection.hpp
    include/Car.h
    include/Intersection.h
    include/Lane.h
    include/Road.h
    include/Simulation.h
    include/utility.h
) 
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test")
target_compile_features(main PRIVATE cxx_std_17)
set(SFML_DIR "C:/c++/libaries/SFML-3.1.0/lib/cmake/SFML")
find_package(SFML 3.1 COMPONENTS Graphics Window System Audio REQUIRED)
target_link_libraries(main PRIVATE SFML::Graphics)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
