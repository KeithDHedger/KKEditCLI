/*
 *
 * ©K. D. Hedger. Tue  5 Jun 15:53:33 BST 2018 kdhedger68713@gmail.com

 * This file (menus.cpp) is part of KKEditCLI.

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

const char	*menuNames[]={"_File","_Edit","_View","_Navigation","F_unctions","_Bookmarks","_Tools",NULL};
const char	*fileMenuNames[]={"_New     ","_Open    ","_Save    ","Save _As ","_Quit    ",NULL};
const char	*editMenuNames[]={"_Cut   ","Cop_y  ","_Paste ",NULL};
const char	*viewMenuNames[]={"Show docs","Show docs","Show docs",NULL};
const char	*navMenuNames[]={"Goto _Define    ","Open _Include   ","Goto _Line      ","Search _GTK Docs",NULL};
//const char	*functionsMenuNames[]={"drawMenuStyle","drawMenuBar","printStatusBar","eventLoop","initCursesLib","oneLiner",NULL};
funcStruct	**functionData;
char		**functionsMenuNames;

const char	*bookmarksMenuNames[]={"_Remove All Marks","_Toggle BM       ",NULL};
const char	*toolsMenuNames[]={"_Manage Tools",NULL};

void drawMenuStyle(const char **menulist,int menunum,int x,int y,int style)
{
	moveCursToTemp(x,y);
	switch(style)
		{
			case FLATNORM:
				SETINVERSEON
				SETFORECOL(GREEN);
				SETBACKCOL(BLACK);
				break;
			case FLATINVERT:
				SETINVERSEOFF
				SETFORECOL(GREEN);
				SETBACKCOL(BLACK);
				break;
		}

	for(unsigned j=0;j<strlen(menulist[menunum]);j++)
		{
			if(menulist[menunum][j]=='_')
				{
					j++;
					printf("%s%c%s",UNDERSCOREON,menulist[menunum][j],UNDERSCOREOFF);
				}
			else
				printf("%c",menulist[menunum][j]);
		}
	fflush(NULL);
}

void drawMenuBar(void)
{
	int	menucnt=0;
	int	x=1;
	int y=1;

	while(menuNames[menucnt]!=NULL)
		{
			drawMenuStyle(menuNames,menucnt,x,y,FLATNORM);
			x+=strlen(menuNames[menucnt++]);
		}
	SETNORMAL;
}

int drawMenuWindow(const char **menulist,int sx,int sy,int prelight)
{
	int	cnt=0;
	int	y=sy;

	while(menulist[cnt]!=NULL)
		{
			if(prelight==cnt)
				drawMenuStyle(menulist,cnt,sx,y++,FLATINVERT);
			else
				drawMenuStyle(menulist,cnt,sx,y++,FLATNORM);
			cnt++;
		}
	SETNORMAL;
	return(cnt);
}

int doMenuEvent(const char **menunames,int sx,int sy)
{
	bool	loop=true;
	int		selection=0;
	char	buf[16]={0,};
	int		maxitems;
	int		cnt=0;
	char	tstr[3]={'_',0,0};

	maxitems=drawMenuWindow(menunames,sx,sy,-1);
	HIDECURS;
	fflush(NULL);
	while(loop==true)
		{
			memset(buf,0,16);
			read(STDIN_FILENO,&buf,15);
//			DEBUGFUNC("0=%x 1=%x 2=%x 3=%x",buf[0],buf[1],buf[2],buf[3]);
			if(buf[0]==ESCCHAR)
				{
					switch(buf[1])
						{
//cursor home and end
						case 'O':
							switch(buf[2])
								{
								case CURSHOME:
									selection=1;
									drawMenuWindow(menunames,sx,2,selection-1);
									break;
								case CURSEND:
									selection=maxitems;
									drawMenuWindow(menunames,sx,2,selection-1);
									break;
								}
							break;
//cursor keys
						case '[':
							switch(buf[2])
								{
//keys
								case PAGEDOWN:
									break;
								case PAGEUP:
									break;
								case KEYUP:
									selection--;
									if(selection<1)
										selection=1;
									drawMenuWindow(menunames,sx,2,selection-1);
									break;
								case KEYDOWN:
									selection++;
									if(selection>maxitems)
										selection=maxitems;
									drawMenuWindow(menunames,sx,2,selection-1);
									continue;
									break;
								case KEYLEFT:
									selection=MENULEFT;
									loop=false;
									continue;
									break;
								case KEYRITE:
									selection=MENURITE;									
									loop=false;
									continue;
									break;
								}
							break;
						}
//end esc char
				}
			
			if((buf[0]==ESCCHAR) && (buf[1]==0))
				{
					loop=false;
					selection=CONT;
					continue;
				}

			if(buf[0]=='\n')
				{
					SETNORMAL;
					SHOWCURS;
					printLines();
					moveCursToTemp(currentX,currentY);
					return(selection);
				}

			tstr[1]=buf[0];
			cnt=0;
			while(menunames[cnt]!=NULL)
				{
					if(strcasestr((char*)menunames[cnt],(char*)&tstr)!=NULL)
						{
							SETNORMAL;
							SHOWCURS;
							printLines();
							moveCursToTemp(currentX,currentY);
							return(cnt+1);
						}
					cnt++;
				}
		}
	SETNORMAL;
	clearScreen();
	printLines();
	moveCursToTemp(currentX,currentY);
	SHOWCURS;
	return(selection);
}

