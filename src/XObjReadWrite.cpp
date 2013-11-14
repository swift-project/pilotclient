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
#include "XObjReadWrite.h"
#include "XObjDefs.h"
#include "XUtils.h"
#include <stdio.h>
#include <stdlib.h>

#if APL
	#define CRLF	"\r"
#elif WIN
	#define CRLF	"\r\n"
#else
	#define CRLF	"\n"
#endif

/*
	TODO: err checking diagnostics
	TODO: fix scientific notation support.
*/

bool	XObjRead(const char * inFile, XObj& outObj)
{
	vector<string>	tokens;
	string			ascii, vers, tag, line;
	int				cmd_id, count, obj2_op;
	int				version = 1;
	vec_tex			vst;
	vec_rgb			vrgb;
	
	float scanned_st_rgb[4][3]={0,0,0 , 0,0,0,// [corner][color or st]
                               0,0,0 , 0,0,0};	
	
	outObj.cmds.clear();

	/*********************************************************************
	 * READ HEADER
	 *********************************************************************/
	 
	StTextFileScanner	f(inFile, true);
	if (f.done()) return false;

	// First we should have some kind of token telling us whether we're Mac or PC
	// line endings.  But we don't care that much.
	line = f.get();
	BreakString(line, tokens);
	if (tokens.empty()) return false;
	ascii = tokens[0];
	f.next();
	if (f.done()) return false;
	
	// Read the version string.  We expect either '2' or '700'.
	line = f.get();
	
	BreakString(line, tokens);
	if (tokens.empty()) return false;
	vers = tokens[0];
	f.next();
	if (f.done()) return false;	

		 if (vers == "700") version = 7;
	else if (vers == "2"  ) version = 2;
	else					version = 1;
	
	// If we're OBJ7, another token 'OBJ' follows...this is because
	// all XP7 files start with the previous two lines.
	if (version == 7)
	{
		line = f.get();
		BreakString(line, tokens);
		if (tokens.empty()) return false;
		tag = tokens[0];
		f.next();
		if (f.done()) return false;
	}
	
	// The last line of the header is the texture file name.	
	if (version != 1)
	{
		line = f.get();	
		BreakString(line, tokens);
		if (tokens.empty()) return false;
		outObj.texture = tokens[0];
		f.next();
		if (f.done()) return false;
	}
		
	/************************************************************
	 * READ GEOMETRIC COMMANDS
	 ************************************************************/
	bool	first_loop = true;
	while (!f.done())
	{
		XObjCmd	cmd;
		
		// Special case, don't pull from the file for v-1...that
		// version string is really an obj command.
		if (version != 1 || !first_loop)
		{
			line = f.get();
			f.next();
		}		
		first_loop = false;

		BreakString(line, tokens);
		if (tokens.empty()) continue;

		/************************************************************
		 * OBJ2 SCANNING
		 ************************************************************/		
		if (version != 7)
		{
			obj2_op = atoi(tokens[0].c_str());
			switch(obj2_op) {
			case 1:
			case 2:
				// Points and lines.  The header line contains the color x10.
				// The type (pt or line) tell how much geometry follows.
				cmd.cmdID = (obj2_op == 1) ? obj_Light : obj_Line;
				cmd.cmdType = type_PtLine;
				count = obj2_op;
				if (tokens.size() < 4) return false;			
				scanned_st_rgb[0][0]=scanned_st_rgb[1][0]=atof(tokens[1].c_str())*0.1; // r
				scanned_st_rgb[0][1]=scanned_st_rgb[1][1]=atof(tokens[2].c_str())*0.1; // g
				scanned_st_rgb[0][2]=scanned_st_rgb[1][2]=atof(tokens[3].c_str())*0.1; // b

				// Sets of x,y,z follows.
				for (int t = 0; t < count; ++t)
				{
					line = f.get();
					f.next();
					BreakString(line, tokens);
					if (tokens.size() < 3) return false;
					vrgb.v[0] = atof(tokens[0].c_str());
					vrgb.v[1] = atof(tokens[1].c_str());
					vrgb.v[2] = atof(tokens[2].c_str());
					vrgb.rgb[0] = scanned_st_rgb[t][0];
					vrgb.rgb[1] = scanned_st_rgb[t][1];
					vrgb.rgb[2] = scanned_st_rgb[t][2];
					cmd.rgb.push_back(vrgb);
				}
				outObj.cmds.push_back(cmd);					
				break;

			case 3:
			case 4:
			case 5:
				// Finite-size polygons.  The header line contains s1, s2, t1, t2.
				cmd.cmdID = (obj2_op == 5) ? obj_Quad_Hard : obj_Quad;
				if (obj2_op == 3) cmd.cmdID = obj_Tri;
				cmd.cmdType = type_Poly;
				count = obj2_op;
				if (count == 5) count = 4;				
				if (tokens.size() < 5 && version == 2) return false;
				// Make sure to 'spread' the 4 S/T coords to 8 points.  This is 
				// because 
				if (version == 2)
				{
					scanned_st_rgb[2][0]=scanned_st_rgb[3][0]=atof(tokens[1].c_str());	// s1
					scanned_st_rgb[0][0]=scanned_st_rgb[1][0]=atof(tokens[2].c_str());	// s2
					scanned_st_rgb[1][1]=scanned_st_rgb[2][1]=atof(tokens[3].c_str());	// t1
					scanned_st_rgb[0][1]=scanned_st_rgb[3][1]=atof(tokens[4].c_str());  // t2
				} else {
					scanned_st_rgb[2][0]=scanned_st_rgb[3][0]=0.0;
					scanned_st_rgb[0][0]=scanned_st_rgb[1][0]=0.0;
					scanned_st_rgb[1][1]=scanned_st_rgb[2][1]=0.0;
					scanned_st_rgb[0][1]=scanned_st_rgb[3][1]=0.0;
				}
				// Read sets of 3 points.
				for (int t = 0; t < count; ++t)
				{
					line = f.get();
					f.next();
					BreakString(line, tokens);
					if (tokens.size() < 3) return false;
					
					vst.v[0] =  atof(tokens[0].c_str());
					vst.v[1] =  atof(tokens[1].c_str());
					vst.v[2] =  atof(tokens[2].c_str());
					vst.st[0] = scanned_st_rgb[t][0];
					vst.st[1] = scanned_st_rgb[t][1];
					cmd.st.push_back(vst);
				}
				outObj.cmds.push_back(cmd);
				break;				
				
			case 99:
				// 99 is the end token for obj2 files.
				return true;
			default:
				// Negative numbers equal positive
				// quad strips.  The count is the number
				// of vertex pairs, since they are always even.
				if (obj2_op >= 0)
					return false;
				count = -obj2_op;
				cmd.cmdID = obj_Quad_Strip;
				cmd.cmdType = type_Poly;
				
				// Read a pair of x,y,z,s,t coords.
				while (count--)
				{
					line = f.get();
					f.next();
					BreakString(line, tokens);
					if (tokens.size() < 10) return false;
					vst.v[0] = atof(tokens[0].c_str());
					vst.v[1] = atof(tokens[1].c_str());
					vst.v[2] = atof(tokens[2].c_str());
					vst.st[0] = atof(tokens[6].c_str());
					vst.st[1] = atof(tokens[8].c_str());
					cmd.st.push_back(vst);
					vst.v[0] = atof(tokens[3].c_str());
					vst.v[1] = atof(tokens[4].c_str());
					vst.v[2] = atof(tokens[5].c_str());
					vst.st[0] = atof(tokens[7].c_str());
					vst.st[1] = atof(tokens[9].c_str());
					cmd.st.push_back(vst);
				}
				outObj.cmds.push_back(cmd);					
				break;
			}
			
		} else {

			/************************************************************
			 * OBJ7 SCANNING
			 ************************************************************/		
		
			cmd_id = FindObjCmd(tokens[0].c_str());
			
			cmd.cmdType = gCmds[cmd_id].cmd_type;
			cmd.cmdID = gCmds[cmd_id].cmd_id;
			count = gCmds[cmd_id].elem_count;
			
			switch(gCmds[cmd_id].cmd_type) {
			case type_None:
				if (cmd_id == obj_End)
					return true;
				else
					return false;
			case type_PtLine:
			
				if ((count == 0) && (tokens.size() > 1))
					count = atoi(tokens[1].c_str());
				while (count-- && !f.done())
				{
					line = f.get();
					f.next();
					BreakString(line, tokens);
					if (tokens.size() > 5)
					{
						vrgb.v[0] = atof(tokens[0].c_str());
						vrgb.v[1] = atof(tokens[1].c_str());
						vrgb.v[2] = atof(tokens[2].c_str());
						vrgb.rgb[0] = atof(tokens[3].c_str());
						vrgb.rgb[1] = atof(tokens[4].c_str());
						vrgb.rgb[2] = atof(tokens[5].c_str());
						
						cmd.rgb.push_back(vrgb);
					} else
						return false;
				}
				outObj.cmds.push_back(cmd);
				break;
				
			case type_Poly:

				if ((count == 0) && (tokens.size() > 1))
					count = atoi(tokens[1].c_str());
				while (count-- && !f.done())
				{
					line = f.get();
					f.next();
					BreakString(line, tokens);
					if (tokens.size() > 4)
					{
						vst.v[0] = atof(tokens[0].c_str());
						vst.v[1] = atof(tokens[1].c_str());
						vst.v[2] = atof(tokens[2].c_str());
						vst.st[0] = atof(tokens[3].c_str());
						vst.st[1] = atof(tokens[4].c_str());
						
						cmd.st.push_back(vst);
						
						if (tokens.size() > 9)
						{
							--count;
							vst.v[0] = atof(tokens[5].c_str());
							vst.v[1] = atof(tokens[6].c_str());
							vst.v[2] = atof(tokens[7].c_str());
							vst.st[0] = atof(tokens[8].c_str());
							vst.st[1] = atof(tokens[9].c_str());
							
							cmd.st.push_back(vst);
						}
					
					} else
						return false;
				}
				outObj.cmds.push_back(cmd);
				break;
			case type_Attr:
				
				if (tokens.size() > count)
				{
					for (int n = 0; n < count; ++n)
						cmd.attributes.push_back(atof(tokens[n+1].c_str()));
				} else
					return false;
					
				outObj.cmds.push_back(cmd);
				break;
			}

		}	// Obj 7 case

	} // While loop
	return true;
}

#if !defined(APL)
#define APL 0
#endif

bool	XObjWrite(const char * inFile, const XObj& inObj)
{
	FILE * fi = fopen(inFile, "w");
	if (!fi) return false;

	fprintf(fi,"%c" CRLF, APL ? 'A' : 'I');
	fprintf(fi,"700" CRLF);
	fprintf(fi,"OBJ" CRLF CRLF);
	fprintf(fi,"%s\t\t//" CRLF CRLF, inObj.texture.c_str());
	
	for (vector<XObjCmd>::const_iterator iter = inObj.cmds.begin(); iter != inObj.cmds.end(); ++iter)
	{
		int 	index	= FindIndexForCmd(iter->cmdID);
		switch(iter->cmdType) {
		case type_PtLine:
		
			if (gCmds[index].elem_count == 0)
				fprintf(fi,"%s %zd\t\t//" CRLF, gCmds[index].name, iter->rgb.size());
			else
				fprintf(fi,"%s\t\t//" CRLF, gCmds[index].name);
			
			for (vector<vec_rgb>::const_iterator riter = iter->rgb.begin();
				riter != iter->rgb.end(); ++riter)
			{
				fprintf(fi,"%f %f %f    %f %f %f" CRLF,
					riter->v[0], riter->v[1], riter->v[2],
					riter->rgb[0], riter->rgb[1], riter->rgb[2]);
			}
			fprintf(fi,CRLF);
			break;
			
			
		case type_Poly:
		
			if (gCmds[index].elem_count == 0)
				fprintf(fi,"%s %zd\t\t//" CRLF, gCmds[index].name, iter->st.size());
			else
				fprintf(fi,"%s\t\t//" CRLF, gCmds[index].name);
			
			for (vector<vec_tex>::const_iterator siter = iter->st.begin();
				siter != iter->st.end(); ++siter)
			{
				fprintf(fi,"%f %f %f    %f %f" CRLF,
					siter->v[0], siter->v[1], siter->v[2],
					siter->st[0], siter->st[1]);
			}
			fprintf(fi,CRLF);
			break;

		
		case type_Attr:
			fprintf(fi,"%s",gCmds[index].name);
			for (vector<float>::const_iterator aiter = iter->attributes.begin();
				aiter != iter->attributes.end(); ++aiter)
			{
				fprintf(fi," %f", *aiter);
			}
			fprintf(fi, "\t\t//" CRLF CRLF);
			break;
		}	
	}

	fprintf(fi,"end\t\t//" CRLF);
	
	fclose(fi);
	return true;
}
