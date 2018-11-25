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

typedef struct {int a; int b; int expected;} vector_t;

const vector_t vectors[] = {
  {0,1,0},
  {1,0,0},
  {1,1,1},
  {2,3,6},
};

int __wrap_printf (const char *format, ...)
{
  int param1;

  /* extract result from vargs ('printf("%d\n", result)') */
  va_list args;
  va_start(args, format);
  param1 = va_arg(args, int);
  va_end(args);

  /* ensure that parameters match expecteds in expect_*() calls  */
  check_expected_ptr(format);
  check_expected(param1);

  /* get mocked return value from will_return() call */
  return mock();
}

static void test_internal(void **state)
{
    int actual;
    int i;

    for (i = 0; i < sizeof(vectors)/sizeof(vector_t); i++)
    {
      /* get i-th inputs and expected values as vector */
      const vector_t *vector = &vectors[i];

      /* call function under test */
      actual = internal(vector->a, vector->b);

      /* assert result */
      assert_int_equal(vector->expected, actual);
    }
}

static void test_main(void **state)
{
  int expected = 0;
  int actual;

  /* expect parameters to printf call */
  expect_string(__wrap_printf, format, "%d\n");
  expect_value(__wrap_printf, param1, 60);

  /* printf should return 3 */
  will_return(__wrap_printf, 3);

  /* call __real_main as this is main() from program.c */
  actual = __real_main(0, NULL);

  /* assert that main return success */
  assert_int_equal(expected, actual);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_internal),
    cmocka_unit_test(test_main),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
