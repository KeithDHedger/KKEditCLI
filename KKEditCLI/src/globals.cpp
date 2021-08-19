/*
 *
 * ©K. D. Hedger. Mon  6 May 17:24:42 BST 2019 keithdhedger@gmail.com

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

CTK_mainAppClass		*mainApp=new CTK_mainAppClass;
int						windowRows=mainApp->maxRows-TOPLINE;
int						windowCols=mainApp->maxCols;
int						showLineNumbers=4;
const char				*tmpEdDir;
char					*manFile=NULL;
char					*configFolder=NULL;
CTK_cursesLabelClass	*fileInfoLabel=NULL;

int						newCnt=0;
std::string				clip="";
int						sinkReturn;
char					*sinkReturnStr;

//tools
int						intermarg=0;
int						flagsarg=0;
int						inpopup=0;
int						alwayspopup=0;
int						clearview=0;
char					*commandarg=NULL;
char					*commentarg=NULL;
char					*menuname=NULL;
int						rootarg=0;
int						keycode=0;
int						usebar=0;
std::vector<toolStruct>	tools;

//functions
std::vector<funcStruct*>	functions;
//bms
std::vector<bookmarkStruct>	bms;

//utils
char* oneLiner(bool noreturn,const char* fmt,...)
{
	FILE	*fp;
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(MAXBUFFERKKEDIT);
	subbuffer=(char*)alloca(MAXBUFFERKKEDIT);

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
			fgets(buffer,MAXBUFFERKKEDIT,fp);
			if(strlen(buffer)>0)
				{
					if(buffer[strlen(buffer)-1] =='\n')
						buffer[strlen(buffer)-1]=0;
				}
			pclose(fp);
			if(noreturn==true)
				return(NULL);
			else
				return(strdup(buffer));
		}
	return(NULL);
}

//find src box
CTK_cursesSourceEditBoxClass* getSrcBox(int page)
{
	CTK_cursesGadgetClass	*srcbox=NULL;

	srcbox=mainApp->CTK_getGadgetNum(page,SRCGADGET,1);
	return(static_cast<CTK_cursesSourceEditBoxClass*>(srcbox));
}

//find edit box
CTK_cursesEditBoxClass* getEditBox(int page)
{
	CTK_cursesGadgetClass	*edbox;

	edbox=mainApp->CTK_getGadgetNum(page,EDITGADGET,1);
	return(static_cast<CTK_cursesEditBoxClass*>(edbox));
}
