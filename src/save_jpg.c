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

#include <stdbool.h>
#include <stdio.h>
#include <jpeglib.h>

#include "screenshot.h"

void save_jpg(screenshot *ss) {
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	char * filename = "screenshot.jpg";
	FILE * outfile;
	if ((outfile = fopen(filename, "wb+")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		exit(1);
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width      = ss->width;
	cinfo.image_height     = ss->height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	/*set the quality [0..100]  */
	jpeg_set_quality (&cinfo, 100, true);
	jpeg_start_compress(&cinfo, true);

	JSAMPROW row_pointer;          /* pointer to a single row */
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer = (JSAMPROW) &ss->data[cinfo.next_scanline * ss->height * ss->width];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
}
