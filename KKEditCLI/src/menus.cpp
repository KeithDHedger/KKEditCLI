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

//menus
//static menus
const char	*menuNames[]={"_File","_Edit","_Tabs","_Navigation","F_unctions","_Bookmarks","_Tools",NULL};
const char	*fileMenuNames[]={" _New"," _Open"," _Save"," Save _As"," _Quit",NULL};
const char	*editMenuNames[]={" _Cut"," Cop_y"," _Paste",NULL};
const char	*navMenuNames[]={" Goto _Define"," Open _Include"," Goto _Line"," Search _GTK Docs",NULL};

//dynamic menus
char		**tabsMenuNames=NULL;
funcStruct	**functionData=NULL;
char		**functionsMenuNames=NULL;
const char	*bookmarksMenuNames[]={" _Remove All Marks"," _Toggle BM",NULL};
const char	*toolsMenuNames[]={" _Manage Tools",NULL};

int			menuWidth=0;
int			menuStart=0;

void drawMenuStyle(const char **menulist,int menunum,int x,int y,int style,bool doshortcut)
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

	for(unsigned j=0;j<menuWidth;j++)
		{
			if(j<strlen(menulist[menunum]))
				{
					if((menulist[menunum][j]=='_') && (doshortcut==true))
						{
							j++;
							printf("%s%c%s",UNDERSCOREON,menulist[menunum][j],UNDERSCOREOFF);
						}
					else
						printf("%c",menulist[menunum][j]);
				}
			else
				printf(" ");
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
			menuWidth=strlen(menuNames[menucnt]);
			drawMenuStyle(menuNames,menucnt,x,y,FLATNORM,true);
			x+=strlen(menuNames[menucnt++]);
		}
	SETNORMAL;
}

int drawMenuWindow(const char **menulist,int sx,int sy,int prelight,bool doshortcut)
{
	int	cnt=0;
	int	y=sy;
	int	maxitems=0;

	menuWidth=0;
	while(menulist[maxitems]!=NULL)
		{
			if(strlen(menulist[maxitems])>menuWidth)
				menuWidth=strlen(menulist[maxitems])+1;
			maxitems++;
		}

	cnt=0;
	while((menulist[cnt+menuStart]!=NULL) && (y<menuHite+mBarHite))
		{
			if(prelight==cnt)
				drawMenuStyle(menulist,cnt+menuStart,sx,y++,FLATINVERT,doshortcut);
			else
				drawMenuStyle(menulist,cnt+menuStart,sx,y++,FLATNORM,doshortcut);
			cnt++;
		}
	SETNORMAL;
	return(maxitems);
}

int doMenuEvent(const char **menunames,int sx,int sy,bool doshortcut)
{
	bool	loop=true;
	int		selection=0;
	char	buf[16]={0,};
	int		maxitems;
	int		cnt=0;
	char	tstr[3]={'_',0,0};

	if(menunames!=NULL)
		maxitems=drawMenuWindow(menunames,sx,sy,-1,doshortcut);
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
									menuStart=0;
									drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
									break;
								case CURSEND:
									if(maxitems>rows-2)
										{
											selection=rows-2;
											menuStart=maxitems-rows+2;
											if((menuStart+rows-2)>maxitems)
												menuStart=maxitems-rows+2;
										}
									else
										selection=maxitems;
									drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
									break;
								}
							break;
//cursor keys
						case '[':
							switch(buf[2])
								{
//keys
								case PAGEDOWN:
									if(maxitems<menuHite)
										break;								
									menuStart+=menuHite;
									if((menuStart+menuHite)>maxitems)
										menuStart=maxitems-menuHite+1;
									drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
									break;
								case PAGEUP:
									menuStart-=menuHite;
									if(menuStart<0)
										menuStart=0;
									drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
									break;
								case KEYUP:
									selection--;
									if(selection<1)
										{
											selection=1;
											if(menuStart>0)
												menuStart--;
										}
									drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
									break;
								case KEYDOWN:
									selection++;
									if((selection>maxitems) || (selection>menuHite))
										selection=menuHite-mBarHite;
									if(selection>maxitems)
										selection=maxitems;

									if(selection+menuStart<=maxitems)
										{
											if(selection>menuHite-mBarHite)
												{
													selection=menuHite-mBarHite;
													menuStart++;
												}
										}
									else
										selection--;
									drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
									break;
								case KEYLEFT:
									menuStart=0;
									selection=MENULEFT;
									loop=false;
									continue;
									break;
								case KEYRITE:
									menuStart=0;
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
					DEBUGFUNC("selection+menuStart=%i",selection+menuStart);
					return(selection+menuStart);
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

void buildTabMenu(void)
{
	int	cnt=0;
	if(tabsMenuNames!=NULL)
		{
			while(tabsMenuNames[cnt]!=NULL)
				free(tabsMenuNames[cnt++]);
			free(tabsMenuNames);
			tabsMenuNames=NULL;
		}

	tabsMenuNames=(char**)calloc(sizeof(char*),maxPages+1);
	for(int j=0;j<maxPages;j++)
		{
			if(pages[j]!=NULL)
				asprintf(&tabsMenuNames[j]," %s ",pages[j]->filePath);
		}
	tabsMenuNames[maxPages]=NULL;
}



