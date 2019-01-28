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
#ifndef LOGGER_H
#define LOGGER_H

#define LEVELOPT(name) {.value = LEVEL_##name, .text = #name}

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

/**
 * \brief Set logging verbosity to numeric value
 *
 * \param level level of verbosity to set
 *
 * \return error code
 * \retval 0 no error
 * \retval 1 error occurred
 */
int set_numeric_log_level(level_t level);

/**
 * \brief Set logging verbosity to named value
 *
 * \param level level of verbosity to set as either ASCII number or name
 *
 * \return error code
 * \retval 0 no error
 * \retval 1 error occurred
 */
int set_log_level(const char *level);

#endif // LOGGER_H
