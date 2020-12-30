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
#include <stdlib.h>
#include "logging.h"

#if defined(USE_XINERAMA) && !defined(USE_XRANDR)
#include <X11/extensions/Xinerama.h>
#elif defined(USE_XRANDR)
#include <pthread.h>
#include <X11/extensions/Xrandr.h>
#endif

SCREENCAPTURE_API screen_data_t* screen_info(int8_t *count) {
    *count = 0;
    screen_data_t *screens = NULL;

    Display *disp = XOpenDisplay(XDisplayName(NULL));

    #if defined(USE_XINERAMA) && !defined(USE_XRANDR)
    if (XineramaIsActive(disp)) {
        int xine_count = 0;
        XineramaScreenInfo *xine_info = XineramaQueryScreens(disp, &xine_count);

        if (xine_info != NULL) {
            if (xine_count > UINT8_MAX) {
                *count = UINT8_MAX;

                log_warn("%s [%u]: Screen count overflow detected!\n",
                        __FUNCTION__, __LINE__);
            } else {
                *count = (uint8_t) xine_count;
            }

            screens = malloc(sizeof(screen_data_t) * xine_count);

            if (screens != NULL) {
                for (int i = 0; i < xine_count; i++) {
                    screens[i] = (screen_data_t) {
                        .number = xine_info[i].screen_number,
                        .x = xine_info[i].x_org,
                        .y = xine_info[i].y_org,
                        .width = xine_info[i].width,
                        .height = xine_info[i].height
                    };
                }
            }

            XFree(xine_info);
        }
    }
    #elif defined(USE_XRANDR)
    pthread_mutex_lock(&xrandr_mutex);
    if (xrandr_resources != NULL) {
        int xrandr_count = xrandr_resources->ncrtc;
        if (xrandr_count > UINT8_MAX) {
            *count = UINT8_MAX;

            log_warn("%s [%u]: Screen count overflow detected!\n",
                    __FUNCTION__, __LINE__);
        } else {
            *count = (uint8_t) xrandr_count;
        }

        screens = malloc(sizeof(screen_data_t) * xrandr_count);

        if (screens != NULL) {
            for (int i = 0; i < xrandr_count; i++) {
                XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(disp, xrandr_resources, xrandr_resources->crtcs[i]);

                if (crtc_info != NULL) {
                    screens[i] = (screen_data_t) {
                        .number = i + 1,
                        .x = crtc_info->x,
                        .y = crtc_info->y,
                        .width = crtc_info->width,
                        .height = crtc_info->height
                    };

                    XRRFreeCrtcInfo(crtc_info);
                } else {
                    log_warn("%s [%u]: XRandr failed to return crtc information! (%#X)\n",
                            __FUNCTION__, __LINE__, xrandr_resources->crtcs[i]);
                }
            }
        }
    }
    pthread_mutex_unlock(&xrandr_mutex);
    #else
    Screen* default_screen = DefaultScreenOfDisplay(disp);

    if (default_screen->width > 0 && default_screen->height > 0) {
        screens = malloc(sizeof(screen_data_t));

        if (screens != NULL) {
            *count = 1;
            screens[0] = (screen_data_t) {
                .number = 1,
                .x = 0,
                .y = 0,
                .width = default_screen->width,
                .height = default_screen->height
            };
        }
    }
    #endif

    return screens;
}
