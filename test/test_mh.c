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

static void test_9600_to_baud(void **state)
{
  speed_t expected = B9600;
  speed_t actual;

  actual = int_to_baud(9600);

  assert_int_equal(expected, actual);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_9600_to_baud),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
