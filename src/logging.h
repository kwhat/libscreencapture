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

#ifndef _included_logger
#define _included_logger

#include <screencapture.h>

#ifndef __FUNCTION__
#define __FUNCTION__ __func__
#endif

#define log_debug(...) log_callback(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define log_info(...)  log_callback(LOG_LEVEL_INFO,  __VA_ARGS__)
#define log_warn(...)  log_callback(LOG_LEVEL_WARN,  __VA_ARGS__)
#define log_error(...) log_callback(LOG_LEVEL_ERROR, __VA_ARGS__)

// screen_log_callback_t(level, message)
extern screen_log_callback_t log_callback;

#endif
