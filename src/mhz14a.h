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
#ifndef MHZ14A_H
#define MHZ14A_H

#define LEVELOPT(name) {.value = LEVEL_##name, .text = #name}

typedef enum {
  RET_SUCCESS = 0,
  RET_NOCMD,
  RET_CMD_DUPL,
  RET_MODE_ERR,
  RET_ARG,
  RET_UNPARSED,
  RET_INTERNAL = 255
} result_t;

typedef enum {
  LEVEL_ERROR = 0, /**< This is default setting */
  LEVEL_WARNING,
  LEVEL_INFO,
  LEVEL_DEBUG,
  LEVEL_MAX, /**< This is pseudo level - do not use */
} level_t;

typedef struct {
  level_t value;
  char *text;
} levelopt_t;

extern level_t log_level;

#endif // MHZ14A_H
