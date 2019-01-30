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
#include <sys/select.h>

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
  DEBUG("Unsupported baudrate: %d", baud);
  errno = ENOTSUP;
  return -1;
}

int int_to_charsize(int databits, tcflag_t *cflags)
{
  tcflag_t bits;

  if (cflags == NULL)
  {
    DEBUG("Nowhere to write character size to");
    return -1;
  }

  /* check if there is opt for given param */
  if (((uint8_t) databits) >= sizeof(databitopts)/sizeof(tcflag_t))
  {
    DEBUG("Num of databits greater than maximum supported value: %d", databits);
    return -2;
  }

  bits = databitopts[databits];

  /* if databitopt is equal -1 it is unsupported, same way as when out of
   * bounds */
  if (bits == -1)
  {
    DEBUG("Unsupported data bit count: %d", databits);
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
    DEBUG("Nowhere to write parity indicator to");
    return -1;
  }

  if (islower(parity))
  {
    parity = toupper(parity);
  }

  if ((parityopts[parity] & (1<<31)) == 0)
  {
    DEBUG("Unsupported parity: %c", parity);
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
    DEBUG("Nowhere to write stop bit size to");
    return -1;
  }

  switch (stopbits)
  {
    case 10: *cflags &= ~CSTOPB; break;
    case 20: *cflags |= CSTOPB; break;
    default:
      DEBUG("%d.%d stop bits are not supported", stopbits / 10, stopbits % 10);
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
    DEBUG("Neither direction, nor data, nor parity, nor stop bits "
        "are to be set");
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
      DEBUG("Unsupported direction");
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
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  if (err = tcsetattr(fd, TCSANOW, &options))
  {
    perror("tcsetattr");
    return -9;
  }

  return 0;
}

ssize_t perform_io(io_func_t func, int fd, void *buf, size_t count,
    int timeout)
{
  size_t left = 0;
  size_t processed = 0;
  fd_set fds;
  fd_set *rfd = NULL, *wfd = NULL, *efd = NULL;
  struct timeval tv;
  int result = -1;

  left = count;
  while (left > 0)
  {
    if (timeout != 0)
    {
      /* select descriptor to ensure that it will not block */
      tv.tv_sec = timeout;
      tv.tv_usec = 0;

      FD_ZERO(&fds);
      FD_SET(fd, &fds);

      if (func == &read)
      {
        rfd = &fds;
      }
      else if (func == &write)
      {
        wfd = &fds;
      }
      else
      {
        errno = ENOTSUP;
        return (ssize_t)-1;
      }

      result = select(fd + 1, rfd, wfd, efd, &tv);
      if (result == -1)
      {
        perror("select");
        return (ssize_t)-1;
      }
      else if (!result)
      {
        /* timeout */
        errno = ENODATA;
        return (ssize_t)-1;
      }
    }
    processed = func(fd, buf + count - left, left);
    if (processed == -1)
    {
      if (errno == EAGAIN)
      {
        continue;
      }
      return (ssize_t)-1;
    }
    left -= processed;
  }
  return count - left;
}

int process_command(mhopt_t *opts)
{
  int err = 0;
  int fd = -1;
  pkt_t packet;
  size_t left = 0;
  size_t processed = 0;
  uint16_t result = (uint16_t)-1;
  int tries = 0;

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
      tries = opts->tries;
      while (tries--)
      {
        err = perform_io((io_func_t) write, fd, &packet, sizeof(packet),
            opts->timeout);
        if (err != sizeof(packet))
        {
          perror("write");
          continue;
        }

      /* read response */
        err = perform_io((io_func_t) read, fd, &packet, sizeof(packet),
            opts->timeout);
        if (err != sizeof(packet))
        {
          perror("read");
          continue;
        }
        break;
      }
      if (err != sizeof(packet))
      {
        err = -3; goto error;
      }
      if (!tries)
      {
        err = -4; goto error;
      }

      /* parse response */
      result = return_gas_concentration(packet);
      if (result == (uint16_t)-1)
      {
        perror("return_gas_concentration");
        err = -5; goto error;
      }
      opts->gas_concentration = result;
      break;
    default:
        err = -6; goto error;
  }

  close(fd);
  return 0;
error:
  close(fd);
  return err;
}
