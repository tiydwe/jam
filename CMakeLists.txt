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
    src/Car_merge.cpp
    src/Car_misc.cpp
    src/Intersection.cpp
    src/Lane.cpp
    src/Road.cpp
    src/Simulation.cpp
    src/utility.cpp
    src/TrafficLight.cpp
    src/Game.cpp
    src/physical/RoadPhysical.cpp
    src/physical/CarPhysical.cpp
    src/physical/IntersectionPhysical.cpp
    include/Car.h
    include/Intersection.h
    include/Lane.h
    include/Road.h
    include/Simulation.h
    include/utility.h
    include/TrafficLight.h
    include/Game.h
    include/physical/RoadPhysical.h
    include/physical/CarPhysical.h
    include/physical/IntersectionPhysical.h
) 
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/physical")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/physical")
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test")
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:main>/assets"
    COMMENT "Copying assets folder to build directory..."
)
target_compile_features(main PRIVATE cxx_std_17)
set(SFML_DIR "C:/c++/libaries/SFML-3.1.0/lib/cmake/SFML")
find_package(SFML 3.1 COMPONENTS Graphics Window System Audio REQUIRED)
target_link_libraries(main PRIVATE SFML::Graphics)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
