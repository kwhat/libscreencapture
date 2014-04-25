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

#include <stdio.h>
#include <png.h>

#include "screenshot.h"

void save_img(screenshot *ss) {
	int code = 0;
	FILE *fp;
	png_structp png_ptr;
	png_infop png_info_ptr;
	png_bytep png_row;

	// Open file
	fp = fopen("test.png", "wb");
	if (fp == NULL) {
		fprintf (stderr, "Could not open file for writing\n");
		code = 1;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf (stderr, "Could not allocate write struct\n");
		code = 1;
	}

	// Initialize info structure
	png_info_ptr = png_create_info_struct(png_ptr);
	if (png_info_ptr == NULL) {
		fprintf (stderr, "Could not allocate info struct\n");
		code = 1;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit color depth)
	png_set_IHDR(png_ptr, png_info_ptr, ss->width, ss->height,
		  8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_png(png_ptr, png_info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	// Free
	fclose(fp);

	if (png_info_ptr != NULL) {
		png_free_data(png_ptr, png_info_ptr, PNG_FREE_ALL, -1);
	}

	if (png_ptr != NULL) {
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
	}

	if (png_row != NULL) {
		free(png_row);
	}
}
