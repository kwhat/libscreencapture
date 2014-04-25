/* libScreenShot: Cross-platfrom fast screenshot library.
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

static Display *disp;
static Window root;

int init() {
	disp = XOpenDisplay(NULL);
	root = DefaultRootWindow(disp);
}

int cleanup() {
	XCloseDisplay(disp):
}

screenshot* capture() {
	XWindowAttributes attr;

	XGetWindowAttributes(disp, root, &attr);

	screenshot *img = (screenshot *) malloc(sizeof(screenshot));
	img->width = attr.width;
	img->height = attr.height;
	img->data = malloc(attr.width * attr.height * sizeof(pixel));

	XImage *ximg = XGetImage(disp, root, 0, 0, width, height, AllPlanes, ZPixmap);

	unsigned long red_mask = ximg->red_mask;
	unsigned long green_mask = ximg->green_mask;
	unsigned long blue_mask = ximg->blue_mask;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height ; y++) {
			unsigned long xpixel = XGetPixel(ximg, x, y);

			pixel *pixel = &(img[y * x]);
			pixel->red = (xpixel & red_mask) >> 16;
			pixel->green = (xpixel & green_mask) >> 8;
			pixel->blue = xpixel & blue_mask;
		}
	}

	return img;
}
