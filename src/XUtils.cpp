/* 
 * Copyright (c) 2004, Laminar Research.
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
#include "XUtils.h"
#include <stdio.h>
#include <ctype.h>
#include "XObjDefs.h"
#include <stdlib.h>
//#include "PlatformUtils.h"
//#include <time.h>
#include <map>
#include <math.h>
#include <string.h>

#if !defined(XUTILS_EXCLUDE_MAC_CRAP) && defined(__MACH__)
#define XUTILS_EXCLUDE_MAC_CRAP 1
#endif

#if APL
//using namespace Metrowerks;
#endif

static char * my_fgets(char * s, int n, FILE * file)
{
	char *	p = s;
	int			c;
	
	if (--n < 0)
		return(NULL);
	
	if (n)
		do
		{
			c = fgetc(file);
			
			if (c == EOF)
				if (/*feof(file) &&*/ p != s)
					break;
				else
				{
					return(NULL);
				}
			
			*p++ = c;
		}
		while (c != '\n' && c != '\r' && --n);
	
	*p = 0;
	
	return(s);
}



StTextFileScanner::StTextFileScanner(const char * file, bool skip) :
	mFile(fopen(file, "r")),
	mDone(false),
	mSkipBlanks(skip)
{
	read_next();
}	

StTextFileScanner::~StTextFileScanner()
{
	if (mFile)
		fclose(mFile);
}

bool	StTextFileScanner::done()
{
	return mDone;
}

void	StTextFileScanner::next()
{
	read_next();
}

string	StTextFileScanner::get()
{
	return mBuf;
}

void	StTextFileScanner::read_next(void)
{
	mBuf.clear();
	mDone = true;

	char	buf[4096];

	while (mFile && /*!feof(mFile) &&*/ my_fgets(buf, sizeof(buf), mFile))
	{
		int len = strlen(buf);
		while ((len > 0) && (buf[len-1] == '\r' || buf[len-1] == '\n'))
		{
			buf[len-1] = 0;
			--len;
		}
		
		if (buf[0] == '\r' || buf[0] == '\n')
			mBuf = buf+1;
		else
			mBuf = buf;
			
		if (!mBuf.empty() || !mSkipBlanks)
		{
			mDone = false;
			return;
		}
	}
}

void	BreakString(const string& line, vector<string>& words)
{
	words.clear();
	string::const_iterator i = line.begin();
	while (i < line.end())
	{
		string::const_iterator s = i;
		while (s < line.end() && isspace(*s))
			++s;
		
		string::const_iterator e = s;
		while (e < line.end() && !isspace(*e))
			++e;
			
		if (s < e)
			words.push_back(string(s,e));
		
		i = e;
	}
}

void	StringToUpper(string& s)
{	
	for (string::iterator i = s.begin(); i != s.end(); ++i)
		*i = toupper(*i);
}

bool	HasExtNoCase(const string& inStr, const char * inExt)
{
	string s(inStr);
	string e(inExt);
	StringToUpper(s);
	StringToUpper(e);
	
	
	if (s.rfind(e) == (s.length() - e.length()))
		return true;
	return false;
}

void	ChangePolyCmdCW(XObjCmd& ioCmd)
{
	vector<vec_tex>	v;
	for (vector<vec_tex>::reverse_iterator riter = ioCmd.st.rbegin();
		riter != ioCmd.st.rend(); ++riter)
	{
		v.push_back(*riter);
	}
	ioCmd.st = v;
}

bool	GetNextNoComments(StTextFileScanner& f, string& s)
{
	while(!f.done())
	{
		s = f.get();
		f.next();
		if (s.empty() || s[0] != '#')
			return true;		
	}
	return false;
}

double	GetObjRadius(const XObj& inObj)
{
	double	dist = 0, d;
	for (vector<XObjCmd>::const_iterator c = inObj.cmds.begin();
		c != inObj.cmds.end(); ++c)	
	{
		for (vector<vec_tex>::const_iterator v = c->st.begin();
			v != c->st.end(); ++v)
		{
			d = sqrt(v->v[0] * v->v[0] +
					 v->v[1] * v->v[1] +
					 v->v[2] * v->v[2]);
			if (d > dist) dist = d;
		}
		
		for (vector<vec_rgb>::const_iterator p = c->rgb.begin();
			p != c->rgb.end(); ++p)
		{
			d = sqrt(p->v[0] * p->v[0] +
					 p->v[1] * p->v[1] +
					 p->v[2] * p->v[2]);
			if (d > dist) dist = d;
		}
	}
	return dist;
}

int		PickRandom(vector<double>& chances)
{
	double	v = (double) (rand() % RAND_MAX) / (double) RAND_MAX;
	
	for (int n = 0; n < chances.size(); ++n)
	{
		if (v < chances[n])
			return n;
		v -= chances[n];
	}
	return chances.size();
}

bool	RollDice(double inProb)
{
	if (inProb <= 0.0) return false;
	if (inProb >= 1.0) return true;
	double	v = (double) (rand() % RAND_MAX) / (double) RAND_MAX;
	return v < inProb;
}

double	RandRange(double mmin, double mmax)
{
	if (mmin >= mmax)
		return mmin;
	double	v = (double) rand() / (double) RAND_MAX;
	return mmin + ((mmax - mmin) * v);
}		

double	RandRangeBias(double mmin, double mmax, double biasRatio, double randomAmount)
{
	double	span = mmax - mmin;
	double lower_rat = biasRatio * (1.0 - randomAmount);
	double upper_rat = lower_rat + randomAmount;
	return RandRange(mmin + span * lower_rat,mmin + span * upper_rat);
}

#if 0
void		StripPath(string& ioPath)
{
	string::size_type sep = ioPath.rfind(DIR_CHAR);
	if (sep != ioPath.npos)
		ioPath = ioPath.substr(sep+1,ioPath.npos);
}

void		StripPathCP(string& ioPath)
{
	string::size_type sep;
	sep = ioPath.rfind(':');
	if (sep != ioPath.npos)
		ioPath = ioPath.substr(sep+1,ioPath.npos);
	sep = ioPath.rfind('\\');
	if (sep != ioPath.npos)
		ioPath = ioPath.substr(sep+1,ioPath.npos);
}

void		ExtractPath(string& ioPath)
{
	string::size_type sep = ioPath.rfind(DIR_CHAR);
	if (sep != ioPath.npos)
		ioPath = ioPath.substr(0, sep);
}
#endif
#if APL

#if !defined(XUTILS_EXCLUDE_MAC_CRAP)

#include <Processes.h>

OSErr	FindSuperFolder(const FSSpec& inItem, FSSpec& outFolder)
{
		CInfoPBRec	paramBlock;
		OSErr		err;
		
	paramBlock.dirInfo.ioCompletion = 	NULL;
	paramBlock.dirInfo.ioNamePtr =		(StringPtr) (&(outFolder.name));
	paramBlock.dirInfo.ioVRefNum = 		inItem.vRefNum;
	paramBlock.dirInfo.ioFDirIndex = 	-1;
	paramBlock.dirInfo.ioDrDirID = 		inItem.parID;
	
	err = ::PBGetCatInfoSync(&paramBlock);
	if (err != noErr)
		return err;
		
	outFolder.vRefNum = paramBlock.dirInfo.ioVRefNum;
	outFolder.parID= paramBlock.dirInfo.ioDrParID;
	return noErr;
}

void	AppPath(string& outString)
{
	ProcessSerialNumber	psn = { 0, kCurrentProcess };
	ProcessInfoRec info = { 0 };
	FSSpec	spec;
	Str255	name;
	info.processInfoLength = sizeof(info);
	info.processAppSpec = &spec;
	info.processName = name;
	GetProcessInformation(&psn, &info);
	FSSpec_2_String(spec, outString);
}

void	FSSpec_2_String(const FSSpec& inSpec, string& outString)
{
	outString.clear();
	
	FSSpec	foo(inSpec);
	FSSpec	foo2;
	
	while (1)
	{
		if (outString.empty())
			outString = std::string(foo.name+1, foo.name+foo.name[0]+1);
		else
			outString = std::string(foo.name+1, foo.name+foo.name[0]+1) + std::string(":") + outString;
		if (FindSuperFolder(foo, foo2) != noErr)
			break;
		foo = foo2;
	}
}

#endif
#endif

void	ExtractFixedRecordString(	
				const string&		inLine,
				int					inBegin,
				int					inEnd,
				string&				outString)
{
	int	sp = inBegin-1;
	int ep = inEnd;
	if (ep > inLine.length()) ep = inLine.length();
	if (sp > inLine.length()) sp = inLine.length();
	
	while ((sp < ep) && (inLine[sp] == ' '))
		++sp;
	
	while ((ep > sp) && (inLine[ep-1] == ' '))
		--ep;
		
	outString = inLine.substr(sp, ep - sp);
}				
				
bool	ExtractFixedRecordLong(
				const string&		inLine,
				int					inBegin,
				int					inEnd,
				long&				outLong)
{
	string	foo;
	ExtractFixedRecordString(inLine, inBegin, inEnd, foo);
	if (foo.empty())	return false;
	outLong = strtol(foo.c_str(), NULL, 10);
	return true;
}				
				
bool	ExtractFixedRecordUnsignedLong(
				const string&		inLine,
				int					inBegin,
				int					inEnd,
				unsigned long&		outUnsignedLong)
{
	string	foo;
	ExtractFixedRecordString(inLine, inBegin, inEnd, foo);
	if (foo.empty())	return false;
	outUnsignedLong = strtoul(foo.c_str(), NULL, 10);
	return true;
}				

#pragma mark -

struct	XPointPool::XPointPoolImp {

	struct	p_info {
		float xyz[3];
		float st[2];
	};
	vector<p_info>			pts;
	map<string, int>		index;

	void	clear() 
	{
		pts.clear();
		index.clear();
	}
	
	int		count(void)
	{
		return pts.size();
	}
	
	int		accumulate(const float xyz[3], const float st[2])
	{
		static	char	buf[256];
		sprintf(buf,"%08X%08X%08X|%08x%08x",
			*(reinterpret_cast<const int*>(xyz+0)),
			*(reinterpret_cast<const int*>(xyz+1)),
			*(reinterpret_cast<const int*>(xyz+2)),
			*(reinterpret_cast<const int*>(st +0)),
			*(reinterpret_cast<const int*>(st +1)));
		string	key(buf);
		map<string, int>::iterator i = index.find(key);
		if (i != index.end()) return i->second;			
		p_info	p;
		memcpy(p.xyz, xyz, sizeof(p.xyz));
		memcpy(p.st, st, sizeof(p.st));
		pts.push_back(p);
		index.insert(map<string,int>::value_type(key, pts.size()));
		pts.push_back(p);
		return pts.size()-1;
	}
	
	void	get(int i, float xyz[3], float st[2])
	{
		p_info& p = pts[i];
		memcpy(xyz,p.xyz,sizeof(p.xyz));
		memcpy(st,p.st,sizeof(p.st));
	}
};

XPointPool::XPointPool()
{
	mImp = new XPointPoolImp;
}

XPointPool::~XPointPool()
{
	delete mImp;
}
		
void	XPointPool::clear()
{
	mImp->clear();
}

int		XPointPool::accumulate(const float xyz[3], const float st[2])
{
	return mImp->accumulate(xyz, st);
}

void	XPointPool::get(int index, float xyz[3], float st[2])
{
	mImp->get(index,xyz,st);
}

int		XPointPool::count(void)
{
	return mImp->count();
}

void	DecomposeObjCmd(const XObjCmd& inCmd, vector<XObjCmd>& outCmds, int maxValence)
{
	XObjCmd	c;
	c.cmdType = type_Poly;
	c.cmdID = obj_Tri;
	switch(inCmd.cmdID) {
	case obj_Tri:
		// Triangles never need breaking down.
		outCmds.push_back(inCmd);
		break;
	case obj_Quad:
	case obj_Quad_Hard:
	case obj_Smoke_Black:
	case obj_Smoke_White:
	case obj_Movie:
		// Quads - split into triangles if necessary.
		if (maxValence > 3) {
			outCmds.push_back(inCmd);
			outCmds.back().cmdID = obj_Quad;
		} else {
			outCmds.push_back(inCmd);
			outCmds.back().cmdID = obj_Tri;
			outCmds.back().st.erase(outCmds.back().st.begin()+3);
			outCmds.push_back(inCmd);			
			outCmds.back().cmdID = obj_Tri;
			outCmds.back().st.erase(outCmds.back().st.begin()+1);
		}
		break;
	case obj_Polygon:
		// Polygons might be ok.  But if we have to break them down,
		// we generate N-2 triangles in a fan configuration.
		if (maxValence < inCmd.st.size())
		{
			c.st.push_back(inCmd.st[0]);
			c.st.push_back(inCmd.st[1]);
			c.st.push_back(inCmd.st[2]);
			for (int n = 2; n < inCmd.st.size(); ++n)
			{
				c.st[1] = inCmd.st[n-1];
				c.st[2] = inCmd.st[n  ];
				outCmds.push_back(c);
			}
		} else 
			outCmds.push_back(inCmd);
		break;
	case obj_Tri_Strip:
		// Triangle strips - every other triangle's vertices
		// are backward!
		c.st.push_back(inCmd.st[0]);
		c.st.push_back(inCmd.st[1]);
		c.st.push_back(inCmd.st[2]);
		for (int n = 2; n < inCmd.st.size(); ++n)
		{
			if (n%2)
			{
				c.st[0] = inCmd.st[n-2];
				c.st[1] = inCmd.st[n  ];
				c.st[2] = inCmd.st[n-1];
				outCmds.push_back(c);
			} else {
				c.st[0] = inCmd.st[n-2];
				c.st[1] = inCmd.st[n-1];
				c.st[2] = inCmd.st[n  ];
				outCmds.push_back(c);
			}
		}
		break;
	case obj_Tri_Fan:
		// Tri fan - run around the triangle fan emitting triangles.
		c.st.push_back(inCmd.st[0]);
		c.st.push_back(inCmd.st[1]);
		c.st.push_back(inCmd.st[2]);
		for (int n = 2; n < inCmd.st.size(); ++n)
		{
			c.st[1] = inCmd.st[n-1];
			c.st[2] = inCmd.st[n  ];
			outCmds.push_back(c);
		}
		break;
	case obj_Quad_Strip:
		// Quad strips can become either quads or triangles!!
		if (maxValence > 3)
		{
			c.cmdID = obj_Quad;
			c.st.push_back(inCmd.st[0]);
			c.st.push_back(inCmd.st[1]);
			c.st.push_back(inCmd.st[2]);
			c.st.push_back(inCmd.st[3]);
			for (int n = 2; n < inCmd.st.size(); n += 2)
			{
				c.st[0] = inCmd.st[n-2];
				c.st[1] = inCmd.st[n-1];
				c.st[2] = inCmd.st[n+1];
				c.st[3] = inCmd.st[n  ];
				outCmds.push_back(c);
			}
		} else {
			c.st.push_back(inCmd.st[0]);
			c.st.push_back(inCmd.st[1]);
			c.st.push_back(inCmd.st[2]);
			for (int n = 2; n < inCmd.st.size(); ++n)
			{
				if (n%2)
				{
					c.st[0] = inCmd.st[n-2];
					c.st[1] = inCmd.st[n  ];
					c.st[2] = inCmd.st[n-1];
					outCmds.push_back(c);
				} else {
					c.st[0] = inCmd.st[n-2];
					c.st[1] = inCmd.st[n-1];
					c.st[2] = inCmd.st[n  ];
					outCmds.push_back(c);
				}
			}
		}
		break;
	default:
		outCmds.push_back(inCmd);
	}
}

void	DecomposeObj(const XObj& inObj, XObj& outObj, int maxValence)
{
	outObj.cmds.clear();
	outObj.texture = inObj.texture;
	for (vector<XObjCmd>::const_iterator cmd = inObj.cmds.begin(); 
		cmd != inObj.cmds.end(); ++cmd)
	{
		vector<XObjCmd>		newCmds;
		DecomposeObjCmd(*cmd, newCmds, maxValence);
		outObj.cmds.insert(outObj.cmds.end(), newCmds.begin(), newCmds.end());
	}
}
