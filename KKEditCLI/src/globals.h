/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:10:48 BST 2018 keithdhedger@gmail.com

 * This file (globals.h) is part of KKEditCLI.

 * KKEditCLI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEditCLI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEditCLI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GLOBALS_
#define _GLOBALS_

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#include <fstream>
#include <sstream>

#include <ftw.h>

#include <srchilite/sourcehighlight.h>
#include <srchilite/langmap.h>
#include <srchilite/lineranges.h>
#include <srchilite/languageinfer.h>

#include <dialog.h>
#include <termkey.h>
#include <cdk.h>

#include "config.h"

using namespace std;
using namespace srchilite;

#define MAXBUFFER 2048
#define MAXLINES 50000

#ifdef _ENABLEDEBUG_
static bool	showFileData=true;
static void debugFunc(const char *file,const char *func,int line,const char *fmt, ...)
{
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(512);
	subbuffer=(char*)alloca(512);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'c':
								sprintf(subbuffer,"%c",va_arg(ap,int));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							case 'x':
								sprintf(subbuffer,"%x",va_arg(ap,int));
								break;
							case 'p':
								sprintf(subbuffer,"%p",va_arg(ap,char*));
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);

	if(showFileData==true)
		{
			fprintf(stderr,"\nFile: %s\nFunc: %s\nLine: %i\n",basename(file),func,line);
			fprintf(stderr,"----USER DATA----\n%s\n----END----\n",buffer);
		}
	else
		{
			fprintf(stderr,"%s\n",buffer);
		}
}
#define DEBUGFUNC(x,...) debugFunc(__FILE__,__func__,__LINE__,(const char*)x,__VA_ARGS__)
#else
#ifdef _WARN_ENABLEDEBUG_
#define DEBUGFUNC(...) fprintf(stderr,"Remove debug code here: %s:%i\n",__FILE__,__LINE__);
#else
#define DEBUGFUNC(...) 
#endif
#endif

static inline void freeAndNull(char** ptr)
{
	if(*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;
}

#include "curseslib.h"
#include "editor.h"
#include "files.h"
#include "menus.h"
#include "gui.h"

//termkey
extern char buffer[];
extern TermKey *tk;
extern TermKeyResult ret;
extern TermKeyKey key;
extern TermKeyFormat format;

//term info
extern int			cols;
extern int			rows;
extern int			minY;
extern int			minX;
extern int			currentX;
extern int			currentY;
extern unsigned		maxRows;
extern int			maxCols;
extern int			forceCols;

//file
extern int			newFileNum;

//gui
extern int			tabs;
extern int			startY;
extern int			endY;
extern int			lineColour;
extern int			mBarHite;
extern int			menuHite;

//prefs
extern int			tabs;
extern bool			liveUpdate;
extern bool			hilite;

//files
extern const char	*tmpEdDir;
extern char			*tmpEdFile;
extern char			*tmpEdFilePath;

//pages
extern int			maxPages;
extern pageStruct	**pages;
extern pageStruct	*page;

//utils
char* oneLiner(bool noreturn,const char* fmt,...);

#endif
