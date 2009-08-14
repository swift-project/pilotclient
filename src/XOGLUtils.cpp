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

OSStatus aglInitEntryPoints (void)
{
    OSStatus err = noErr;
    const Str255 frameworkName = "\pOpenGL.framework";
    FSRefParam fileRefParam;
    FSRef fileRef;
    CFURLRef bundleURLOpenGL;

    memset(&fileRefParam, 0, sizeof(fileRefParam));
    memset(&fileRef, 0, sizeof(fileRef));

    fileRefParam.ioNamePtr  = frameworkName;
    fileRefParam.newRef = &fileRef;

    // For some reason an explicit cast satisfies gcc TDG
    SInt32 fileRefParamIO_DirID = static_cast<SInt32>(fileRefParam.ioDirID);

    // Frameworks directory/folder
    err = FindFolder (kSystemDomain, kFrameworksFolderType, false,
                      &fileRefParam.ioVRefNum, &fileRefParamIO_DirID);
    if (noErr != err) {
        return err;
    }
    err = PBMakeFSRefSync (&fileRefParam); // make FSRef for folder
    if (noErr != err) {
        return err;
    }
    // create URL to folder
    bundleURLOpenGL = CFURLCreateFromFSRef (kCFAllocatorDefault,
                                            &fileRef);
    if (!bundleURLOpenGL) {
        return paramErr;
    }
    // create ref to GL's bundle
    gBundleRefOpenGL = CFBundleCreate (kCFAllocatorDefault,
                                       bundleURLOpenGL);
    if (!gBundleRefOpenGL) {
        return paramErr;
    }
    CFRelease (bundleURLOpenGL); // release created bundle
    // if the code was successfully loaded, look for our function.
    if (!CFBundleLoadExecutable (gBundleRefOpenGL)) {
        return paramErr;
    }
    return err;
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
