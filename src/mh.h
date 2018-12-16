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
#ifndef MH_H
#define MH_H

#include <stdint.h>
#include <termios.h>

#include "mh_uart.h"

#define speed(baudrate) { baudrate, B##baudrate }

typedef struct {
  char *device; /*< filename of UART device */
  int baudrate; /*< baudrate (usually 9600) */
  uint8_t databits; /*< number of data bits (usually 8) */
  uint8_t parity; /*< parity bit (usually no parity) */
  uint8_t stopbits; /*< number of stop bits (usually 1) */
  command_t command; /*< command to execute on sensor (depending on command
                      *  parameter has to be filled) */
  uint16_t gas_concentration; /*< output - gas concentration if read command
                               *  selected */
  uint16_t span_point; /*< input - span point to set if calibrate span point
                        *  selected */
} mhopt_t;

typedef enum {
  DIR_UNDEFINED = 0,
  DIR_INPUT,
  DIR_OUTPUT,
  DIR_BOTH,
} direction_t;

typedef struct {
  int baudrate;
  speed_t speed;
} speedopt_t;

int process_command(mhopt_t *opts);

/**
 * \brief Convert baudrate as integer into termios bitfield
 *
 * \param baud baudrate as integer number of bauds per second
 *
 * \return bitfield under CBAUD mask
 */
speed_t int_to_baud(int baud);

/**
 * \brief Set baudrate of descriptor
 *
 * \param fd file descriptor which speed is to be adjusted
 * \param baud baudrate as number (only specific values are supported)
 * \param dir direction at which speed is to be adjusted
 *
 * \return success indicator
 * \retval 0 success
 * \retval -1 error occurred
 */
int termios_speed(int fd, int baud, direction_t dir);

/**
 * \brief Set terminal parameters
 *
 * \param fd file descriptor which parameters will be adjusted
 * \param databits number of data bits (usually 8)
 * \param parity meaning of parity bit
 * \param stopbits number of stop bits (usually 1)
 *
 * \return error indicator
 * \retval 0 success
 * \retval -1 error occurred
 */
int termios_params(int fd, uint8_t databits, char parity, uint8_t stopbits);

#endif // MH_H
