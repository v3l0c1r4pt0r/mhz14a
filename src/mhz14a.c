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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>

#include "mh_uart.h"
#include "mh.h"
#include "config.h"

int main(int argc, char **argv)
{
  int c;
  int digit_optind = 0;
  mhopt_t opts;

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      /* {name, has_arg, flag, val} */
      /* UART options */
      {"baud", required_argument, 0, 'b' },
      {"mode", required_argument, 0, 'm' },
      {"dev", required_argument, 0, 'd' },
      /* MH-Z14A functions */
      {"read", no_argument, 0, 'r' },
      {"zero", no_argument, 0, 'z' },
      {"span", required_argument, 0, 's' },
      /* general */
      {"version", no_argument, 0, 'v' },
      {"help", no_argument, 0, 'h' },
      {0, 0, 0, 0 }
    };

    c = getopt_long(argc, argv, "b:m:d:rzs:vh",
        long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 0:
        /* for default long options
         * (option->flag==0 -> 4th element of struct option) */
        printf("option %s", long_options[option_index].name);
        if (optarg)
          printf(" with arg %s", optarg);
        printf("\n");
        break;

      case '0':
      case '1':
      case '2':
        if (digit_optind != 0 && digit_optind != this_option_optind)
          printf("digits occur in two different argv-elements.\n");
        digit_optind = this_option_optind;
        printf("option %c\n", c);
        break;

      case 'b':
        /* --baud=BAUDRATE */
        opts.baudrate = atol(optarg); // TODO: maybe safer ?
        break;

      case 'm':
        /* --mode=MODE */
        break;

      case 'd':
        /* --device=FILE */
        opts.device = strdup(optarg);
        break;

      case 'r':
        /* --read */
        opts.command = CMD_GAS_CONCENTRATION;
        break;

      case 's':
        /* --span=SPANPOINT */
        opts.command = CMD_CALIBRATE_SPAN;
        opts.span_point = atol(optarg);
        break;

      case 'z':
        /* --zero */
        opts.command = CMD_CALIBRATE_ZERO;
        break;

      case 'v':
        /* --version */
        printf("mh-z14a version %s\n", MHZ14A_VERSION);
        return 0;

      case 'h':
        /* --help */
        printf("Usage: %s -b BAUD | -v | -h\n");
        return 0;

      case '?':
        /* -* not in optstring */
        break;

      default:
        /* optstrings not handled (mistakes?) */
        printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  /* positional arguments */
  if (optind < argc) {
    printf("Error: too many arguments provided!\n");
    return 1;
  }

  return 0;
}
