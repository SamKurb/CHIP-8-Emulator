cmake_minimum_required(VERSION 3.15)

project(chip8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/main.cpp
    src/Chip8.cpp
    src/inputhandler.cpp
    src/renderer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

find_package(SDL2_ttf REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)

find_package(SDL2_mixer REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/roms"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/roms"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
