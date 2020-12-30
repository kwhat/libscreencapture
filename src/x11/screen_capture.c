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

#include <sys/time.h>
#include <stdio.h>

#include <screencapture.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h>
#include "logging.h"

#ifdef USE_XSHM
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

static int8_t* create_ximg_buffer(XImage *ximg) {
    int8_t *buffer = NULL;

    if (ximg != NULL) {
        size_t size = ximg->width * ximg->height * (ximg->bits_per_pixel / 8);
        buffer = (int8_t*) malloc(size);
        if (buffer != NULL) {
            memcpy(buffer, ximg->data, size);
        } else {
            log_error("%s [%u]: Out of memory, malloc failed!\n",
                    __FUNCTION__, __LINE__);
        }
    }

    return buffer;
}

SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height) {
    int8_t* buffer = NULL;

    Display *disp = XOpenDisplay(NULL);
    if (disp != NULL) {
        Window root = DefaultRootWindow(disp);
        #ifdef USE_XSHM
        XShmSegmentInfo shminfo;
        XImage *ximg = XShmCreateImage(disp, DefaultVisual(disp, 0), DefaultDepth(disp, 0), XShmPixmapFormat(disp), NULL, &shminfo, width, height);
        if (ximg != NULL) {
            shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT | 0777);
            if (shminfo.shmid >= 0) {
                shminfo.readOnly = False;
                shminfo.shmaddr = ximg->data = shmat(shminfo.shmid, 0, 0);

                if (shminfo.shmaddr != NULL) {
                    if (XShmAttach(disp, &shminfo) && XShmGetImage(disp, root, ximg, x, y, AllPlanes)) {
                        buffer = create_ximg_buffer(ximg);

                        XShmDetach(disp, &shminfo);
                    }

                    shmdt(shminfo.shmaddr);
                } else {
                    //fprintf(stderr, "%s\n", explain_shmat(shmid, shmaddr, shmflg));
                }

                shmctl(shminfo.shmid, IPC_RMID, 0);
            }

            XDestroyImage(ximg);
        }
        #else
        XImage *ximg = XGetImage(disp, root, x, y, width, height, AllPlanes, ZPixmap);
        if (ximg != NULL) {
            buffer = create_ximg_buffer(ximg);

            XDestroyImage(ximg);
        }
        #endif
    }

    return buffer;
}
