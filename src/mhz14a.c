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
#include <ctype.h>

#include "mhz14a.h"
#include "mh_uart.h"
#include "mh.h"
#include "logger.h"
#include "config.h"

#define OPT_LOG (CHAR_MAX + 1)

void help(char usage, char *progname)
{
  printf("Usage: %s [-b BAUD] [-m DPS] [-d FILE] [-r | -z | -s SPAN] | -v | -h\n",
      progname);
  if (!usage)
  {
    printf("\n"
        "  -r, --read          read sensor data[ppm]\n"
        "  -z, --zero          calibrate zero point\n"
        "  -s, --span=SPAN     calibrate span point at SPAN\n"
        "  -b, --baud=BAUDRATE set baudrate to BAUDRATE (default: 9600)\n"
        "  -m, --mode=DPS      set mode to D-databits, P-parity and S-stopbits\n"
        "                      (default: 8N1)\n"
        "  -d, --dev=DEVICE    set device at which sensor can be found\n"
        "                      (default: /dev/ttyS0)\n"
        "  -t,--timeout=SEC    set number of seconds before timeout to SEC (default:\n"
        "                      0 - infinity)\n"
        "  -T,--times=TRIES    set number of tries to TRIES (default: 1 - no retry)\n"
        "      --log=LEVEL     set logging verbosity to LEVEL (default: 0 - error)\n"
        "                      One of the following is allowed (either number or text):\n"
        "                        0/ERROR; 1/WARNING; 2/INFO; 3/DEBUG\n"
        "  -v, --version       print version of the program and exit\n"
        "  -h, --help          print this help information and exit\n"
        "\n");
  }
}

int main(int argc, char **argv)
{
  int c;
  int digit_optind = 0;
  optind = 0;
  optarg = NULL;
  mhopt_t opts = {
    .device = NULL,
    .baudrate = 9600,
    .databits = 8,
    .parity = 'N',
    .stopbits = 10,
    .command = 0,
    .gas_concentration = 0,
    .span_point = 0,
    .timeout = 0,
    .tries = 1,
  };
  int result;

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
      {"timeout", required_argument, 0, 't' },
      {"times", required_argument, 0, 'T' },
      {"log", required_argument, 0, OPT_LOG },
      {"version", no_argument, 0, 'v' },
      {"help", no_argument, 0, 'h' },
      {0, 0, 0, 0 }
    };

    c = getopt_long(argc, argv, "b:m:d:rzs:t:T:vh",
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
        if (strlen(optarg) != 3 ||
            !isdigit(optarg[0]) ||
            (!isupper(optarg[1]) && !islower(optarg[1])) ||
            !isdigit(optarg[2]))
        {
          ERROR("Unsupported mode");
          return RET_MODE_ERR;
        }

        opts.databits = optarg[0] - '0';
        opts.parity = optarg[1];
        opts.stopbits = (optarg[2] - '0') * 10; // TODO: scanf to float

        break;

      case 'd':
        /* --device=FILE */
        opts.device = strdup(optarg); // TODO: call free()
        break;

      case 'r':
        /* --read */
        if (opts.command != 0)
        {
          ERROR("more than one command given");
          return RET_CMD_DUPL;
        }

        opts.command = CMD_GAS_CONCENTRATION;
        break;

      case 's':
        /* --span=SPANPOINT */
        if (opts.command != 0)
        {
          ERROR("more than one command given");
          return RET_CMD_DUPL;
        }

        opts.command = CMD_CALIBRATE_SPAN;
        opts.span_point = atol(optarg);
        break;

      case 'z':
        /* --zero */
        if (opts.command != 0)
        {
          ERROR("more than one command given");
          return RET_CMD_DUPL;
        }

        opts.command = CMD_CALIBRATE_ZERO;
        break;

      case 't':
        /* --timeout=SEC */
        opts.timeout = atol(optarg); // TODO: maybe safer ?
        break;

      case 'T':
        /* --times=TRIES */
        opts.tries = atol(optarg); // TODO: maybe safer ?
        break;

      case OPT_LOG:
        /* --log */
        if (set_log_level(optarg))
        {
          ERROR("unknown log level: %s", optarg);
          return RET_ARG;
        }
        break;

      case 'v':
        /* --version */
        printf("mh-z14a version %s\n", MHZ14A_VERSION);
        return RET_SUCCESS;

      case 'h':
        /* --help */
        help(0, argv[0]);
        return RET_SUCCESS;

      case '?':
        /* -* not in optstring */
        return RET_UNPARSED;

      default:
        /* optstrings not handled (mistakes?) */
        WARNING("getopt returned character code 0%o", c);
    }
  }

  /* positional arguments */
  if (optind < argc) {
    ERROR("too many arguments provided!");
    return RET_UNPARSED;
  }

  /* check if command was already given */
  if (opts.command == 0)
  {
    ERROR("no command given");
    return RET_NOCMD;
  }

  result = process_command(&opts);
  if (result != 0)
  {
    ERROR("Execution returned %d", result);
  }
  else
  {
    switch (opts.command)
    {
      case CMD_GAS_CONCENTRATION:
        printf("%d\n", opts.gas_concentration);
        break;
      case CMD_CALIBRATE_SPAN:
      case CMD_CALIBRATE_ZERO:
        break;
      case CMD_SWITCH_ABC:
      case CMD_SET_RANGE:
        ERROR("Not implemented");
        return 42;
      default:
        ERROR("Internal error on command %x", opts.command);
        return RET_INTERNAL;
    }
  }

  return result;
}
