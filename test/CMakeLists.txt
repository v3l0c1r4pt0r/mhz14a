add_mocked_test(program SOURCES ${CMAKE_SOURCE_DIR}/src/mh_uart.c MOCKS printf)
target_include_directories(test_program PRIVATE ${CMAKE_BINARY_DIR}/src)
add_mocked_test(mh_uart)
