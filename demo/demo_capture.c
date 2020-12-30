#include <screencapture.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
#include <jpeglib.h>

void save_jpg(int8_t* buffer, uint16_t width, uint16_t height) {
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
	cinfo.in_color_space   = JCS_EXT_RGBA;

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
*/

/*
#include <olectl.h>
#include <windows.h>

// https://docs.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
bool save_bmp(int8_t* buffer, uint16_t width, uint16_t height) {
    // A file is created, this is where we will save the screen capture.
    HANDLE hFile = CreateFile("screenshot.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }


    BITMAPINFOHEADER   bi;
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

    // Add the size of the headers to the size of the bitmap to get the total file size.
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    

    BITMAPFILEHEADER   bmfHeader;

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)buffer, dwBmpSize, &dwBytesWritten, NULL);

    // Close the handle for the file that was created.
    CloseHandle(hFile);

    return true;
}
*/

#include <CoreGraphics/CoreGraphics.h>
#include <CoreServices/CoreServices.h>
#include <ImageIO/ImageIO.h>

bool save_png(int8_t* buffer, uint16_t width, uint16_t height) {
    CGDataProviderRef provider_ref = CGDataProviderCreateWithData(NULL, buffer, width * height * 4, NULL);
    CGColorSpaceRef cs_ref = CGColorSpaceCreateDeviceRGB();
    CGImageRef img_ref = CGImageCreate(width,                         // width
                                   height,                         // height
                                   8,                          // bitsPerComponent
                                   32,                         // bitsPerPixel
                                   width * 4,                       // bytesPerRow
                                   cs_ref,                      // colorspace
                                   kCGBitmapByteOrderDefault,  // bitmapInfo
                                   provider_ref,                   // CGDataProvider
                                   NULL,                       // decode array
                                   false,                         // shouldInterpolate
                                   kCGRenderingIntentDefault); // intent

    CGColorSpaceRelease(cs_ref);
    CGDataProviderRelease(provider_ref);

    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("screenshot.png"), kCFURLPOSIXPathStyle, false);

    CGImageDestinationRef destination_ref = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    if (!destination_ref) {
        printf("CGImageDestinationCreateWithURL %p\n",
            url);
        return false;
    }

    CGImageDestinationAddImage(destination_ref, img_ref, NULL);

    if (!CGImageDestinationFinalize(destination_ref)) {
        printf("CGImageDestinationFinalize");
        CFRelease(destination_ref);
        CGImageRelease(img_ref);
        return false;
    }

    CFRelease(destination_ref);
    CGImageRelease(img_ref);

    return true;
}


int main() {
    int8_t* buffer = screen_capture(0, 0, 1024, 768);

    /*
    if (save_bmp(buffer, 1024, 768)) {
        printf("Success\n");
    } else {
        printf("Failure\n");
    }
    */

	//save_jpg(buffer, 1024, 768);

    if (save_png(buffer, 1024, 768)) {
        printf("Success\n");
    } else {
        printf("Failure\n");
    }

	free(buffer);

	return 0;
}
