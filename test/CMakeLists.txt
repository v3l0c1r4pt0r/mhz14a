add_mocked_test(mhz14a
  SOURCES ${CMAKE_SOURCE_DIR}/src/mh.c
          ${CMAKE_SOURCE_DIR}/src/mh_uart.c
  MOCKS process_command)
target_include_directories(test_mhz14a PRIVATE ${CMAKE_BINARY_DIR}/src)
add_mocked_test(mh_uart)
