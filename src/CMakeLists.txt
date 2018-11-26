configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
add_executable(mhz14a mhz14a.c mh.c mh_uart.c)
target_include_directories(mhz14a PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
