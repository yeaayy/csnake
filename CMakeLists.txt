cmake_minimum_required(VERSION 3.15)
project(CMath)

add_compile_options("$<BUILD_INTERFACE:-Wall;-ggdb>")

add_executable(snake)
target_sources(snake PUBLIC
    src/snake.c
    src/main.c
)
target_link_libraries(snake PUBLIC -lncurses)
