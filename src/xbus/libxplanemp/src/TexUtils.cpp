/* 
 * Copyright (c) 2006, Laminar Research.
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
#include "XPLMGraphics.h"
#include "TexUtils.h"
#include "BitmapUtils.h"
#include "XPLMUtilities.h"
#include <utility>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <utility>
#if IBM
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#elif APL
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

using std::swap;

static void HalfBitmap(ImageInfo& ioImage)
{
	int row_b = (ioImage.width * ioImage.channels) + ioImage.pad;
	
	unsigned char * srcp1 = ioImage.data;
	unsigned char * srcp2 = ioImage.data + row_b;
	unsigned char * dstp = ioImage.data;
	
	ioImage.height /= 2;
	ioImage.width /= 2;
	int yr = ioImage.height;
	int t1, t2, t3, t4;

	if (ioImage.channels == 3)
	{
		while (yr--)
		{
			int xr = ioImage.width;
			while (xr--)
			{
				t1  = *srcp1++;
				t2  = *srcp1++;
				t3  = *srcp1++;

				t1 += *srcp1++;
				t2 += *srcp1++;
				t3 += *srcp1++;

				t1 += *srcp2++;
				t2 += *srcp2++;
				t3 += *srcp2++;

				t1 += *srcp2++;
				t2 += *srcp2++;
				t3 += *srcp2++;

				t1 >>= 2;
				t2 >>= 2;
				t3 >>= 2;

				*dstp++ = t1;
				*dstp++ = t2;
				*dstp++ = t3;
			}
			
			srcp1 += row_b;
			srcp1 += ioImage.pad;
			srcp2 += row_b;
			srcp2 += ioImage.pad;
		}
	} else {

		while (yr--)
		{
			int xr = ioImage.width;
			while (xr--)
			{
				t1  = *srcp1++;
				t2  = *srcp1++;
				t3  = *srcp1++;
				t4  = *srcp1++;

				t1 += *srcp1++;
				t2 += *srcp1++;
				t3 += *srcp1++;
				t4 += *srcp1++;

				t1 += *srcp2++;
				t2 += *srcp2++;
				t3 += *srcp2++;
				t4 += *srcp2++;

				t1 += *srcp2++;
				t2 += *srcp2++;
				t3 += *srcp2++;
				t4 += *srcp2++;

				t1 >>= 2;
				t2 >>= 2;
				t3 >>= 2;
				t4 >>= 2;

				*dstp++ = t1;
				*dstp++ = t2;
				*dstp++ = t3;
				*dstp++ = t4;
			}
			
			srcp1 += row_b;
			srcp1 += ioImage.pad;
			srcp2 += row_b;
			srcp2 += ioImage.pad;
		}
	}	
	ioImage.pad = 0;

}

bool LoadTextureFromFile(const char * inFileName, int inTexNum, bool magentaAlpha, bool inWrap, bool mipmap, int * outWidth, int * outHeight, int inDeres)
{
	bool	ok = false;
	struct ImageInfo	im;
	long	count = 0;
#if 1
	unsigned char * p;
#endif	
	int result = CreateBitmapFromPNG(inFileName, &im);
	if (result) result = CreateBitmapFromFile(inFileName, &im);
	if (result == 0)
	{
		while (inDeres > 0)
		{
			HalfBitmap(im);
			--inDeres;
		}
	
		if (!magentaAlpha || ConvertBitmapToAlpha(&im) == 0)
		{
			if (im.pad == 0)
			{
				XPLMBindTexture2d(inTexNum, 0);
				if (magentaAlpha)
				{
#if 1
					p = im.data;
					count = im.width * im.height;
					while (count--)
					{
						std::swap(p[0], p[2]);
//						swap(p[1], p[2]);
						p += 4;
					}
#endif				
					if (mipmap)
						gluBuild2DMipmaps(GL_TEXTURE_2D, 4, im.width, im.height, GL_RGBA, GL_UNSIGNED_BYTE, im.data);
					else
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.width ,im.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.data);
					if (outWidth) *outWidth = im.width;
					if (outHeight) *outHeight = im.height;
				} else {
#if 1
					p = im.data;
					count = im.width * im.height;
					while (count--)
					{
						std::swap(p[0], p[2]);
						p += 3;
					}
#endif				
					if (mipmap)
						gluBuild2DMipmaps(GL_TEXTURE_2D, 3, im.width, im.height, GL_RGB, GL_UNSIGNED_BYTE, im.data);					
					else
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im.width ,im.height, 0, GL_RGB, GL_UNSIGNED_BYTE, im.data);
						if (outWidth) *outWidth = im.width;
						if (outHeight) *outHeight = im.height;
				}
				ok = true;
			}	
		}
				
		DestroyBitmap(&im);
	}
	
	if (ok)
	{
		// BAS note: for some reason on my WinXP system with GF-FX, if
		// I do not set these explicitly to linear, I get no drawing at all.
		// Who knows what default state the card is in. :-(
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);

		static const char * ver_str = (const char *) glGetString(GL_VERSION);
		static const char * ext_str = (const char *) glGetString(GL_EXTENSIONS);
		
		static bool tex_clamp_avail = 
			strstr(ext_str,"GL_SGI_texture_edge_clamp"		) ||
			strstr(ext_str,"GL_SGIS_texture_edge_clamp"		) || 
			strstr(ext_str,"GL_ARB_texture_edge_clamp"		) || 
			strstr(ext_str,"GL_EXT_texture_edge_clamp"		) ||
			strncmp(ver_str,"1.2", 3) ||
			strncmp(ver_str,"1.3", 3) ||
			strncmp(ver_str,"1.4", 3) ||
			strncmp(ver_str,"1.5", 3);

		
			 if(inWrap 		   ){glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT		 );
								 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT		 );}
		else if(tex_clamp_avail){glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
								 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);}
		else					{glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP		 );
								 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP		 );}
		
	}	
	int err = glGetError();
	if (err)
	{
		char buf[256];
		sprintf(buf, "Texture load got OGL err: %d\n", err);
		XPLMDebugString(buf);
	}
	return ok;
}
