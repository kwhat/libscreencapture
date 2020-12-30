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
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height) {
    // Get handle for the desktop window
    HWND hDesktopWnd = GetDesktopWindow();

    // Get handle for the desktop device context (DC)
    HDC hDesktopDC = GetDC(hDesktopWnd);
    if (hDesktopDC == NULL) {
        return NULL;
    }

    // Create a memory device context (DC) from the desktop context
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
    if (hCaptureDC == NULL) {
        return NULL;
    }

    // Create a bitmap compatible with the device that is associated with the specified device context.
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, width, height);

	// Select the compatible bitmap into the compatible memory DC.
	HGDIOBJ hOld = SelectObject(hCaptureDC, hCaptureBitmap);

	// Bit block transfer into our compatible memory DC.
    if (BitBlt(hCaptureDC, 0, 0, width, height, hDesktopDC, x, y, SRCCOPY) == false) {
        return NULL;
    }

/*
    BITMAPINFOHEADER   bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
*/
    BITMAPINFO MyBMInfo = {0};
    MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

    // Get the BITMAPINFO structure from the bitmap
    if (GetDIBits(hDesktopDC, hCaptureBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS) == 0) {
        return NULL;
    }

    // create the bitmap buffer
    BYTE* lpPixels = malloc(MyBMInfo.bmiHeader.biSizeImage);
    if (lpPixels == NULL) {
        return NULL;
    }

    // Better do this here - the original bitmap might have BI_BITFILEDS, which makes it
    // necessary to read the color table - you might not want this.
    MyBMInfo.bmiHeader.biCompression = BI_RGB;

    // get the actual bitmap buffer
    if (GetDIBits(hDesktopDC, hCaptureBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS) == 0) {
        return NULL;
    }

    // always select the previously selected object once done
    SelectObject(hCaptureDC, hOld);

	ReleaseDC(hDesktopWnd, hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);

	return lpPixels;
}
