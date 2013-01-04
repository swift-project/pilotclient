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

#include "XPMPMultiplayerObj.h"
#include "XPMPMultiplayerVars.h"

//#include "PlatformUtils.h"
#include "XObjReadWrite.h"
#include "TexUtils.h"
#include "XOGLUtils.h"

#include <map>
#include <vector>
#include <math.h>

#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"

#define DEBUG_NORMALS 0
#define	DISABLE_SHARING 0
#define BLEND_NORMALS 1

using namespace std;

const	double	kMetersToNM = 0.000539956803;
// Some color constants
//const	float	kNavLightRed[] = {1.0, 0.0, 0.2, 0.6};
//const	float	kNavLightGreen[] = {0.0, 1.0, 0.3, 0.6};
//const	float	kLandingLight[]	= {1.0, 1.0, 0.7, 0.6};
//const	float	kStrobeLight[]	= {1.0, 1.0, 1.0, 0.4};
const	float	kNavLightRed[] = {1.0, 0.0, 0.2, 0.5};
const	float	kNavLightGreen[] = {0.0, 1.0, 0.3, 0.5};
const	float	kLandingLight[]	= {1.0, 1.0, 0.7, 0.6};
const	float	kStrobeLight[]	= {1.0, 1.0, 1.0, 0.7};

static	int sLightTexture = -1;

static void MakePartialPathNativeObj(string& io_str)
{
//	char sep = *XPLMGetDirectorySeparator();
	for(int i = 0; i < io_str.size(); ++i)
	if(io_str[i] == '/' || io_str[i] == ':' || io_str[i] == '\\')
		io_str[i] = '/';
}

static	XPLMDataRef sFOVRef = XPLMFindDataRef("sim/graphics/view/field_of_view_deg");
static	float		sFOV = 60.0;

bool 	NormalizeVec(float vec[3])
{
	float	len=sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
	if (len>0.0)
	{
		len = 1.0 / len;
		vec[0] *= len;
		vec[1] *= len;
		vec[2] *= len;
		return true;
	}
	return false;
}

void	CrossVec(float a[3], float b[3], float dst[3])
{
  dst[0] = a[1] * b[2] - a[2] * b[1] ;
  dst[1] = a[2] * b[0] - a[0] * b[2] ;
  dst[2] = a[0] * b[1] - a[1] * b[0] ;
}


/*****************************************************
			Ben's Crazy Point Pool Class
******************************************************/

class	OBJ_PointPool {
public:
		OBJ_PointPool(){};
		~OBJ_PointPool(){};

	 int AddPoint(float xyz[3], float st[2]);
	void PreparePoolToDraw();
	void CalcTriNormal(int idx1, int idx2, int idx3);
	void NormalizeNormals(void);
	void DebugDrawNormals();
	void Purge() { mPointPool.clear(); }
	 int Size() { return mPointPool.size(); }
private:
	vector<float>	mPointPool;
};

		

// Adds a point to our pool and returns it's index.
// If one already exists in the same location, we
// just return that index
int	OBJ_PointPool::AddPoint(float xyz[3], float st[2])
{
#if !DISABLE_SHARING
	// Use x as the key...see if we can find it
	for(int n = 0; n < mPointPool.size(); n += 8)
	{
		if((xyz[0] == mPointPool[n]) &&
		   (xyz[1] == mPointPool[n+1]) &&
		   (xyz[2] == mPointPool[n+2]) &&
		   (st[0] == mPointPool[n+3]) &&
		   (st[1] == mPointPool[n+4]))
				return n/8;	// Clients care about point # not array index
	}
#endif	

	// If we're here, no match was found so we add it to the pool
	// Add XYZ
	mPointPool.push_back(xyz[0]);	mPointPool.push_back(xyz[1]);	mPointPool.push_back(xyz[2]);
	// Add ST
	mPointPool.push_back(st[0]); mPointPool.push_back(st[1]);
	// Allocate some space for the normal later
	mPointPool.push_back(0.0); mPointPool.push_back(0.0); mPointPool.push_back(0.0);
	return (mPointPool.size()/8)-1;
}

// This function sets up OpenGL for our point pool
void OBJ_PointPool::PreparePoolToDraw()
{
	// Setup our triangle data (20 represents 5 elements of 4 bytes each
	// namely s,t,xn,yn,zn)
	glVertexPointer(3, GL_FLOAT, 32, &(*mPointPool.begin()));
	// Set our texture data (24 represents 6 elements of 4 bytes each
	// namely xn, yn, zn, x, y, z. We start 3 from the beginning to skip
	// over x, y, z initially.
	glClientActiveTextureARB(GL_TEXTURE1);
	glTexCoordPointer(2, GL_FLOAT, 32, &(*(mPointPool.begin() + 3)));
	glClientActiveTextureARB(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 32, &(*(mPointPool.begin() + 3)));
	// Set our normal data...
	glNormalPointer(GL_FLOAT, 32, &(*(mPointPool.begin() + 5)));
}

void OBJ_PointPool::CalcTriNormal(int idx1, int idx2, int idx3)
{
	if (mPointPool[idx1*8  ]==mPointPool[idx2*8  ]&&
		mPointPool[idx1*8+1]==mPointPool[idx2*8+1]&&
		mPointPool[idx1*8+2]==mPointPool[idx2*8+2])		return;
	if (mPointPool[idx1*8  ]==mPointPool[idx3*8  ]&&
		mPointPool[idx1*8+1]==mPointPool[idx3*8+1]&&
		mPointPool[idx1*8+2]==mPointPool[idx3*8+2])		return;
	if (mPointPool[idx2*8  ]==mPointPool[idx3*8  ]&&
		mPointPool[idx2*8+1]==mPointPool[idx3*8+1]&&
		mPointPool[idx2*8+2]==mPointPool[idx3*8+2])		return;

	// idx2->idx1 cross idx1->idx3 = normal product
	float	v1[3], v2[3], n[3];
	v1[0] = mPointPool[idx2*8  ] - mPointPool[idx1*8  ];
	v1[1] = mPointPool[idx2*8+1] - mPointPool[idx1*8+1];
	v1[2] = mPointPool[idx2*8+2] - mPointPool[idx1*8+2];

	v2[0] = mPointPool[idx2*8  ] - mPointPool[idx3*8  ];
	v2[1] = mPointPool[idx2*8+1] - mPointPool[idx3*8+1];
	v2[2] = mPointPool[idx2*8+2] - mPointPool[idx3*8+2];
	
	// We do NOT normalize the cross product; we want larger triangles
	// to make bigger normals.  When we blend them, bigger sides will
	// contribute more to the normals.  We'll normalize the normals
	// after the blend is done.
	CrossVec(v1, v2, n);
	mPointPool[idx1*8+5] += n[0];
	mPointPool[idx1*8+6] += n[1];
	mPointPool[idx1*8+7] += n[2];

	mPointPool[idx2*8+5] += n[0];
	mPointPool[idx2*8+6] += n[1];
	mPointPool[idx2*8+7] += n[2];

	mPointPool[idx3*8+5] += n[0];
	mPointPool[idx3*8+6] += n[1];
	mPointPool[idx3*8+7] += n[2];
}

inline void swapped_add(float& a, float& b)
{
	float a_c = a;
	float b_c = b;
	a += b_c;
	b += a_c;
}

void OBJ_PointPool::NormalizeNormals(void)
{
	// Average all normals of same-point, different texture points?  Why is this needed?
	// Well...the problem is that we get non-blended normals around the 'seam' where the ACF fuselage
	// is put together...at this point the separate top and bottom texes touch.  Their color will
	// be the same but the S&T coords won't.  If we have slightly different normals and the sun is making
	// shiney specular hilites, the discontinuity is real noticiable.
#if BLEND_NORMALS
	for (int n = 0; n < mPointPool.size(); n += 8)
	{
		for (int m = 0; m < mPointPool.size(); m += 8)
		if (mPointPool[n  ]==mPointPool[m  ] &&
			mPointPool[n+1]==mPointPool[m+1] &&
			mPointPool[n+2]==mPointPool[m+2] &&
			m != n)
		{
			swapped_add(mPointPool[n+5], mPointPool[m+5]);
			swapped_add(mPointPool[n+6], mPointPool[m+6]);
			swapped_add(mPointPool[n+7], mPointPool[m+7]);
		}
	}
#endif	
	for (int n = 5; n < mPointPool.size(); n += 8)
	{
		NormalizeVec(&mPointPool[n]);
	}
}

// This is a debug routine that will draw each vertex's normals
void OBJ_PointPool::DebugDrawNormals()
{
	XPLMSetGraphicsState(0, 0, 0, 0, 0, 1, 0);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_LINES);
		for(int n = 0; n < mPointPool.size(); n+=8)
		{
			glVertex3f(mPointPool[n], mPointPool[n+1], mPointPool[n+2]);
			glVertex3f(mPointPool[n] + mPointPool[n+5], mPointPool[n+1] + mPointPool[n+1+5],
					   mPointPool[n+2] + mPointPool[n+2+5]);
		}
	glEnd();
}

/*****************************************************
			Object and Struct Definitions
******************************************************/
static	map<string, int>	sTexes;

struct	LightInfo_t {
	float			xyz[3];
	int				rgb[3];
};

// One of these structs per LOD read from the OBJ file
struct	LODObjInfo_t {
	
	float					nearDist;	// The visible range
	float					farDist;	// of this LOD
	vector<int>				triangleList;
	vector<LightInfo_t>		lights;
	OBJ_PointPool			pointPool;
	GLuint					dl;
};

// One of these structs per OBJ file
struct	ObjInfo_t {

	string					path;
	int						texnum;
	int						texnum_lit;
	XObj					obj;
	vector<LODObjInfo_t>	lods;
};

static vector<ObjInfo_t>	sObjects;

/*****************************************************
		Utility functions to handle OBJ stuff
******************************************************/

int	   OBJ_LoadTexture(const char * inFilePath, bool inForceMaxTex)
{
	string	path(inFilePath);
	if (sTexes.count(path) > 0)
		return sTexes[path];
	
	int texNum;
	XPLMGenerateTextureNumbers(&texNum, 1);
	
	int derez = 5 - gIntPrefsFunc("planes", "resolution", 3);
	if (inForceMaxTex) 
		derez = 0;
	bool ok = LoadTextureFromFile(path.c_str(), texNum, true, false, true, NULL, NULL, derez);
	if (!ok) return 0;
	
	sTexes[path] = texNum;
	return texNum;
}

bool	OBJ_Init(const char * inTexturePath)
{
	// Now that we've successfully loaded an aircraft,
	// we can load our lighting texture
	static bool firstTime = true;
	if(firstTime)
	{
		sLightTexture = OBJ_LoadTexture(inTexturePath, true);
		firstTime = false;
	}
	return sLightTexture != 0;
}

// Load one model - return -1 if it can't be loaded.
int		OBJ_LoadModel(const char * inFilePath)
{
	string path(inFilePath);
	
	for (int n = 0; n < sObjects.size(); ++n)
	{
		if (path == sObjects[n].path)
			return n;
	}
	
	sObjects.push_back(ObjInfo_t());
	bool ok = XObjRead(path.c_str(), sObjects.back().obj);
	if (!ok)
	{
		sObjects.pop_back();
		return -1;
	}
	
	MakePartialPathNativeObj(sObjects.back().obj.texture);
	
	sObjects.back().path = path;
	string tex_path(path);
	string::size_type p = tex_path.find_last_of("\\:/");//XPLMGetDirectorySeparator());
	tex_path.erase(p+1);
	tex_path += sObjects.back().obj.texture;
	tex_path += ".png";
	sObjects.back().texnum = OBJ_LoadTexture(tex_path.c_str(), false);

	tex_path = path;
	p = tex_path.find_last_of("\\:/");//XPLMGetDirectorySeparator());
	tex_path.erase(p+1);
	tex_path += sObjects.back().obj.texture;
	tex_path += "_LIT.png";
	sObjects.back().texnum_lit = OBJ_LoadTexture(tex_path.c_str(), false);

	// We prescan all of the commands to see if there's ANY LOD. If there's
	// not then we need to add one ourselves. If there is, we will find it
	// naturally later.
	bool foundLOD = false;
	for (vector<XObjCmd>::iterator cmd = sObjects.back().obj.cmds.begin();
		cmd != sObjects.back().obj.cmds.end(); ++cmd)
	{
		if((cmd->cmdType == type_Attr) && (cmd->cmdID == attr_LOD))
			foundLOD = true;
	}
	if(foundLOD == false)
	{
		sObjects.back().lods.push_back(LODObjInfo_t());
		sObjects.back().lods.back().nearDist = 0;
		sObjects.back().lods.back().farDist = 40000;
	}
	// Go through all of the commands for this object and filter out the polys
	// and the lights.
	for (vector<XObjCmd>::iterator cmd = sObjects.back().obj.cmds.begin();
		cmd != sObjects.back().obj.cmds.end(); ++cmd)
	{
		switch(cmd->cmdType) {
		case type_Attr:
			if(cmd->cmdID == attr_LOD)
			{
				// We've found a new LOD section so save this
				// information in a new struct. From now on and until
				// we hit this again, all data is for THIS LOD instance.
				sObjects.back().lods.push_back(LODObjInfo_t());
				// Save our visible LOD range
				sObjects.back().lods.back().nearDist = cmd->attributes[0];
				sObjects.back().lods.back().farDist = cmd->attributes[1];
			}
			break;
		case type_PtLine:
			if(cmd->cmdID == obj_Light)
			{
				// For each light we've found, copy the data into our
				// own light vector
				for(int n = 0; n < cmd->rgb.size(); n++)
				{
					sObjects.back().lods.back().lights.push_back(LightInfo_t());
					sObjects.back().lods.back().lights.back().xyz[0] = cmd->rgb[n].v[0];
					sObjects.back().lods.back().lights.back().xyz[1] = cmd->rgb[n].v[1];
					sObjects.back().lods.back().lights.back().xyz[2] = cmd->rgb[n].v[2];
					sObjects.back().lods.back().lights.back().rgb[0] = cmd->rgb[n].rgb[0];
					sObjects.back().lods.back().lights.back().rgb[1] = cmd->rgb[n].rgb[1];
					sObjects.back().lods.back().lights.back().rgb[2] = cmd->rgb[n].rgb[2];
				}
			}
			break;
		case type_Poly:
			{
				vector<int> indexes;
				// First get our point pool setup with all verticies
				for(int n = 0; n < cmd->st.size(); n++)
				{
					float xyz[3], st[2];
					int index;
					
					xyz[0] = cmd->st[n].v[0];
					xyz[1] = cmd->st[n].v[1];
					xyz[2] = cmd->st[n].v[2];
					st[0]  = cmd->st[n].st[0];
					st[1]  = cmd->st[n].st[1];
					index = sObjects.back().lods.back().pointPool.AddPoint(xyz, st);
					indexes.push_back(index);
				}
				
				switch(cmd->cmdID) {
				case obj_Tri:
					for(int n = 0; n < indexes.size(); ++n)
					{
						sObjects.back().lods.back().triangleList.push_back(indexes[n]);
					}
					break;
				case obj_Tri_Fan:
					for(int n = 2; n < indexes.size(); n++)
					{
							sObjects.back().lods.back().triangleList.push_back(indexes[0  ]);
							sObjects.back().lods.back().triangleList.push_back(indexes[n-1]);
							sObjects.back().lods.back().triangleList.push_back(indexes[n  ]);						
					}
					break;
				case obj_Tri_Strip:
				case obj_Quad_Strip:
					for(int n = 2; n < indexes.size(); n++)
					{
						if((n % 2) == 1)
						{
							sObjects.back().lods.back().triangleList.push_back(indexes[n - 2]);
							sObjects.back().lods.back().triangleList.push_back(indexes[n]);
							sObjects.back().lods.back().triangleList.push_back(indexes[n - 1]);
						}
						else
						{
							sObjects.back().lods.back().triangleList.push_back(indexes[n - 2]);
							sObjects.back().lods.back().triangleList.push_back(indexes[n - 1]);
							sObjects.back().lods.back().triangleList.push_back(indexes[n]);
						}
					}
					break;
				case obj_Quad:
					for(int n = 3; n < indexes.size(); n += 4)
					{
						sObjects.back().lods.back().triangleList.push_back(indexes[n-3]);
						sObjects.back().lods.back().triangleList.push_back(indexes[n-2]);
						sObjects.back().lods.back().triangleList.push_back(indexes[n-1]);
						sObjects.back().lods.back().triangleList.push_back(indexes[n-3]);
						sObjects.back().lods.back().triangleList.push_back(indexes[n-1]);
						sObjects.back().lods.back().triangleList.push_back(indexes[n  ]);
					}					
					break;
				}
			}
			break;
		}
	}
	
	// Calculate our normals for all LOD's
	for (int i = 0; i < sObjects.back().lods.size(); i++)
	{
		for (int n = 0; n < sObjects.back().lods[i].triangleList.size(); n += 3)
		{
			sObjects.back().lods[i].pointPool.CalcTriNormal(
									sObjects.back().lods[i].triangleList[n],
									sObjects.back().lods[i].triangleList[n+1],
									sObjects.back().lods[i].triangleList[n+2]);
		}
		sObjects.back().lods[i].pointPool.NormalizeNormals();
		sObjects.back().lods[i].dl = 0;
	}
	sObjects.back().obj.cmds.clear();
	return sObjects.size()-1;
}

/*****************************************************
			Aircraft Model Drawing
******************************************************/
// Note that texID and litTexID are OPTIONAL! They will only be filled
// in if the user wants to override the default texture specified by the
// obj file
void	OBJ_PlotModel(int model, int texID, int litTexID, float inDistance, double inX,
					  double inY, double inZ, double inPitch, double inRoll, double inHeading)
{
	int tex, lit;
	// Find out what LOD we need to draw
	int lodIdx = -1;
	for(int n = 0; n < sObjects[model].lods.size(); n++)
	{
		if((inDistance >= sObjects[model].lods[n].nearDist) &&
		   (inDistance <= sObjects[model].lods[n].farDist))
		{
		   lodIdx = n;
		   break;
		}
	}
	// If we didn't find a good LOD bin, we don't draw!
	if(lodIdx == -1)
		return;

	// pointPool is and always was empty! returning early
	if(sObjects[model].lods[lodIdx].pointPool.Size()==0 && sObjects[model].lods[lodIdx].dl == 0)
		return;

	static XPLMDataRef	night_lighting_ref = XPLMFindDataRef("sim/graphics/scenery/percent_lights_on");
	bool	use_night = XPLMGetDataf(night_lighting_ref) > 0.25;

	if (model == -1) return;

	if(texID)
	{
		tex = texID;
		lit = litTexID;
	}
	else
	{
		tex = sObjects[model].texnum;
		lit = sObjects[model].texnum_lit;
	}
	if (!use_night)	lit = 0;
	if (tex == 0) lit = 0;
	XPLMSetGraphicsState(1, (tex != 0) + (lit != 0), 1, 1, 1, 1, 1);
	if (tex != 0)	XPLMBindTexture2d(tex, 0);
	if (lit != 0)	XPLMBindTexture2d(lit, 1);
	
	if (tex) { glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); }
	if (lit) { glActiveTextureARB(GL_TEXTURE1); glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD); glActiveTextureARB(GL_TEXTURE0); }
	
	if (sObjects[model].lods[lodIdx].dl == 0)
	{
		sObjects[model].lods[lodIdx].dl = glGenLists(1);
		
		GLint xpBuffer;
		// See if the card even has VBO. If it does, save xplane's pointer
		// and bind to 0 for us.
		if(glBindBufferARB)
		{
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &xpBuffer); 
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
		// Save XPlanes OpenGL state
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		// Setup OpenGL pointers to our pool
		sObjects[model].lods[lodIdx].pointPool.PreparePoolToDraw();
		// Enable vertex data sucking
		glEnableClientState(GL_VERTEX_ARRAY);
		// Enable normal array sucking
		glEnableClientState(GL_NORMAL_ARRAY);
		// Enable texture coordinate data sucking
		glClientActiveTextureARB(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTextureARB(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		// Disable colors - maybe x-plane left it around.
		glDisableClientState(GL_COLOR_ARRAY);
	
		glNewList(sObjects[model].lods[lodIdx].dl, GL_COMPILE);
		// Kick OpenGL and draw baby!
		glDrawElements(GL_TRIANGLES, sObjects[model].lods[lodIdx].triangleList.size(), 
						GL_UNSIGNED_INT, &(*sObjects[model].lods[lodIdx].triangleList.begin()));	

#if DEBUG_NORMALS
		sObjects[model].lods[lodIdx].pointPool.DebugDrawNormals();
		XPLMSetGraphicsState(1, (tex != 0) + (lit != 0), 1, 1, 1, 1, 1);		
#endif

		glEndList();

		// Disable vertex data sucking
		glDisableClientState(GL_VERTEX_ARRAY);
		// Disable texture coordinate data sucking
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		// Disable normal array sucking
		glDisableClientState(GL_NORMAL_ARRAY);

		// Restore Xplane's OpenGL State
		glPopClientAttrib();

		// If we bound before, we need to put xplane back where it was
		if(glBindBufferARB)
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, xpBuffer);

		sObjects[model].lods[lodIdx].triangleList.clear();
		sObjects[model].lods[lodIdx].pointPool.Purge();
	}
	glCallList(sObjects[model].lods[lodIdx].dl);


}

/*****************************************************
			Textured Lights Drawing

 Draw one or more lights on our OBJ.
 RGB of 11,11,11 is a RED NAV light
 RGB of 22,22,22 is a GREEN NAV light
 RGB of 33,33,33 is a Red flashing BEACON light
 RGB of 44,44,44 is a White flashing STROBE light
 RGB of 55,55,55 is a landing light
******************************************************/
void	OBJ_BeginLightDrawing()
{
	sFOV = XPLMGetDataf(sFOVRef);

	// Setup OpenGL for the drawing
	XPLMSetGraphicsState(1, 1, 0,   1, 1 ,   1, 0);
	XPLMBindTexture2d(sLightTexture, 0);
}

void	OBJ_DrawLights(int model, float inDistance, double inX, double inY,
					   double inZ, double inPitch, double inRoll, double inHeading,
					   xpmp_LightStatus lights)
{
	bool navLights = lights.navLights == 1;
	bool bcnLights = lights.bcnLights == 1;
	bool strbLights = lights.strbLights == 1;
	bool landLights = lights.landLights == 1;

	int offset = lights.timeOffset;

	// flash frequencies
	if(bcnLights) {
		bcnLights = false;
		int x = (int)(XPLMGetElapsedTime() * 1000 + offset) % 1200;
		switch(lights.flashPattern) {
			case xpmp_Lights_Pattern_EADS: 
				// EADS pattern: two flashes every 1.2 seconds
				if(x < 120 || ((x > 240 && x < 360))) bcnLights = true;
				break;

			case xpmp_Lights_Pattern_GA:
				// GA pattern: 900ms / 1200ms
				if((((int)(XPLMGetElapsedTime() * 1000 + offset) % 2100) < 900)) bcnLights = true;
				break;

			case xpmp_Lights_Pattern_Default: 
			default:
				// default pattern: one flash every 1.2 seconds
				if(x < 120) bcnLights = true;
				break;
		}

	}
	if(strbLights) {
		strbLights = false;
		int x = (int)(XPLMGetElapsedTime() * 1000 + offset) % 1700;
		switch(lights.flashPattern) {
			case xpmp_Lights_Pattern_EADS: 
				if(x < 80 || (x > 260 && x < 340)) strbLights = true;
				break;

			case xpmp_Lights_Pattern_GA: 
				// similar to the others.. but a little different frequency :)
				x = (int)(XPLMGetElapsedTime() * 1000 + offset) % 1900;
				if(x < 100) strbLights = true;
				break;

			case xpmp_Lights_Pattern_Default:
			default:
				if(x < 80) strbLights = true;
				break;
		}
	}

	// Find out what LOD we need to draw
	int lodIdx = -1;
	for(int n = 0; n < sObjects[model].lods.size(); n++)
	{
		if((inDistance >= sObjects[model].lods[n].nearDist) &&
		   (inDistance <= sObjects[model].lods[n].farDist))
		{
		   lodIdx = n;
		   break;
		}
	}
	// If we didn't find a good LOD bin, we don't draw!
	if(lodIdx == -1)
		return;

	double size, distance;
	// Where are we looking?
	XPLMCameraPosition_t cameraPos;
	XPLMReadCameraPosition(&cameraPos);
	
	// We can have 1 or more lights on each aircraft
	for(int n = 0; n < sObjects[model].lods[lodIdx].lights.size(); n++)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		// First we translate to our coordinate system and move the origin
		// to the center of our lights.
		glTranslatef(sObjects[model].lods[lodIdx].lights[n].xyz[0],
					sObjects[model].lods[lodIdx].lights[n].xyz[1],
					sObjects[model].lods[lodIdx].lights[n].xyz[2]);

		// Now we undo the rotation of the plane
		glRotated(-inRoll, 0.0, 0.0, -1.0);
		glRotated(-inPitch, 1.0, 0.0, 0.0);
		glRotated(-inHeading, 0.0, -1.0, 0.0);

		// Now we undo the rotation of the camera
		// NOTE: The order and sign of the camera is backwards
		// from what we'd expect (the plane rotations) because
		// the camera works backwards. If you pan right, everything
		// else moves left!
		glRotated(cameraPos.heading, 0.0, -1.0, 0.0);
		glRotated(cameraPos.pitch, 1.0, 0.0, 0.0);
		glRotated(cameraPos.roll, 0.0, 0.0, -1.0);

		// Find our distance from the camera
		float dx = cameraPos.x - inX;
		float dy = cameraPos.y - inY;
		float dz = cameraPos.z - inZ;
		distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));

		// Convert to NM
		distance *= kMetersToNM;

		// Scale based on our FOV and Zoom. I did my initial
		// light adjustments at a FOV of 60 so thats why
		// I divide our current FOV by 60 to scale it appropriately.
		distance *= sFOV/60.0;
		distance /= cameraPos.zoom;

		// Calculate our light size. This is piecewise linear. I noticed
		// that light size changed more rapidly when closer than 3nm so
		// I have a separate equation for that.
		if(distance <= 3.6)
			size = (10 * distance) + 1;
		else
			size = (6.7 * distance) + 12;

		// Finally we can draw our lights
		// Red Nav
		glBegin(GL_QUADS);
		if((sObjects[model].lods[lodIdx].lights[n].rgb[0] == 11) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[1] == 11) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[2] == 11))
		{
			if(navLights) {
				glColor4fv(kNavLightRed);
				glTexCoord2f(0, 0.5); glVertex2f(-(size/2.0), -(size/2.0));
				glTexCoord2f(0, 1.0); glVertex2f(-(size/2.0), (size/2.0));
				glTexCoord2f(0.25, 1.0); glVertex2f((size/2.0), (size/2.0));
				glTexCoord2f(0.25, 0.5); glVertex2f((size/2.0), -(size/2.0));
			}
		}
		// Green Nav
		else if((sObjects[model].lods[lodIdx].lights[n].rgb[0] == 22) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[1] == 22) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[2] == 22))
		{
			if(navLights) {
				glColor4fv(kNavLightGreen);
				glTexCoord2f(0, 0.5); glVertex2f(-(size/2.0), -(size/2.0));
				glTexCoord2f(0, 1.0); glVertex2f(-(size/2.0), (size/2.0));
				glTexCoord2f(0.25, 1.0); glVertex2f((size/2.0), (size/2.0));
				glTexCoord2f(0.25, 0.5); glVertex2f((size/2.0), -(size/2.0));
			}
		}
		// Beacon
		else if((sObjects[model].lods[lodIdx].lights[n].rgb[0] == 33) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[1] == 33) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[2] == 33))
		{
				if(bcnLights)
				{
					glColor4fv(kNavLightRed);
					glTexCoord2f(0, 0.5); glVertex2f(-(size/2.0), -(size/2.0));
					glTexCoord2f(0, 1.0); glVertex2f(-(size/2.0), (size/2.0));
					glTexCoord2f(0.25, 1.0); glVertex2f((size/2.0), (size/2.0));
					glTexCoord2f(0.25, 0.5); glVertex2f((size/2.0), -(size/2.0));
				}
		}
		// Strobes
		else if((sObjects[model].lods[lodIdx].lights[n].rgb[0] == 44) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[1] == 44) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[2] == 44))
		{
				if(strbLights)
				{
					glColor4fv(kStrobeLight);
					glTexCoord2f(0.25, 0.0); glVertex2f(-(size/1.5), -(size/1.5));
					glTexCoord2f(0.25, 0.5); glVertex2f(-(size/1.5), (size/1.5));
					glTexCoord2f(0.50, 0.5); glVertex2f((size/1.5), (size/1.5));
					glTexCoord2f(0.50, 0.0); glVertex2f((size/1.5), -(size/1.5));
				}
		}
		// Landing Lights
		else if((sObjects[model].lods[lodIdx].lights[n].rgb[0] == 55) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[1] == 55) &&
		   (sObjects[model].lods[lodIdx].lights[n].rgb[2] == 55))
		{
			if(landLights) {
			// BEN SEZ: modulate the _alpha to make this dark, not
			// the light color.  Otherwise if the sky is fairly light the light
			// will be darker than the sky, which looks f---ed during the day.
				float color[4];
				color[0] = kLandingLight[0];
				if(color[0] < 0.0) color[0] = 0.0;
				color[1] = kLandingLight[1];
				if(color[0] < 0.0) color[0] = 0.0;
				color[2] = kLandingLight[2];
				if(color[0] < 0.0) color[0] = 0.0;
				color[3] = kLandingLight[3] * ((distance * -0.05882) + 1.1764);
				glColor4fv(color);
				glTexCoord2f(0.25, 0.0); glVertex2f(-(size/2.0), -(size/2.0));
				glTexCoord2f(0.25, 0.5); glVertex2f(-(size/2.0), (size/2.0));
				glTexCoord2f(0.50, 0.5); glVertex2f((size/2.0), (size/2.0));
				glTexCoord2f(0.50, 0.0); glVertex2f((size/2.0), -(size/2.0));
			}
		} else {
			// rear nav light and others? I guess...
			if(navLights) {
				glColor3f(
					sObjects[model].lods[lodIdx].lights[n].rgb[0] * 0.1,
					sObjects[model].lods[lodIdx].lights[n].rgb[1] * 0.1,
					sObjects[model].lods[lodIdx].lights[n].rgb[2] * 0.1);
				glTexCoord2f(0, 0.5); glVertex2f(-(size/2.0), -(size/2.0));
				glTexCoord2f(0, 1.0); glVertex2f(-(size/2.0), (size/2.0));
				glTexCoord2f(0.25, 1.0); glVertex2f((size/2.0), (size/2.0));
				glTexCoord2f(0.25, 0.5); glVertex2f((size/2.0), -(size/2.0));
			}
		}
		glEnd();
		// Put OpenGL back how we found it
		glPopMatrix();
	}
}

int		OBJ_GetModelTexID(int model)
{
	return sObjects[model].texnum;
}
