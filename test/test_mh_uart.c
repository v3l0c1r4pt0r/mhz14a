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

static void test_checksum_overflow(void **state)
{
  pkt_t packet = {.start = 0xff, .reserved = {0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff}};
  uint8_t expected = 0x7;
  uint8_t actual;

  actual = checksum(&packet);

  assert_int_equal(expected, actual);
}

static void test_checksum_inval_in(void **state)
{
  uint8_t expected = 0xff; // TODO: are you sure ?
  uint8_t actual;

  actual = checksum(NULL);

  assert_int_equal(expected, actual);
}

test_sizeof(sendpkt_t);
test_sizeof(returnpkt_t);
test_sizeof(read_gas_t);
test_sizeof(calibrate_zero_t);
test_sizeof(calibrate_span_t);
test_sizeof(return_gas_t);

static void test_read_packet(void **state)
{
  uint8_t expected[] = {0xff, 1, 0x86, 0, 0, 0, 0, 0, 0x79};
  pkt_t actual;

  actual = init_read_gas_packet();

  assert_memory_equal(expected, &actual, sizeof(pkt_t));
}

static void test_zero_packet(void **state)
{
  uint8_t expected[] = {0xff, 1, 0x87, 0, 0, 0, 0, 0, 0x78};
  pkt_t actual;

  actual = init_calibrate_zero_packet();

  assert_memory_equal(expected, &actual, sizeof(pkt_t));
}

static void test_span_packet(void **state)
{
  uint8_t expected[] = {0xff, 1, 0x88, 7, 0xd0, 0, 0, 0, 0xa0};
  pkt_t actual;

  actual = init_calibrate_span_packet(0x7d0);

  assert_memory_equal(expected, &actual, sizeof(pkt_t));
}

static void test_gas_return(void **state)
{
  uint8_t input[] = {0xff, 0x86, 2, 0x60, 0x47, 0, 0, 0, 0xd1};
  uint16_t expected = 0x260;
  uint16_t actual;

  actual = return_gas_concentration(*((pkt_t*) input));

  assert_int_equal(expected, actual);
}

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
    cmocka_unit_test(test_checksum_overflow),
    cmocka_unit_test(test_checksum_inval_in),
    cmocka_unit_test(test_read_packet),
    cmocka_unit_test(test_zero_packet),
    cmocka_unit_test(test_span_packet),
    cmocka_unit_test(test_gas_return),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
