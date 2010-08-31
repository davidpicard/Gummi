/**
 * @file   utils.h
 * @brief  utility for gummi
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef GUMMI_UTILS
#define GUMMI_UTILS

#define L_IS_TYPE(level, type) ((level & type) == type)
#define L_IS_GUI(level) (level & 0xf0)
#define L_INFO     0x01    /* for informative messages */
#define L_DEBUG    0x02    /* debug messages, only print if -d flags is used */
#define L_FATAL    0x04    /* for fatal errors, program will exit */
#define L_G_INFO   0x10    /* GUI info */
#define L_G_ERROR  0x20    /* recoverable error */
#define L_G_FATAL  0x40    /* inrecoverable error */

/**
 * @brief slog initlization
 * @param debug boolean value for whether output debug message or not
 */
void slog_init(int debug);

/**
 * @brief debug interface for gummi
 * @param level levels or listed above
 * @param fmt just like the printf function
 * @param ... parameters for fmt
 */
void slog(int level, const char *fmt, ...);

gboolean gummi_yes_no_dialog(const gchar* message);

#endif /* GUMMI_UTILS */
