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

#include "mhz14a.h"

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

char *span_argv[] = {
  "./mhz14a",
  "-s", "65535"
};

char *span_noarg_argv[] = {
  "./mhz14a",
  "-s"
};

char *zero_argv[] = {
  "./mhz14a",
  "-z"
};

char *multi_argv[] = {
  "./mhz14a",
  "-z", "-s", "-r"
};

char *nocmd_argv[] = {
  "./mhz14a"
};

char *wrongopt_argv[] = {
  "./mhz14a",
  "-#"
};

char *wrong_mode_argv1[] = {
  "./mhz14a",
  "-m", "an1"
};

char *wrong_mode_argv2[] = {
  "./mhz14a",
  "-m", "811"
};

char *wrong_mode_argv3[] = {
  "./mhz14a",
  "-m", "8nb"
};

char *wrong_mode_argv4[] = {
  "./mhz14a",
  "-m", "test"
};

int __wrap_printf (const char *format, ...)
{
  return 1;
}

/* if printf has no VARGS it is replaced with puts() */
int __wrap_puts (const char *format)
{
  return 1;
}

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
  int expected = RET_SUCCESS;
  int actual;

  expect_string(__wrap_process_command, device, "/dev/ttyS0");
  expect_value(__wrap_process_command, baudrate, 115200);
  expect_value(__wrap_process_command, databits, 9);
  expect_value(__wrap_process_command, parity, 'E');
  expect_value(__wrap_process_command, stopbits, 20);
  expect_value(__wrap_process_command, command, CMD_GAS_CONCENTRATION);
  expect_value(__wrap_process_command, gas_concentration, 0);
  expect_value(__wrap_process_command, span_point, 0);

  will_return(__wrap_process_command, 0);

  actual = __real_main(sizeof(read_argv)/sizeof(char*), read_argv);

  assert_int_equal(expected, actual);
}

static void test_main_span(void **state)
{
  int expected = RET_SUCCESS;
  int actual;

  expect_value(__wrap_process_command, device, NULL);
  expect_value(__wrap_process_command, baudrate, 9600);
  expect_value(__wrap_process_command, databits, 8);
  expect_value(__wrap_process_command, parity, 'N');
  expect_value(__wrap_process_command, stopbits, 10);
  expect_value(__wrap_process_command, command, CMD_CALIBRATE_SPAN);
  expect_value(__wrap_process_command, gas_concentration, 0);
  expect_value(__wrap_process_command, span_point, 65535);

  will_return(__wrap_process_command, 0);

  actual = __real_main(sizeof(span_argv)/sizeof(char*), span_argv);

  assert_int_equal(expected, actual);
}

static void test_main_span_noarg(void **state)
{
  int expected = RET_UNPARSED;
  int actual;

  actual = __real_main(sizeof(span_noarg_argv)/sizeof(char*), span_noarg_argv);

  assert_int_equal(expected, actual);
}

static void test_main_zero(void **state)
{
  int expected = RET_SUCCESS;
  int actual;

  expect_value(__wrap_process_command, device, NULL);
  expect_value(__wrap_process_command, baudrate, 9600);
  expect_value(__wrap_process_command, databits, 8);
  expect_value(__wrap_process_command, parity, 'N');
  expect_value(__wrap_process_command, stopbits, 10);
  expect_value(__wrap_process_command, command, CMD_CALIBRATE_ZERO);
  expect_value(__wrap_process_command, gas_concentration, 0);
  expect_value(__wrap_process_command, span_point, 0);

  will_return(__wrap_process_command, 0);

  actual = __real_main(sizeof(zero_argv)/sizeof(char*), zero_argv);

  assert_int_equal(expected, actual);
}

static void test_main_multi(void **state)
{
  int expected = RET_CMD_DUPL;
  int actual;

  actual = __real_main(sizeof(multi_argv)/sizeof(char*), multi_argv);

  assert_int_equal(expected, actual);
}

static void test_main_nocmd(void **state)
{
  int expected = RET_NOCMD;
  int actual;

  actual = __real_main(sizeof(nocmd_argv)/sizeof(char*), nocmd_argv);

  assert_int_equal(expected, actual);
}

static void test_main_wrongopt(void **state)
{
  int expected = RET_UNPARSED;
  int actual;

  actual = __real_main(sizeof(wrongopt_argv)/sizeof(char*), wrongopt_argv);

  assert_int_equal(expected, actual);
}

#define test_main_wrong_mode(num) static void test_main_wrong_mode##num(void **state) \
{ \
  int expected = RET_MODE_ERR; \
  int actual; \
 \
  actual = __real_main(sizeof(wrong_mode_argv##num)/sizeof(char*), wrong_mode_argv##num); \
 \
  assert_int_equal(expected, actual); \
}

test_main_wrong_mode(1);
test_main_wrong_mode(2);
test_main_wrong_mode(3);
test_main_wrong_mode(4);

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_main_read),
    cmocka_unit_test(test_main_span),
    cmocka_unit_test(test_main_span_noarg),
    cmocka_unit_test(test_main_zero),
    cmocka_unit_test(test_main_multi),
    cmocka_unit_test(test_main_nocmd),
    cmocka_unit_test(test_main_wrongopt),
    cmocka_unit_test(test_main_wrong_mode1),
    cmocka_unit_test(test_main_wrong_mode2),
    cmocka_unit_test(test_main_wrong_mode3),
    cmocka_unit_test(test_main_wrong_mode4),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
