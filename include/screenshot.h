/* libScreenShot: Cross-platfrom fast screen capture library.
 * Copyright (C) 2014 Alexander Barker.  All Rights Received.
 * https://github.com/kwhat/libscreenshot/
 *
 * libScreenShot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libScreenShot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SCREENSHOT_H
#define __SCREENSHOT_H

/* Begin Error Codes */
#define SCREENSHOT_SUCCESS							0x00
#define SCREENSHOT_FAILURE							0x01

// System level errors.
#define SCREENSHOT_ERROR_OUT_OF_MEMORY				0x02
/* End Error Codes */

typedef enum _log_level {
	LOG_LEVEL_DEBUG = 1,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR
} log_level;

typedef bool (*logger_t)(unsigned int, const char *, ...);

/* Begin Data Structures */
typedef struct _pixel {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} pixel;

typedef struct _screenshot {
	unsigned short width;
	unsigned short height;
	pixel *data;
} screenshot;
/* End Data Structures */

#ifdef _WIN32
#define SCREENSHOT_API __declspec(dllexport)
#else
#define SCREENSHOT_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

	// Set the logger callback functions.
	SCREENSHOT_API void sc_set_logger_proc(logger_t logger_proc);

	SCREENSHOT_API int sc_set_engine();

	SCREENSHOT_API int sc_capture();

#ifdef __cplusplus
}
#endif

#endif
