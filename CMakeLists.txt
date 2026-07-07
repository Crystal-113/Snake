cmake_minimum_required(VERSION 3.16)

project(Snake VERSION 1.1.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(Snake WIN32
    src/main.cpp
    src/DefaultGamemode.cpp
    src/Renderer.cpp
    src/Texture.cpp
)

target_include_directories(Snake PRIVATE
    include
)

target_link_libraries(Snake PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

add_custom_command(TARGET Snake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Snake>/assets
)

install(TARGETS Snake
        RUNTIME DESTINATION .)

install(DIRECTORY assets
        DESTINATION .)
