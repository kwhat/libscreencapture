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

#include <CoreGraphics/CoreGraphics.h>
#include <screencapture.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SCREENCAPTURE_API int8_t* screen_capture(int16_t x, int16_t y, uint16_t width, uint16_t height) {
    CGRect rect = CGRectMake(x, y, width, height);
    CGImageRef image_ref = CGDisplayCreateImageForRect(kCGDirectMainDisplay, rect);

    CGDataProviderRef provider_ref = CGImageGetDataProvider(image_ref);
    CFDataRef data_ref = CGDataProviderCopyData(provider_ref);

    size_t bpp = CGImageGetBitsPerPixel(image_ref) / 8;

    int8_t *buffer = malloc(width * height * bpp);
    memcpy(buffer, CFDataGetBytePtr(data_ref), width * height * bpp);

    CFRelease(data_ref);
    CGImageRelease(image_ref);

    return buffer;
}
