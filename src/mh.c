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
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#include "mh.h"

speedopt_t speeds[] = {
  speed(0), speed(50), speed(75), speed(110), speed(134), speed(150),
  speed(200), speed(300), speed(600), speed(1200), speed(1800), speed(2400),
  speed(4800), speed(9600), speed(19200), speed(38400), speed(57600),
  speed(115200), speed(230400), speed(460800), speed(500000), speed(576000),
  speed(921600), speed(1000000), speed(1152000), speed(1500000), speed(2000000),
  speed(2500000), speed(3000000), speed(3500000), speed(4000000),
};

tcflag_t databitopts[] = {-1, -1, -1, -1, -1, CS5, CS6, CS7, CS8};

tcflag_t parityopts[] = {
  ['N'] = (1<<31),
  ['E'] = (1<<31)|PARENB,
  ['O'] = (1<<31)|PARENB|PARODD,
  ['S'] = (1<<31),
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
  tcflag_t bits;

  if (cflags == NULL)
  {
    return -1;
  }

  /* check if there is opt for given param */
  if (((uint8_t) databits) >= sizeof(databitopts)/sizeof(tcflag_t))
  {
    return -2;
  }

  bits = databitopts[databits];

  /* if databitopt is equal -1 it is unsupported, same way as when out of
   * bounds */
  if (bits == -1)
  {
    return -2;
  }

  *cflags &= ~CSIZE;
  *cflags |= bits;

  return 0;
}

int char_to_parity(char parity, tcflag_t *cflags)
{
  const tcflag_t PARITYBITS = PARENB|PARODD;

  if (cflags == NULL)
  {
    return -1;
  }

  if (islower(parity))
  {
    parity = toupper(parity);
  }

  if ((parityopts[parity] & (1<<31)) == 0)
  {
    /* unsupported */
    return -2;
  }

  *cflags &= ~PARITYBITS;
  *cflags |= parityopts[parity] & PARITYBITS;

  return 0;
}

int int_to_stopbits(int stopbits, tcflag_t *cflags)
{
  if (cflags == NULL)
  {
    return -1;
  }

  switch (stopbits)
  {
    case 10: *cflags &= ~CSTOPB; break;
    case 20: *cflags |= CSTOPB; break;
    default:
      return -2;
  }

  return 0;
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

  /* set baudrate for selected direction */
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

  /* set data bits */
  if (databits != 0)
  {
    if (err = int_to_charsize(databits, &options.c_cflag))
    {
      return -6;
    }
  }

  /* set parity */
  if (parity != '\0')
  {
    if (err = char_to_parity(parity, &options.c_cflag))
    {
      return -7;
    }
  }
  /* set stop bits */
  if (stopbits != 0)
  {
    if (err = int_to_stopbits(stopbits, &options.c_cflag))
    {
      return -8;
    }
  }

  options.c_cflag |= (CLOCAL | CREAD);

  if (err = tcsetattr(fd, TCSANOW, &options))
  {
    perror("tcsetattr");
    return -9;
  }

  return 0;
}

int process_command(mhopt_t *opts)
{
  int err = 0;
  int fd = -1;
  pkt_t packet;
  size_t left = 0;
  size_t processed = 0;

  if ((fd = open(opts->device, O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
  {
    perror("open");
    return -1;
  }

  if (err = termios_params(
        fd, /* file descriptor */
        opts->baudrate,
        DIR_BOTH, /* direction */
        opts->databits,
        opts->parity,
        opts->stopbits))
  {
    err = -2; goto error;
  }

  switch (opts->command)
  {
    case CMD_GAS_CONCENTRATION:
      /* write request */
      packet = init_read_gas_packet();
      left = sizeof(packet);
      while (left > 0)
      {
        processed = write(fd, ((void*) &packet) + sizeof(packet) - left, left);
        if (processed == -1)
        {
          if (errno == EAGAIN)
          {
            continue;
          }
          perror("write");
          err = -3; goto error;
        }
        left -= processed;
      }

      /* read response */
      left = sizeof(packet);
      while (left > 0)
      {
        processed = read(fd, ((void*) &packet) + sizeof(packet) - left, left);
        if (processed == -1)
        {
          if (errno == EAGAIN)
          {
            continue;
          }
          perror("read");
          err = -4; goto error;
        }
        left -= processed;
      }
      opts->gas_concentration = return_gas_concentration(packet);
      break;
    default:
        err = -5; goto error;
  }

  close(fd);
  return 0;
error:
  close(fd);
  return err;
}
