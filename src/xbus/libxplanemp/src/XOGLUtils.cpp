/*
 * Copyright (c) 2005, Ben Supnik and Chris Serio.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
 #include "XOGLUtils.h"

// This had to be renamed because on Linux, namespaces appear to be shared between ourselves and XPlane
// so i would end up overwritting XPlanes function pointer!
#if APL
//PFNGLBINDBUFFERARBPROC			glBindBufferARB			 = NULL;
#endif
#if IBM
PFNGLBINDBUFFERARBPROC			glBindBufferARB			 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLMULTITEXCOORD2FVARBPROC	glMultiTexCoord2fvARB	 = NULL;
#endif

/**************************************************
Nasty Mac Specific Stuff to Load OGL DLL Extensions
***************************************************/

#if APL

#include <CoreFoundation/CFBundle.h>
#include <Carbon/Carbon.h>

CFBundleRef	gBundleRefOpenGL = NULL;


// Utility routine to get a bundle from the system folder by file name....typically used to find OpenGL to get extension functions.
int load_bundle_by_filename (const char * in_filename, CFBundleRef * io_bundle_ref)
{
    OSStatus		err = noErr;
	FSRef			framework_fs;
    CFURLRef		framework_url = NULL;
	CFURLRef		bundle_url = NULL;
	CFStringRef		bundle_name = NULL;
	CFBundleRef		bundle_ref = NULL;
	
	bundle_name = CFStringCreateWithCString(kCFAllocatorDefault, in_filename, kCFStringEncodingUTF8);
    if (bundle_name == NULL) {
		err = paramErr;
    	goto bail; }
	
	err = FSFindFolder(kSystemDomain, kFrameworksFolderType, false, &framework_fs);
    if (noErr != err) {
		err = dirNFErr;
    	goto bail; }
	
    // create URL to folder
    framework_url = CFURLCreateFromFSRef (kCFAllocatorDefault, &framework_fs);
	if(framework_url == NULL) {
		err = ioErr;
		goto bail; }
	
	bundle_url = CFURLCreateWithFileSystemPathRelativeToBase(kCFAllocatorDefault, bundle_name, kCFURLPOSIXPathStyle, false, framework_url);
	if(bundle_url == NULL) {
		err = fnfErr;
		goto bail; }
	
    bundle_ref = CFBundleCreate (kCFAllocatorDefault, bundle_url);
	if(bundle_ref == NULL) {
		err = permErr;
		goto bail; }
	
    if (!CFBundleLoadExecutable (bundle_ref)) {
        err = bdNamErr;
		goto bail;
    }
	
	if (io_bundle_ref) { *io_bundle_ref = bundle_ref; bundle_ref = NULL; }
bail:
		if(bundle_ref)		CFRelease(bundle_ref);
	if(bundle_name)		CFRelease(bundle_name);
	if(bundle_url)		CFRelease(bundle_url);
	if(framework_url)	CFRelease(framework_url);
	
    return err;
}


OSStatus aglInitEntryPoints (void)
{
	return load_bundle_by_filename ("OpenGL.framework", &gBundleRefOpenGL);
}

void * aglGetProcAddress (char * pszProc)
{
	static bool first_time = true;
	if (first_time)
	{
		first_time = false;
		if (aglInitEntryPoints() != noErr)
			return NULL;			
	}
    return CFBundleGetFunctionPointerForName (gBundleRefOpenGL,
                CFStringCreateWithCStringNoCopy (NULL,
                     pszProc, CFStringGetSystemEncoding (), NULL));
}

#define 	wglGetProcAddress(x)		aglGetProcAddress(x)

#endif

#if LIN

#define		wglGetProcAddress(x)		glXGetProcAddressARB((GLubyte*) (x))

#endif

/**************************************************
			   Utilities Initialization
***************************************************/

bool	OGL_UtilsInit()
{
	static bool firstTime = true;
	if(firstTime)
	{
		// Initialize all OGL Function Pointers
#if APL
//		glBindBufferARB 		 = (PFNGLBINDBUFFERARBPROC)			 wglGetProcAddress("glBindBufferARB"		 );
#endif		
#if IBM		
		glBindBufferARB			 = (PFNGLBINDBUFFERARBPROC)			 wglGetProcAddress("glBindBufferARB"		);
		glActiveTextureARB 		 = (PFNGLACTIVETEXTUREARBPROC)		 wglGetProcAddress("glActiveTextureARB"		 );
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC )	 wglGetProcAddress("glMultiTexCoord2fARB"    );
		glMultiTexCoord2fvARB	 = (PFNGLMULTITEXCOORD2FVARBPROC )	 wglGetProcAddress("glMultiTexCoord2fvARB"   );
#endif		
		firstTime = false;
	}

	// Make sure everything got initialized
	if(glBindBufferARB &&
	   glActiveTextureARB &&
	   glClientActiveTextureARB &&
	   glMultiTexCoord2fARB &&
	   glMultiTexCoord2fvARB)
	{
	   return true;
	}
	else
		return false;

}
