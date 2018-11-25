configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
add_executable(program program.c mh_uart.c)
target_include_directories(program PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
