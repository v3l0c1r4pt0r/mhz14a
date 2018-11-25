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

#include "mh_uart.h"

#include "mh_uart.c"

#define test_sizeof(name) static void test_sizeof_##name(void **state) \
{ \
  assert_int_equal(sizeof(pkt_t), sizeof(name)); \
}

static void test_checksum(void **state)
{
  pkt_t packet = {.start = 0xff, .reserved = {1,2,3,4,5,6,7}};
  uint8_t expected = 0xe4;
  uint8_t actual;

  actual = checksum(&packet);

  assert_int_equal(expected, actual);
}

test_sizeof(sendpkt_t);
test_sizeof(returnpkt_t);
test_sizeof(read_gas_t);
test_sizeof(calibrate_zero_t);
test_sizeof(calibrate_span_t);
test_sizeof(return_gas_t);

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_sizeof_sendpkt_t),
    cmocka_unit_test(test_sizeof_returnpkt_t),
    cmocka_unit_test(test_sizeof_read_gas_t),
    cmocka_unit_test(test_sizeof_calibrate_zero_t),
    cmocka_unit_test(test_sizeof_calibrate_span_t),
    cmocka_unit_test(test_sizeof_return_gas_t),
    cmocka_unit_test(test_checksum),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
