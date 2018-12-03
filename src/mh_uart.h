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
#ifndef MH_UART_H
#define MH_UART_H
#include <stdint.h>

#define __packed__ __attribute__ ((packed))

typedef enum {
  CMD_GAS_CONCENTRATION = 0x86,
  CMD_CALIBRATE_ZERO = 0x87,
  CMD_CALIBRATE_SPAN = 0x88,
} command_t;

typedef struct {
  uint8_t start;
  uint8_t reserved[7];
  uint8_t checksum;
} __packed__ pkt_t;

typedef struct {
  uint8_t start;
  uint8_t sensor;
  uint8_t reserved[6];
  uint8_t checksum;
} __packed__ sendpkt_t;

typedef struct {
  uint8_t start;
  uint8_t command;
  uint8_t reserved[6];
  uint8_t checksum;
} __packed__ returnpkt_t;

typedef struct {
  uint8_t start;
  uint8_t sensor;
  uint8_t command;
  uint8_t reserved[5];
  uint8_t checksum;
} __packed__ read_gas_t;

typedef struct {
  uint8_t start;
  uint8_t sensor;
  uint8_t command;
  uint8_t reserved[5];
  uint8_t checksum;
} __packed__ calibrate_zero_t;

typedef struct {
  uint8_t start;
  uint8_t sensor;
  uint8_t command;
  uint16_t span_point; /* big endian */
  uint8_t reserved[3];
  uint8_t checksum;
} __packed__ calibrate_span_t;

typedef struct {
  uint8_t start;
  uint8_t command;
  uint16_t concentration; /* big endian */
  uint8_t reserved[4];
  uint8_t checksum;
} __packed__ return_gas_t;

/**
 * \brief Create packet for reading gas ready to be sent
 *
 * \return complete packet
 */
pkt_t init_read_gas_packet();

/**
 * \brief Create packet for calibrating span point
 *
 * \param span_point Span point to calibrate
 *
 * \return complete packet
 */
pkt_t init_calibrate_span_packet(uint16_t span_point);

/**
 * \brief Create packet for calibrating zero point
 *
 * \return complete packet
 */
pkt_t init_calibrate_zero_packet();

/**
 * \brief Compute checksum of a packet
 *
 * \param packet packet of which checksum is computed
 *
 * \return checksum
 */
uint8_t checksum(pkt_t *packet);
#endif // MH_UART_H
