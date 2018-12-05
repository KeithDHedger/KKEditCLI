/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:10:34 BST 2018 keithdhedger@gmail.com

 * This file (globals.cpp) is part of KKEditCLI.

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

#include "globals.h"

//term info
int			cols=0;
int			rows=0;
int			minY=4;
int			minX=9;
int			currentX=minX;
int			currentY=minY;
unsigned	maxRows=0;
int			maxCols=0;
int			forceCols=-1;

//termkey
char buffer[50];
TermKey *tk;
TermKeyResult ret;
TermKeyKey key;
TermKeyFormat format = TERMKEY_FORMAT_VIM;

//file
int			newFileNum=1;

//gui
int			startY=1;
int			endY=1;
int			lineColour=220;
int			mBarHite=1;
int			menuHite=1;

//prefs
int			tabs=8;
bool		liveUpdate=true;
bool		hilite=true;

//files
const char	*tmpEdDir="/dev/shm/";
char		*tmpEdFile=NULL;
char		*tmpEdFilePath=NULL;

//pages
int			maxPages=0;
pageStruct	**pages=NULL;
pageStruct	*page=NULL;

//utils
char* oneLiner(bool noreturn,const char* fmt,...)
{
	FILE	*fp;
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(MAXBUFFER);
	subbuffer=(char*)alloca(MAXBUFFER);

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
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							case '%':
								sprintf(subbuffer,"%%");
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

	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,MAXBUFFER,fp);
			if(strlen(buffer)>0)
				{
					if(buffer[strlen(buffer)-1] =='\n')
						buffer[strlen(buffer)-1]=0;
				}
			pclose(fp);
			if(noreturn==false)
				return(strdup(buffer));
		}
	return(NULL);
}
