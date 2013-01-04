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

#ifndef XPLMMULTIPLAYERCSL_H
#define XPLMMULTIPLAYERCSL_H

/*
 * XPLMMultiplayerCSL
 *
 * This unit is the master switch for managing aircraft models.  It loads up all CSL packages and
 * does the matching logic for finding aircraft.
 *
 */

#include "XPLMPlanes.h"
#include "XPMPMultiplayerVars.h"

/*
 * CSL_Init
 *
 * This routine Initializes the Multiplayer object section and sets up the lighting texture.
 *
 */
bool			CSL_Init(
						const char* inTexturePath);
/*
 * CSL_LoadCSL
 *
 * This routine loads all CSL packages and the related.txt file.
 *
 */
bool			CSL_LoadCSL(
						const char * inFolderPath, 		// Path to CSL folder
						const char * inRelated,			// Path to related.txt - used by renderer for model matching
						const char * inIcao8643);		// Path to ICAO document 8643 (list of aircraft)
						
/*
 * CSL_MatchPlane
 *
 * Given an ICAO and optionally a livery and airline, this routine returns the best plane match, or 
 * NULL if there is no good plane match.  If got_livery is not null, it is filled in with whether
 * the match has a valid paint job or not.
 *
 */
CSLPlane_t *	CSL_MatchPlane(const char * inICAO, const char * inAirline, const char * inLivery, bool * got_livery, bool use_default);

/*
 * CSL_Dump
 *
 * This routine dumps the total state of the CSL system to the error.out file.
 *
 */
void			CSL_Dump(void);

/*
 * CSL_GetOGLIndex
 *
 * Given a model, this routine returns some kind of index number such that
 * sorting all models by these indices gives the ideal OpenGL draw order.
 * In other words, this index number sorts models by OGL state.
 *
 */
int				CSL_GetOGLIndex(CSLPlane_t *		model);

/*
 * CSL_DrawObject
 *
 * Given a plane model rep and the params, this routine does the real drawing.  The coordinate system must be pre-shifted
 * to the plane's location.  (This just dispatches to the appropriate drawing method).
 *
 */
void			CSL_DrawObject(
							CSLPlane_t *			model,
							float					distance,
							double 					x,
							double 					y,
							double 					z,
							double 					pitch,
							double 					roll,
							double 					heading,
							int						type,
							int	   					full,
							xpmp_LightStatus		lights,
							XPLMPlaneDrawState_t *	state);
							

#if APL
int Posix2HFSPath(const char *path, char *result, int resultLen);
int HFS2PosixPath(const char *path, char *result, int resultLen);
#endif

#endif /* XPLMMULTIPLAYERCSL_H */
