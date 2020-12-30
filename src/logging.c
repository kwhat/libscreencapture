/* libScreenCapture: Cross-platform screen capture library.
 * Copyright (C) 2006-2020 Alexander Barker.  All Rights Received.
 * https://github.com/kwhat/libscreencapture/
 *
 * libScreenCapture is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libScreenCapture is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logging.h"

static bool default_callback(screen_log_level_t level, const char *format, ...) {
    bool status = false;

    size_t size = 2048;
    char *buffer = malloc(size);
    if (buffer != NULL) {
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);

        const char *level_name;
        switch (level) {
            case LOG_LEVEL_DEBUG:
                level_name = "DEBUG";
                break;

            case LOG_LEVEL_INFO:
                level_name = "INFO";
                break;

            case LOG_LEVEL_WARN:
                level_name = "WARN";
                break;

            case LOG_LEVEL_ERROR:
                level_name = "ERROR";
                break;

            default:
                level_name = "UNKNOWN";
        }

        va_list args;
        va_start(args, format);
        int len = vsnprintf(buffer, size, format, args);
        if (len >= 0) {
            len = printf("%2d:%2d:%2d %.7s %s\n",
                    tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, level_name, buffer);

            status = (len >= 0);
        }
        va_end(args);

        free(buffer);
    }

    return status;
}

// Current logger function pointer, should never be null.
screen_log_callback_t log_callback = &default_callback;

SCREENCAPTURE_API void screen_set_log_callback(screen_log_callback_t callback) {
    if (callback == NULL) {
        log_callback = &default_callback;
    } else {
        log_callback = callback;
    }
}
