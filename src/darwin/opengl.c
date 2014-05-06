
#import <ApplicationServices/ApplicationServices.h>
#import <CoreFoundation/CoreFoundation.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

int init() {

}

int cleanup() {

}

screenshot* capture() {
    CGContextRef bitmap;
    CGImageRef image;
    void * data;
    long bytewidth;
    GLint width, height;
    long bytes;
    CGColorSpaceRef cSpace = CGColorSpaceCreateWithName (kCGColorSpaceGenericRGB);

	    CGLContextObj    glContextObj;
    CGLPixelFormatObj pixelFormatObj ;
    long numPixelFormats ;
	
    CGLPixelFormatAttribute attribs[] =
    {
        kCGLPFAFullScreen,
        kCGLPFADisplayMask,
        0,    /* Display mask bit goes here */
        0
    } ;


    if ( display == kCGNullDirectDisplay )
        display = CGMainDisplayID();
    attribs[2] = CGDisplayIDToOpenGLDisplayMask(display);


    /* Build a full-screen GL context */
    CGLChoosePixelFormat( attribs, &pixelFormatObj, &numPixelFormats );
    if ( pixelFormatObj == NULL )    // No full screen context support
        return NULL;
    CGLCreateContext( pixelFormatObj, NULL, &glContextObj ) ;
    CGLDestroyPixelFormat( pixelFormatObj ) ;
    if ( glContextObj == NULL )
        return NULL;


    CGLSetCurrentContext( glContextObj ) ;
    CGLSetFullScreen( glContextObj ) ;


    glReadBuffer(GL_FRONT);


    width = srcRect.size.width;
    height = srcRect.size.height;


    bytewidth = width * 4; // Assume 4 bytes/pixel for now
    bytewidth = (bytewidth + 3) & ~3; // Align to 4 bytes
    bytes = bytewidth * height; // width * height

    /* Build bitmap context */
    data = malloc(height * bytewidth);
    if ( data == NULL )
    {
        CGLSetCurrentContext( NULL );
        CGLClearDrawable( glContextObj ); // disassociate from full screen
        CGLDestroyContext( glContextObj ); // and destroy the context
        return NULL;
    }
    bitmap = CGBitmapContextCreate(data, width, height, 8, bytewidth,
                                   cSpace, kCGImageAlphaNoneSkipFirst /* XRGB */);
    CFRelease(cSpace);


    /* Read framebuffer into our bitmap */
    glFinish(); /* Finish all OpenGL commands */
    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* Force 4-byte alignment */
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

    /*
     * Fetch the data in XRGB format, matching the bitmap context.
     */
    glReadPixels(
			(GLint) srcRect.origin.x,
			(GLint)srcRect.origin.y,
			width,
			height,
			GL_BGRA,
			#ifdef __BIG_ENDIAN__
            GL_UNSIGNED_INT_8_8_8_8_REV, // for PPC
			#else
            GL_UNSIGNED_INT_8_8_8_8, // for Intel! http://lists.apple.com/archives/quartz-dev/2006/May/msg00100.html
			#endif
            data);
    /*
     * glReadPixels generates a quadrant I raster, with origin in the lower left
     * This isn't a problem for signal processing routines such as compressors,
     * as they can simply use a negative 'advance' to move between scanlines.
     * CGImageRef and CGBitmapContext assume a quadrant III raster, though, so we need to
     * invert it. Pixel reformatting can also be done here.
     */
    swizzleBitmap(data, bytewidth, height);


    /* Make an image out of our bitmap; does a cheap vm_copy of the bitmap */
    image = CGBitmapContextCreateImage(bitmap);

    /* Get rid of bitmap */
    CFRelease(bitmap);
    free(data);


    /* Get rid of GL context */
    CGLSetCurrentContext( NULL );
    CGLClearDrawable( glContextObj ); // disassociate from full screen
    CGLDestroyContext( glContextObj ); // and destroy the context

    /* Returned image has a reference count of 1 */
    return image;
}