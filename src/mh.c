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
#include <stddef.h>
#include <errno.h>
#include <termios.h>
#include <stdio.h>

#include "mh.h"

speedopt_t speeds[] = {
  speed(0), speed(50), speed(75), speed(110), speed(134), speed(150),
  speed(200), speed(300), speed(600), speed(1200), speed(1800), speed(2400),
  speed(4800), speed(9600), speed(19200), speed(38400), speed(57600),
  speed(115200), speed(230400), speed(460800), speed(500000), speed(576000),
  speed(921600), speed(1000000), speed(1152000), speed(1500000), speed(2000000),
  speed(2500000), speed(3000000), speed(3500000), speed(4000000),
};

speed_t int_to_baud(int baud)
{
  int i;
  speedopt_t *speed = NULL;
  for (i = 0; i < sizeof(speeds)/sizeof(speedopt_t); i++)
  {
    speed = &speeds[i];
    if (baud == speed->baudrate)
    {
      errno = 0;
      return speed->speed;
    }
  }
  errno = ENOTSUP;
  return -1;
}

int int_to_charsize(int databits, tcflag_t *cflags)
{
  return -1;
}

int char_to_parity(char parity, tcflag_t *cflags)
{
  return -1;
}

int int_to_stopbits(int stopbits, tcflag_t *cflags)
{
  return -1;
}

int termios_params(int fd, int baud, direction_t dir, uint8_t databits,
    char parity, uint8_t stopbits)
{
  int err = 0;
  struct termios options;
  speed_t baudbits = int_to_baud(baud);
  if (baudbits == (speed_t)-1)
  {
    perror("int_to_baud");
    return -1;
  }

  /* check if there is something to do */
  if (dir == DIR_UNDEFINED && databits == 0 && parity == '\0' && stopbits == 0)
  {
    return -2;
  }

  if (err = tcgetattr(fd, &options))
  {
    perror("tcgetattr");
    return -3;
  }

  switch(dir)
  {
    case DIR_INPUT: err = cfsetispeed(&options, baudbits); break;
    case DIR_OUTPUT: err = cfsetospeed(&options, baudbits); break;
    case DIR_BOTH: err = cfsetspeed(&options, baudbits); break;
    case DIR_UNDEFINED: /* do not set speed */ break;
    default:
      errno = ENOTSUP;
      return -4;
  }
  if (err)
  {
    perror("cfsetspeed");
    return -5;
  }

  options.c_cflag |= (CLOCAL | CREAD);

  if (err = tcsetattr(fd, TCSANOW, &options))
  {
    perror("tcsetattr");
    return -6;
  }

  return 0;
}

int process_command(mhopt_t *opts)
{
  return -1;
}
