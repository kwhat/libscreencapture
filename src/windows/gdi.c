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

int init() {

}

int cleanup() {

}

screenshot* capture() {
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hDesktopWnd = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktopWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hCaptureDC, hCaptureBitmap);

	// Bit block transfer into our compatible memory DC.
	// FIXME Check for failure
    BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);

	screenshot *img = (screenshot *) malloc(sizeof(screenshot));
	img->width = nScreenWidth;
	img->height = nScreenHeight;
	img->data = malloc(nScreenWidth * nScreenHeight * sizeof(pixel));

	for (int x = 0; x < nScreenWidth; x++) {
		for (int y = 0; y < nScreenHeight; y++) {
			COLORREF bmpPixel = GetPixel(hCaptureDC, x, y);

			pixel *pixel = &(img[y * x]);
			pixel->red = GetRValue(bmpPixel);
			pixel->green = GetRValue(bmpPixel);
			pixel->blue = GetRValue(bmpPixel);
		}
	}

	ReleaseDC(hDesktopWnd, hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap)

	return img;
}
