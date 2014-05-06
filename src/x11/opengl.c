

#include <stdio.h>
#include <stdlib.h>

#include <GL/glx.h>
#include <X11/Xlib.h>



/* Functions in writegif.c */
/* extern void writegif(FILE *fp, XImage *image, int width, int height, int numcol, unsigned char rgb[][256]); */

/* Functions in writebmp.c */
//extern void write_bmp24(FILE *fp, int width, int height, GLvoid *pixels);
int getelem2d(int i, int j, int k, int width, int height, int depth, GLubyte *array3d) {
	int value;

	value = array3d[k + depth * (j + height * i)];

	return value;
}

void putelem2d(int fwdvalue, int i, int j, int k, int width, int height, int depth, GLubyte *array3d) {
	array3d[k + depth * (j + height * i)] = fwdvalue;
}

void grabber() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int width = (int) viewport[2];
	int height = (int) viewport[3];

	int pixelsqty;
	int i, j, k;
	int fwdvalue;
	FILE *fp_bmp;
	GLubyte *pixels;
	GLubyte *revpixels;

	/* Determine pixels quantity */
	pixelsqty = 3 * width * height;

	/* Set up Alignment format for bitmaps */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	/* Define pixel array for a no extra byte. Mode 1. */
	pixels = (GLubyte *) malloc(pixelsqty);
	if (pixels == NULL) {
		printf("No enough memory \n");
		exit(0);
	}

	/* Define revpixel array for a no extra byte. Mode 1. */
	revpixels = (GLubyte*) malloc(pixelsqty);
	if (revpixels == NULL) {
		printf("No enough memory \n");
		exit(0);
	}

	/* Store Full Image from the back buffer */
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	/* Store image in memory */
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	/* Save pixels' RGB values inverse to an array */
	// TODO There has to be a better way than this.
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			for (k = 0; k < 3; k++) {
				fwdvalue = getelem2d(i, j, k, (int) width, height, 3, pixels);
				putelem2d(fwdvalue, i, j, (2 - k), width, height, 3, revpixels);
			}
		}
	}

	/* Write 24-bit BMP file */
	save_jpg();

	/* Close file and free memory */
	fclose(fp_bmp);
	free(pixels);
	free(revpixels);
}

