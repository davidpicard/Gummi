/**
 * @file     utils.c
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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "utils.h"

static int slog_debug = 0;

void slog_init(int debug) {
    slog_debug = debug;
}

void slog(int level, const char *fmt, ...) {
    va_list vap;

    if (L_IS_TYPE(level, L_DEBUG)) {
        if (slog_debug) {
            fprintf(stderr, "[Debug] ");
            va_start(vap, fmt);
            vfprintf(stdout, fmt, vap);
            va_end(vap);
        }
    } else {
        /* TODO: use autotools macro 'PACKAGE' for program name*/
        fprintf(stderr, "[gummi] ");
        va_start(vap, fmt);
        vfprintf(stderr, fmt, vap);
        va_end(vap);
    }

    if (!L_IS_TYPE(level, L_INFO) && !L_IS_TYPE(level, L_DEBUG))
        exit(1);
}
