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
#include <endian.h>
#include <errno.h>

#include "logger.h"
#include "mh_uart.h"

pkt_t init_read_gas_packet()
{
  pkt_t result;
  read_gas_t packet = {
    .start = 0xff,
    .sensor = 1,
    .command = CMD_GAS_CONCENTRATION,
    .reserved = {0,0,0,0,0},
  };

  packet.checksum = checksum((pkt_t*)&packet);
  result = *((pkt_t*) &packet);

  return result;
}

pkt_t init_calibrate_span_packet(uint16_t span_point)
{
  pkt_t result;
  calibrate_span_t packet = {
    .start = 0xff,
    .sensor = 1,
    .command = CMD_CALIBRATE_SPAN,
    .span_point = htobe16(span_point),
    .reserved = {0,0,0},
  };

  packet.checksum = checksum((pkt_t*)&packet);
  result = *((pkt_t*) &packet);

  return result;
}

pkt_t init_calibrate_zero_packet()
{
  pkt_t result;
  calibrate_zero_t packet = {
    .start = 0xff,
    .sensor = 1,
    .command = CMD_CALIBRATE_ZERO,
    .reserved = {0,0,0,0,0},
  };

  packet.checksum = checksum((pkt_t*)&packet);
  result = *((pkt_t*) &packet);

  return result;
}

uint16_t return_gas_concentration(pkt_t packet)
{
  return_gas_t return_packet = *((return_gas_t*) &packet);

  /* verify packet contents */
  if (return_packet.start != 0xff)
  {
    INFO("Expected 0xff as first byte of packet, 0x%x given",
        return_packet.start);
    errno = EINVAL;
    return (uint16_t)-1;
  }
  if (return_packet.command != CMD_GAS_CONCENTRATION)
  {
    INFO("Expected return packet for gas concentration, 0x%x given",
        return_packet.command);
    errno = EINVAL;
    return (uint16_t)-1;
  }
  if (checksum(&packet) != return_packet.checksum)
  {
    INFO("Expected checksum was 0x%x, 0x%x given", checksum(&packet),
        return_packet.checksum);
    errno = EINVAL;
    return (uint16_t)-1;
  }

  return be16toh(return_packet.concentration);
}

uint8_t checksum(pkt_t *packet)
{
  uint8_t cs = 0xff;
  int i;

  if (packet == NULL)
  {
    WARNING("No packet given, checksum cannot be computed");
    return cs;
  }

  for (i = 0; i < sizeof(packet->reserved); i++)
  {
    cs -= packet->reserved[i];
  }
  cs++;
  return cs;
}
