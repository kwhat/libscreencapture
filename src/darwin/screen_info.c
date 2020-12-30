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

#include <CoreGraphics/CoreGraphics.h>
#include <screencapture.h>
#include <stdlib.h>

SCREENCAPTURE_API screen_data* screen_info(int8_t *count) {
    CGError status = kCGErrorFailure;
    screen_data* screens = NULL;

    // Initialize count to zero.
    *count = 0;

    // Allocate memory to hold each display id.
    // If you have more than 32 monitors, send me a picture and make a donation ;)
    CGDirectDisplayID *display_ids = malloc(sizeof(CGDirectDisplayID) * 32);
    if (display_ids != NULL) {
        // NOTE Pass UCHAR_MAX to make sure uint32_t doesn't overflow uint8_t.
        // TOOD Test/Check whether CGGetOnlineDisplayList is more suitable...
        status = CGGetActiveDisplayList(UCHAR_MAX, display_ids, (uint32_t *) count);

        // If there is no error and at least one monitor.
        if (status == kCGErrorSuccess && *count > 0) {
//            logger(LOG_LEVEL_DEBUG, "%s [%u]: CGGetActiveDisplayList: %li.\n",
//                    __FUNCTION__, __LINE__, *count);

            // Allocate memory for the number of screens found.
            screens = malloc(sizeof(screen_data) * (*count));
            if (screens != NULL) {
                for (uint8_t i = 0; i < *count; i++) {
                    //size_t width = CGDisplayPixelsWide(display_ids[i]);
                    //size_t height = CGDisplayPixelsHigh(display_ids[i]);
                    CGRect boundsDisp = CGDisplayBounds(display_ids[i]);
                    if (boundsDisp.size.width > 0 && boundsDisp.size.height > 0) {
                        screens[i] = (screen_data) {
                            .number = i + 1,
                            //TODO: make sure we follow the same convention for the origin
                            //in all other platform implementations (upper-left)
                            //TODO: document the approach with examples in order to show different
                            //cases -> different resolutions (secondary monitors origin might be
                            //negative)
                            .x = boundsDisp.origin.x,
                            .y = boundsDisp.origin.y,
                            .width = boundsDisp.size.width,
                            .height = boundsDisp.size.height
                        };
                }
                }
            }
        } else {
//            logger(LOG_LEVEL_DEBUG, "%s [%u]: multiple_get_screen_info failed: %ld. Fallback.\n",
//                    __FUNCTION__, __LINE__, status);

            size_t width = CGDisplayPixelsWide(CGMainDisplayID());
            size_t height = CGDisplayPixelsHigh(CGMainDisplayID());

            if (width > 0 && height > 0) {
                screens = malloc(sizeof(screen_data));

                if (screens != NULL) {
                    *count = 1;
                    screens[0] = (screen_data) {
                        .number = 1,
                        .x = 0,
                        .y = 0,
                        .width = width,
                        .height = height
                    };
                }
            }
        }

        // Free the id's after we are done.
        free(display_ids);
    }

    return screens;
}
