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

typedef struct {int input; int expected;} vector_t;

const vector_t vectors[] = {
  {0,0},
  {1,2},
  {2,4},
  {INT_MAX/2, INT_MAX-1},
  {INT_MAX, -2},
};

static void test_external(void **state)
{
    int actual;
    int i;

    for (i = 0; i < sizeof(vectors)/sizeof(vector_t); i++)
    {
      const vector_t *vector = &vectors[i];
      actual = external(vector->input);
      assert_int_equal(vector->expected, actual);
    }
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_external),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
