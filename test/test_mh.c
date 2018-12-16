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
#include <string.h>
#include <limits.h>

#include "mh.h"

#include "mh.c"

int __wrap_tcgetattr(int fd, struct termios *termios_p)
{
  check_expected(fd);
  check_expected(termios_p);

  // TODO: fill options
  termios_p->c_iflag = mock();
  termios_p->c_oflag = mock();
  termios_p->c_cflag = mock();
  termios_p->c_lflag = mock();
  termios_p->c_line = mock();
  memcpy(termios_p->c_cc, (void*) mock(), NCCS);
  termios_p->c_ispeed = mock();
  termios_p->c_ospeed = mock();

  return mock();
}

int __wrap_tcsetattr(int fd, int optional_actions,
    const struct termios *termios_p)
{
  tcflag_t c_iflag = termios_p->c_iflag;
  tcflag_t c_oflag = termios_p->c_oflag;
  tcflag_t c_cflag = termios_p->c_cflag;
  tcflag_t c_lflag = termios_p->c_lflag;
  cc_t c_line = termios_p->c_line;
  const cc_t *c_cc = termios_p->c_cc;
  speed_t c_ispeed = termios_p->c_ispeed;
  speed_t c_ospeed = termios_p->c_ospeed;
 
  check_expected(fd);
  check_expected(optional_actions);
  check_expected(termios_p);

  check_expected(c_iflag);
  check_expected(c_oflag);
  check_expected(c_cflag);
  check_expected(c_lflag);
  check_expected(c_cc);
  check_expected(c_line);
  check_expected(c_ispeed);
  check_expected(c_ospeed);

  return mock();
}

int __wrap_cfsetispeed(struct termios *termios_p, speed_t speed)
{
  check_expected(termios_p);
  check_expected(speed);

  return mock();
}

int __wrap_cfsetospeed(struct termios *termios_p, speed_t speed)
{
  check_expected(termios_p);
  check_expected(speed);

  return mock();
}

int __wrap_cfsetspeed(struct termios *termios_p, speed_t speed)
{
  check_expected(termios_p);
  check_expected(speed);

  return mock();
}

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

#define test_termios_databits(x, result, err) \
static void test_termios_databits_##x(void **state) \
{ \
  int expected = err; \
  int actual; \
  tcflag_t cflag = UINT_MAX; \
 \
  actual = int_to_charsize(x, &cflag); \
 \
  assert_int_equal(expected, actual); \
  assert_int_equal(cflag, result); \
}

test_termios_databits(0, UINT_MAX, -2);
test_termios_databits(4, UINT_MAX, -2);
test_termios_databits(5, 0xffffffcf, 0);
test_termios_databits(6, 0xffffffdf, 0);
test_termios_databits(7, 0xffffffef, 0);
test_termios_databits(8, 0xffffffff, 0);
test_termios_databits(9, UINT_MAX, -2);
test_termios_databits(0xffffffff, UINT_MAX, -2);

static void test_termios_speed(void **state)
{
  uint8_t expected = 0;
  uint8_t actual;

  expect_value(__wrap_tcgetattr, fd, 1337);
  expect_not_value(__wrap_tcgetattr, termios_p, NULL);
  will_return(__wrap_tcgetattr, IUTF8|IXON|ICRNL); /* c_iflag */
  will_return(__wrap_tcgetattr, OPOST|ONLCR); /* c_oflag */
  will_return(__wrap_tcgetattr, HUPCL|CREAD|CS8|B38400); /* c_cflag */
  will_return(__wrap_tcgetattr,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG); /* c_lflag */
  will_return(__wrap_tcgetattr, 0); /* c_line */
  will_return(__wrap_tcgetattr,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  /* c_cc */
  will_return(__wrap_tcgetattr, B9600); /* c_ispeed */
  will_return(__wrap_tcgetattr, B9600); /* c_ospeed */
  will_return(__wrap_tcgetattr, 0);

  expect_value(__wrap_tcsetattr, fd, 1337);
  expect_value(__wrap_tcsetattr, optional_actions, TCSANOW);
  expect_not_value(__wrap_tcsetattr, termios_p, NULL);
  expect_value(__wrap_tcsetattr, c_iflag, IUTF8|IXON|ICRNL);
  expect_value(__wrap_tcsetattr, c_oflag, OPOST|ONLCR);
  expect_value(__wrap_tcsetattr, c_cflag, CLOCAL|HUPCL|CREAD|CS8|B115200);
  expect_value(__wrap_tcsetattr, c_lflag,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG);
  expect_value(__wrap_tcsetattr, c_line, 0);
  expect_string(__wrap_tcsetattr, c_cc,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  expect_value(__wrap_tcsetattr, c_ispeed, B115200);
  expect_value(__wrap_tcsetattr, c_ospeed, B115200);
  will_return(__wrap_tcsetattr, 0);


  actual = termios_params(1337, 115200, DIR_BOTH, 0, '\0', 0);

  assert_int_equal(expected, actual);
}

static void test_termios_ispeed(void **state)
{
  uint8_t expected = 0;
  uint8_t actual;

  expect_value(__wrap_tcgetattr, fd, 1337);
  expect_not_value(__wrap_tcgetattr, termios_p, NULL);
  will_return(__wrap_tcgetattr, IUTF8|IXON|ICRNL); /* c_iflag */
  will_return(__wrap_tcgetattr, OPOST|ONLCR); /* c_oflag */
  will_return(__wrap_tcgetattr, HUPCL|CREAD|CS8|B38400); /* c_cflag */
  will_return(__wrap_tcgetattr,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG); /* c_lflag */
  will_return(__wrap_tcgetattr, 0); /* c_line */
  will_return(__wrap_tcgetattr,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  /* c_cc */
  will_return(__wrap_tcgetattr, B9600); /* c_ispeed */
  will_return(__wrap_tcgetattr, B9600); /* c_ospeed */
  will_return(__wrap_tcgetattr, 0);

  expect_value(__wrap_tcsetattr, fd, 1337);
  expect_value(__wrap_tcsetattr, optional_actions, TCSANOW);
  expect_not_value(__wrap_tcsetattr, termios_p, NULL);
  expect_value(__wrap_tcsetattr, c_iflag, IUTF8|IXON|ICRNL);
  expect_value(__wrap_tcsetattr, c_oflag, OPOST|ONLCR);
  expect_value(__wrap_tcsetattr, c_cflag, CLOCAL|HUPCL|CREAD|CS8|B115200);
  expect_value(__wrap_tcsetattr, c_lflag,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG);
  expect_value(__wrap_tcsetattr, c_line, 0);
  expect_string(__wrap_tcsetattr, c_cc,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  expect_value(__wrap_tcsetattr, c_ispeed, B115200);
  expect_value(__wrap_tcsetattr, c_ospeed, B9600);
  will_return(__wrap_tcsetattr, 0);


  actual = termios_params(1337, 115200, DIR_INPUT, 0, '\0', 0);

  assert_int_equal(expected, actual);
}

static void test_termios_ospeed(void **state)
{
  uint8_t expected = 0;
  uint8_t actual;

  expect_value(__wrap_tcgetattr, fd, 1337);
  expect_not_value(__wrap_tcgetattr, termios_p, NULL);
  will_return(__wrap_tcgetattr, IUTF8|IXON|ICRNL); /* c_iflag */
  will_return(__wrap_tcgetattr, OPOST|ONLCR); /* c_oflag */
  will_return(__wrap_tcgetattr, HUPCL|CREAD|CS8|B38400); /* c_cflag */
  will_return(__wrap_tcgetattr,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG); /* c_lflag */
  will_return(__wrap_tcgetattr, 0); /* c_line */
  will_return(__wrap_tcgetattr,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  /* c_cc */
  will_return(__wrap_tcgetattr, B9600); /* c_ispeed */
  will_return(__wrap_tcgetattr, B9600); /* c_ospeed */
  will_return(__wrap_tcgetattr, 0);

  expect_value(__wrap_tcsetattr, fd, 1337);
  expect_value(__wrap_tcsetattr, optional_actions, TCSANOW);
  expect_not_value(__wrap_tcsetattr, termios_p, NULL);
  expect_value(__wrap_tcsetattr, c_iflag, IUTF8|IXON|ICRNL);
  expect_value(__wrap_tcsetattr, c_oflag, OPOST|ONLCR);
  expect_value(__wrap_tcsetattr, c_cflag, CLOCAL|HUPCL|CREAD|CS8|B115200);
  expect_value(__wrap_tcsetattr, c_lflag,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG);
  expect_value(__wrap_tcsetattr, c_line, 0);
  expect_string(__wrap_tcsetattr, c_cc,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  expect_value(__wrap_tcsetattr, c_ispeed, B9600);
  expect_value(__wrap_tcsetattr, c_ospeed, B115200);
  will_return(__wrap_tcsetattr, 0);

  actual = termios_params(1337, 115200, DIR_OUTPUT, 0, '\0', 0);

  assert_int_equal(expected, actual);
}

static void test_termios_invalid_speed(void **state)
{
  uint8_t expected = -1;
  uint8_t actual;

  actual = termios_params(1337, -1, DIR_OUTPUT, 0, '\0', 0);

  assert_int_equal(expected, actual);
}

static void test_termios_wrong_dir(void **state)
{
  uint8_t expected = -4;
  uint8_t actual;

  expect_value(__wrap_tcgetattr, fd, 1337);
  expect_not_value(__wrap_tcgetattr, termios_p, NULL);
  will_return(__wrap_tcgetattr, IUTF8|IXON|ICRNL); /* c_iflag */
  will_return(__wrap_tcgetattr, OPOST|ONLCR); /* c_oflag */
  will_return(__wrap_tcgetattr, HUPCL|CREAD|CS8|B38400); /* c_cflag */
  will_return(__wrap_tcgetattr,
      IEXTEN|ECHOKE|ECHOCTL|ECHOK|ECHOE|ECHO|ICANON|ISIG); /* c_lflag */
  will_return(__wrap_tcgetattr, 0); /* c_line */
  will_return(__wrap_tcgetattr,
      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  /* c_cc */
  will_return(__wrap_tcgetattr, B9600); /* c_ispeed */
  will_return(__wrap_tcgetattr, B9600); /* c_ospeed */
  will_return(__wrap_tcgetattr, 0);

  actual = termios_params(1337, 9600, DIR_BOTH+1, 0, '\0', 0);

  assert_int_equal(expected, actual);
}

static void test_termios_do_nothing(void **state)
{
  uint8_t expected = -2;
  uint8_t actual;

  actual = termios_params(1337, 0, DIR_UNDEFINED, 0, '\0', 0);

  assert_int_equal(expected, actual);
}

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
    cmocka_unit_test(test_termios_databits_0),
    cmocka_unit_test(test_termios_databits_4),
    cmocka_unit_test(test_termios_databits_5),
    cmocka_unit_test(test_termios_databits_6),
    cmocka_unit_test(test_termios_databits_7),
    cmocka_unit_test(test_termios_databits_8),
    cmocka_unit_test(test_termios_databits_9),
    cmocka_unit_test(test_termios_databits_0xffffffff),
    cmocka_unit_test(test_termios_speed),
    cmocka_unit_test(test_termios_ispeed),
    cmocka_unit_test(test_termios_ospeed),
    cmocka_unit_test(test_termios_invalid_speed),
    cmocka_unit_test(test_termios_wrong_dir),
    cmocka_unit_test(test_termios_do_nothing),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
