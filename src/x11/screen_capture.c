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

//*
#include <string.h>
#include <X11/Xutil.h>

#ifdef USE_XSHM
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height) {
    Display *disp;
    Window root;
    disp = XOpenDisplay(NULL);
    if (disp != NULL) {
        root = DefaultRootWindow(disp);
    }

    XShmSegmentInfo shminfo;

struct timeval stop, start;
gettimeofday(&start, NULL);

    XImage *ximg = XShmCreateImage(disp, DefaultVisual(disp, 0), DefaultDepth(disp, 0), XShmPixmapFormat(disp), NULL, &shminfo, width, height);

    shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT | 0777);
    //FIXME shminfo.shmid >= 0 !
    shminfo.shmaddr = ximg->data = shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;

    XShmAttach(disp, &shminfo);
	XShmGetImage(disp, root, ximg, x, y, AllPlanes);

gettimeofday(&stop, NULL);
printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    int8_t* buffer = (int8_t*) malloc(width * height * 4);
    memcpy(buffer, ximg->data, width * height * 4);

	XShmDetach(disp, &shminfo);
	XDestroyImage(ximg);
	shmdt(shminfo.shmaddr);
	shmctl(shminfo.shmid, IPC_RMID, 0);

    return buffer;
}
//*/

/*
#include <X11/Xutil.h>
#include <string.h>
SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height) {
    Display *disp;
    Window root;
    disp = XOpenDisplay(NULL);
    if (disp != NULL) {
        root = DefaultRootWindow(disp);
    }

struct timeval stop, start;
gettimeofday(&start, NULL);

	XImage *ximg = XGetImage(disp, root, x, y, width, height, AllPlanes, ZPixmap);

gettimeofday(&stop, NULL);
printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

	int8_t* buffer = (int8_t*) malloc(width * height * 4);
    memcpy(buffer, ximg->data, width * height * 4);

    XDestroyImage(ximg);

	return buffer;
}
//*/

/*
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <stdio.h>

SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height) {
	GLubyte *pixels;

	// Set up Alignment format for bitmaps
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	// Define pixel array for a no extra byte. Mode 1.
	pixels = (GLubyte *) malloc(4 * width * height);
	if (pixels == NULL) {
		printf("No enough memory \n");
		exit(0);
	}

	// Store Full Image from the back buffer
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	// Store image in memory
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    return pixels;
}
*/