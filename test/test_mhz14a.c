/*
 * Copyright (C) 2018 Kamil Lorenc
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include <setjmp.h>
#include <cmocka.h>

#define main __real_main
#include "mhz14a.c"
#undef main

char *read_argv[] = {
  "./mhz14a",
  "-b", "115200",
  "-m", "9E2",
  "-d", "/dev/ttyS0",
  "-r"
};

int __wrap_process_command (mhopt_t *opts)
{
  char *device = opts->device;
  uint32_t baudrate = opts->baudrate;
  uint8_t databits = opts->databits;
  uint8_t parity = opts->parity;
  uint8_t stopbits = opts->stopbits;
  command_t command = opts->command;
  uint16_t gas_concentration = opts->gas_concentration;
  uint16_t span_point = opts->span_point;

  check_expected(device);
  check_expected(baudrate);
  check_expected(databits);
  check_expected(parity);
  check_expected(stopbits);
  check_expected(command);
  check_expected(gas_concentration);
  check_expected(span_point);

  return mock();
}

static void test_main_read(void **state)
{
  int expected = 0;
  int actual;

  expect_string(__wrap_process_command, device, "/dev/ttyS0");
  expect_value(__wrap_process_command, baudrate, 115200);
  expect_value(__wrap_process_command, databits, 9);
  expect_value(__wrap_process_command, parity, 'E');
  expect_value(__wrap_process_command, stopbits, 2);
  expect_value(__wrap_process_command, command, CMD_GAS_CONCENTRATION);
  expect_value(__wrap_process_command, gas_concentration, 0);
  expect_value(__wrap_process_command, span_point, 0);

  will_return(__wrap_process_command, 0);

  actual = __real_main(sizeof(read_argv)/sizeof(char*), read_argv);

  assert_int_equal(expected, actual);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_main_read),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
