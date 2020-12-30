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

#include <screencapture.h>
#include <windows.h>

// Structure for the monitor_enum_proc() callback so we can track the count.
typedef struct _monitor_info {
    uint8_t count;
    screen_data *data;
} monitor_info;

/* The following function was contributed by Anthony Liguori Jan 14, 2015.
 * https://github.com/kwhat/libuiohook/pull/17
 *
 * callback function called by EnumDisplayMonitors for each enabled monitor
 * http://msdn.microsoft.com/en-us/library/windows/desktop/dd162610(v=vs.85).aspx
 * http://msdn.microsoft.com/en-us/library/dd162610%28VS.85%29.aspx
 * http://msdn.microsoft.com/en-us/library/dd145061%28VS.85%29.aspx
 * http://msdn.microsoft.com/en-us/library/dd144901(v=vs.85).aspx
 */
static BOOL CALLBACK monitor_enum_proc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    int width  = lprcMonitor->right - lprcMonitor->left;
    int height = lprcMonitor->bottom - lprcMonitor->top;
    int origin_x = lprcMonitor->left;
    int origin_y = lprcMonitor->top;

    if (width > 0 && height > 0) {
        monitor_info *screens = (monitor_info *) dwData;

        if (screens->data == NULL) {
            screens->data = (screen_data *) malloc(sizeof(screen_data));
        } else {
            screens->data = (screen_data *) realloc(screens, sizeof(screen_data) * screens->count);
        }

        screens->data[screens->count++] = (screen_data) {
                // Should monitor count start @ zero? Currently it starts at 1.
                .number = screens->count,
                .x = origin_x,
                .y = origin_y,
                .width = width,
                .height = height
            };

            //logger(LOG_LEVEL_INFO, "%s [%u]: Monitor %d: %ldx%ld (%ld, %ld)\n",
            //        __FUNCTION__, __LINE__, screens->count, width, height, origin_x, origin_y);
    }

    return TRUE;
}

SCREENCAPTURE_API screen_data* screen_info(int8_t *count) {
    *count = 0;

    // Create a simple structure to make working with monitor_enum_proc easier.
    monitor_info screens = {
        .count = 0,
        .data = NULL
    };

    BOOL status = EnumDisplayMonitors(NULL, NULL, monitor_enum_proc, (LPARAM) &screens);

    if (!status || screens.count == 0) {
        // Fallback in case EnumDisplayMonitors fails.
        //logger(LOG_LEVEL_INFO, "%s [%u]: EnumDisplayMonitors failed. Fallback.\n",
        //        __FUNCTION__, __LINE__);

        int width  = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        if (width > 0 && height > 0) {
            screens.data = (screen_data *) malloc(sizeof(screen_data));

            if (screens.data != NULL) {
                *count = 1;
                screens.data[0] = (screen_data) {
                    .number = 1,
                    .x = 0,
                    .y = 0,
                    .width = width,
                    .height = height
                };
            }
        }
    } else {
        // Populate the count.
        *count = screens.count;
    }

    return screens.data;
}
