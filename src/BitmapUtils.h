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
#ifndef _BitmapUtils_h_
#define _BitmapUtils_h_

/*
	WARNING: Struct alignment must be "68K" (e.g. 2-byte alignment) for these
	structures to be happy!!!
*/

/* 
	These headers match the Microsoft bitmap file and image headers more or less
	and are read right out of the file.
*/

#if APL
#pragma options align=mac68k
#endif
#if IBM
#pragma pack(push, 2)
#endif

struct	BMPHeader {
	char			signature1;
	char			signature2;
	long			fileSize;
	long			reserved;
	long			dataOffset;
};

struct	BMPImageDesc {
	long			structSize;
	long			imageWidth;
	long			imageHeight;
	short			planes;
	short			bitCount;
	long			compressionType;
	long			imageSize;
	long			xPixelsPerM;	//130B0000?  B013 = 45075?
	long			yPixelsPerM;
	long			colorsUsed;
	long			colorsImportant;
};

#if APL
#pragma options align=reset
#endif
#if IBM
#pragma pack(pop)
#endif


/*
	This is our in memory way of storing an image.  Data is a pointer
	to an array of bytes large enough to hold the image.  We always
	use 24-bit RGB or 32-bit ARGB.  The lower left corner of the BMP
	file is in the first byte of data.
	
	Pad is how many bytes we skip at the end of each scanline.  Each
	scanline must start on a 4-byte boundary!
*/

struct	ImageInfo {
	unsigned char *	data;
	long			width;
	long			height;
	long			pad;
	short			channels;
};	

/* Given a file path and an uninitialized imageInfo structure, this routine fills
 * in the imageInfo structure by loading the bitmap. */
int		CreateBitmapFromFile(const char * inFilePath, struct ImageInfo * outImageInfo);

#if BITMAP_USE_JPEG
/* Same as above, but uses IJG JPEG code. */
int		CreateBitmapFromJPEG(const char * inFilePath, struct ImageInfo * outImageInfo);

/* Same as above, but create the image from an in-memory image of a JFIF file,
 * allows you to read the image yourself, or mem map it. */
int		CreateBitmapFromJPEGData(void * inBytes, int inLength, struct ImageInfo * outImageInfo);
#endif

/* Yada yada yada, libPng. */
int		CreateBitmapFromPNG(const char * inFilePath, struct ImageInfo * outImageInfo);

/* Given an imageInfo structure, this routine writes it to disk as a .bmp file. 
 * Note that only 3-channel bitmaps may be written as .bmp files!! */
int		WriteBitmapToFile(const struct ImageInfo * inImage, const char * inFilePath);

/* This routine creates a new bitmap and fills in an uninitialized imageInfo structure.
 * The contents of the bitmap are undetermined and must be 'cleared' by you. */
int		CreateNewBitmap(long inWidth, long inHeight, short inChannels, struct ImageInfo * outImageInfo);

/* Given a bitmap, this routine fills the whole bitmap in with a gray level of c, where
 * c = 0 means black and c = 255 means white. */
void	FillBitmap(const struct ImageInfo * inImageInfo, char c);

/* This routine deallocates a bitmap that was created with CreateBitmapFromFile or
 * CreateNewBitmap. */ 
void	DestroyBitmap(const struct ImageInfo * inImageInfo);

/* Given two bitmaps, this routine copies a section from one bitmap to another.  
 * This routine will use bicubic and bilinear interpolation to copy the bitmap 
 * as cleanly as possible.  However, if the bitmap contains alpha, the copy routine
 * will create a jagged edge to keep from smearing the alpha channel. */
void	CopyBitmapSection(
			const struct ImageInfo *	inSrc,
			const struct ImageInfo *	inDst,
			long				inSrcLeft,
			long				inSrcTop,
			long				inSrcRight,
			long				inSrcBottom,
			long				inDstLeft,
			long				inDstTop,
			long				inDstRight,
			long				inDstBottom);
			
void	CopyBitmapSectionWarped(
			const struct ImageInfo *	inSrc,
			const struct ImageInfo *	inDst,
			long				inTopLeftX,
			long				inTopLeftY,
			long				inTopRightX,
			long				inTopRightY,
			long				inBotRightX,
			long				inBotRightY,
			long				inBotLeftX,
			long				inBotLeftY,
			long				inDstLeft,
			long				inDstTop,
			long				inDstRight,
			long				inDstBottom);
			
/* This routine rotates a bitmap counterclockwise 90 degrees, exchanging its width
 * and height. */
void	RotateBitmapCCW(
			struct ImageInfo *	ioBitmap);

/* This routine converts a 3-channel bitmap to a 4-channel bitmap by converting
 * magenta pixels to alpha. */
int	ConvertBitmapToAlpha(
			struct ImageInfo *		ioImage);
			
/* This routine converts a 4-channel bitmap to a 3-channel bitmap by converting
 * alpha back to magenta. */
int	ConvertAlphaToBitmap(
			struct ImageInfo *		ioImage);
#endif
