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

int process_command(mhopt_t *opts)
{
  return -1;
}
