cmake_minimum_required (VERSION 3.0)
project (mhz14a)

set(MHZ14A_VERSION "0.2.0")

add_subdirectory(src)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# cmocka
option(ENABLE_TESTS "Perform unit tests after build" OFF)
if (ENABLE_TESTS)
  find_package(CMocka CONFIG REQUIRED)
  include(AddCMockaTest)
  include(AddMockedTest)
  add_subdirectory(test)
  enable_testing()
endif(ENABLE_TESTS)
