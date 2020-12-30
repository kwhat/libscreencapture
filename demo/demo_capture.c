#include <screencapture.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(__APPLE__)
#include <CoreGraphics/CoreGraphics.h>
#include <CoreServices/CoreServices.h>
#include <ImageIO/ImageIO.h>
#elif defined(_WIN32)
#include <olectl.h>
#include <windows.h>
#else
#include <jpeglib.h>
#endif


#if defined(__APPLE__)
bool save_image(int8_t* buffer, uint16_t width, uint16_t height) {
    bool status = false;

    CGDataProviderRef provider_ref = CGDataProviderCreateWithData(NULL, buffer, width * height * 4, NULL);
    if (provider_ref != NULL) {
        CGColorSpaceRef cs_ref = CGColorSpaceCreateDeviceRGB();
        if (cs_ref != null) {
            CGImageRef img_ref = CGImageCreate(
                width,
                height,
                8,          // bitsPerComponent
                32,         // bitsPerPixel
                width * 4,  // bytesPerRow
                cs_ref,
                kCGBitmapByteOrderDefault,
                provider_ref,
                NULL,       // decode array
                false,      // shouldInterpolate
                kCGRenderingIntentDefault
            );

            if (img_ref != NULL) {
                CFURLRef url_ref = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("screenshot.png"), kCFURLPOSIXPathStyle, false);
                if (url_ref != NULL) {
                    CGImageDestinationRef destination_ref = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
                    if (destination_ref != NULL) {
                        CGImageDestinationAddImage(destination_ref, img_ref, NULL);
                        if (CGImageDestinationFinalize(destination_ref)) {
                            status = true;
                        }

                        CFRelease(destination_ref);
                    }

                    CFRelease(url_ref);
                }

                CGImageRelease(img_ref);
            }

            CGColorSpaceRelease(cs_ref);
        }

        CGDataProviderRelease(provider_ref);
    }

    return status;
}
#elif defined(_WIN32)
// https://docs.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
bool save_bmp(int8_t* buffer, uint16_t width, uint16_t height) {
    bool status = false;

    HANDLE hFile = CreateFile(
        "screenshot.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        BITMAPINFOHEADER bi;
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = width;
        bi.biHeight = height;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        DWORD dwBmpSize = ((bi.biWidth * bi.biBitCount + 31) / 32) * 4 * bi.biHeight;


        BITMAPFILEHEADER   bmfHeader;
        bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bmfHeader.bfSize = dwBmpSize + bmfHeader.bfOffBits;
        bmfHeader.bfType = 0x4D42; // bfType must always be BM for Bitmaps.

        if (
            WriteFile(hFile, (LPSTR) &bmfHeader, sizeof(BITMAPFILEHEADER), NULL, NULL) &&
            WriteFile(hFile, (LPSTR) &bi, sizeof(BITMAPINFOHEADER), NULL, NULL) &&
            WriteFile(hFile, (LPSTR) buffer, dwBmpSize, NULL, NULL)
        ) {
            status = true;
        }

        // Close the handle for the file that was created.
        CloseHandle(hFile);
    }

    return status;
}
#else
bool save_jpg(int8_t* buffer, uint16_t width, uint16_t height) {
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

	cinfo.image_width      = width;
	cinfo.image_height     = height;
	cinfo.input_components = 4;
	cinfo.in_color_space   = JCS_EXT_BGRA;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, 100, true);
	jpeg_start_compress(&cinfo, true);

	JSAMPROW row_pointer;
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer = (JSAMPROW) &buffer[cinfo.next_scanline * cinfo.input_components * cinfo.image_width];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
}
#endif

int main() {
    bool status = false;

    int8_t* buffer = screen_capture(0, 0, 1024, 768);
    if (buffer != NULL) {
        #if defined(__APPLE__)
        status = save_png(buffer, 1024, 768);
        #elif defined(_WIN32)
        status = save_bmp(buffer, 1024, 768);
        #else
        status = save_jpg(buffer, 1024, 768);
        #endif

        free(buffer);
    }

	return (int) !status;
}
