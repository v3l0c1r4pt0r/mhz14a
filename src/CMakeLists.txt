configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
add_executable(mhz14a mhz14a.c mh.c mh_uart.c logger.c)
target_include_directories(mhz14a PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mhz14a
         DESTINATION bin
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                     GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE
)
