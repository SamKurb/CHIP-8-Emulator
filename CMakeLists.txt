cmake_minimum_required(VERSION 3.15)


project(chip8)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MAIN_SOURCES
    src/main.cpp
    src/chip8.cpp
    src/inputhandler.cpp
    src/renderer.cpp
    src/audioplayer.cpp
    src/imguirenderer.cpp
)

set(OTHER_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp

    external/imgui/imgui_demo.cpp

    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_sdlrenderer2.cpp

    external/imgui_file_dialog/ImGuiFileDialog.cpp
        include/exceptions/sdlinitexception.h
        include/exceptions/badopcodeexception.h
        include/exceptions/fileinputexception.h
        src/frametimer.cpp
        include/utils/frametimer.h
        src/statemanager.cpp
        include/emulator.h
        src/emulator.cpp
        include/types/enumarray.h
        include/exceptions/chipstackerrorexception.h
        include/exceptions/chipoobmemoryaccessexception.h
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES} ${OTHER_SOURCES})

foreach(src_file IN LISTS MAIN_SOURCES)
    set_source_files_properties(${src_file} PROPERTIES COMPILE_FLAGS
            "-Wall -Wextra -Wconversion -Wsign-conversion -Werror")
endforeach()

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(SDL2 REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

find_package(SDL2_ttf REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)

find_package(SDL2_mixer REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"

)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends/"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui_file_dialog/"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui_file_dialog/"
)

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
