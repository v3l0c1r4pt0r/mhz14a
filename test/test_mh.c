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
#include <setjmp.h>
#include <cmocka.h>

#include "mh.h"

#include "mh.c"

#define test_x_to_baud(x, speed, err) static void test_to_baud_##x(void **state) \
{ \
  speed_t expected = speed; \
  speed_t actual; \
 \
  actual = int_to_baud(x); \
 \
  assert_int_equal(expected, actual); \
  assert_int_equal(err, errno); \
}

test_x_to_baud(0, B0, 0);
test_x_to_baud(1, -1, ENOTSUP);
test_x_to_baud(50, B50, 0);
test_x_to_baud(75, B75, 0);
test_x_to_baud(110, B110, 0);
test_x_to_baud(134, B134, 0);
test_x_to_baud(150, B150, 0);
test_x_to_baud(200, B200, 0);
test_x_to_baud(300, B300, 0);
test_x_to_baud(600, B600, 0);
test_x_to_baud(1200, B1200, 0);
test_x_to_baud(1800, B1800, 0);
test_x_to_baud(2400, B2400, 0);
test_x_to_baud(4800, B4800, 0);
test_x_to_baud(9600, B9600, 0);
test_x_to_baud(19200, B19200, 0);
test_x_to_baud(38400, B38400, 0);
test_x_to_baud(57600,  B57600, 0);
test_x_to_baud(115200, B115200, 0);
test_x_to_baud(230400, B230400, 0);
test_x_to_baud(460800, B460800, 0);
test_x_to_baud(500000, B500000, 0);
test_x_to_baud(576000, B576000, 0);
test_x_to_baud(921600, B921600, 0);
test_x_to_baud(1000000, B1000000, 0);
test_x_to_baud(1152000, B1152000, 0);
test_x_to_baud(1500000, B1500000, 0);
test_x_to_baud(2000000, B2000000, 0);
test_x_to_baud(2500000, B2500000, 0);
test_x_to_baud(3000000, B3000000, 0);
test_x_to_baud(3500000, B3500000, 0);
test_x_to_baud(4000000, B4000000, 0);

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_to_baud_0),
    cmocka_unit_test(test_to_baud_1),
    cmocka_unit_test(test_to_baud_50),
    cmocka_unit_test(test_to_baud_75),
    cmocka_unit_test(test_to_baud_110),
    cmocka_unit_test(test_to_baud_134),
    cmocka_unit_test(test_to_baud_150),
    cmocka_unit_test(test_to_baud_200),
    cmocka_unit_test(test_to_baud_300),
    cmocka_unit_test(test_to_baud_600),
    cmocka_unit_test(test_to_baud_1200),
    cmocka_unit_test(test_to_baud_1800),
    cmocka_unit_test(test_to_baud_2400),
    cmocka_unit_test(test_to_baud_4800),
    cmocka_unit_test(test_to_baud_9600),
    cmocka_unit_test(test_to_baud_19200),
    cmocka_unit_test(test_to_baud_38400),
    cmocka_unit_test(test_to_baud_57600),
    cmocka_unit_test(test_to_baud_115200),
    cmocka_unit_test(test_to_baud_230400),
    cmocka_unit_test(test_to_baud_460800),
    cmocka_unit_test(test_to_baud_500000),
    cmocka_unit_test(test_to_baud_576000),
    cmocka_unit_test(test_to_baud_921600),
    cmocka_unit_test(test_to_baud_1000000),
    cmocka_unit_test(test_to_baud_1152000),
    cmocka_unit_test(test_to_baud_1500000),
    cmocka_unit_test(test_to_baud_2000000),
    cmocka_unit_test(test_to_baud_2500000),
    cmocka_unit_test(test_to_baud_3000000),
    cmocka_unit_test(test_to_baud_3500000),
    cmocka_unit_test(test_to_baud_4000000),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
