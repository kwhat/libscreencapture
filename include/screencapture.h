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

#ifndef __SCREENCAPTURE_H
#define __SCREENCAPTURE_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

/* Begin Log Levels and Function Prototype */
typedef enum _screen_log_level_t {
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} screen_log_level_t;

// Logging callback function prototype.
typedef bool (*screen_log_callback_t)(screen_log_level_t, const char *, ...);
/* End Log Levels and Function Prototype */

/* Begin Data Structures */
typedef struct _screen_data_t {
    uint8_t number;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} screen_data_t;
/* End Data Structures */

#ifdef _WIN32
#define SCREENCAPTURE_API __declspec(dllexport)
#else
#define SCREENCAPTURE_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    SCREENCAPTURE_API screen_data_t* screen_info(int8_t *count);

    SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height);

    SCREENCAPTURE_API void screen_set_log_callback(screen_log_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif
