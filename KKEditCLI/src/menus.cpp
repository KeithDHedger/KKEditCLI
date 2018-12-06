/*
 *
 * ©K. D. Hedger. Tue  5 Jun 15:53:33 BST 2018 keithdhedger@gmail.com

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
const char	*menuNames[]={"_File","_Edit","_Tabs","_Navigation","F_unctions","_Bookmarks","T_ools",NULL};
const char	*fileMenuNames[]={" _New     Ctrl+N"," _Open    Ctrl+O"," _Save    Ctrl+S"," Save _As Ctrl+A"," _Close   Ctrl+W"," _Quit    Ctrl+Q",NULL};
const char	fileMenuShortcuts[FILECNT]={'n','o','s','a','w','q'};
const char	*editMenuNames[]={" _Copy Word Ctrl+C"," C_ut Word  Ctrl+X"," Copy _Line Ctrl+L"," Cut L_ine  Ctrl+Z"," _Paste     Ctrl+V",NULL};
const char	editMenuShortcuts[EDITCNT]={'c','x','l','z','v'};
const char	*navMenuNames[]={" Goto _Define   Ctrl+D"," _Open Include  Ctrl+U"," _Goto Line     Ctrl+J"," Open _Manpage  Ctrl+P"," _Find          Ctrl+F"," Find _Next     Ctrl+G",NULL};
const char	navMenuShortcuts[NAVCNT]={'d','u','j','p','f','g'};

//dynamic menus
char		**tabsMenuNames=NULL;
funcStruct	**functionData=NULL;
char		**functionsMenuNames=NULL;
char		*bookmarksMenuNames[MAXBOOKMARKS]={strdup(" _Remove All Marks"),strdup(" _Toggle BM"),NULL};
const char	*toolsMenuNames[]={"TODO ..."," _Manage Tools",NULL};

unsigned	menuWidth=0;
int			menuStart=0;

void drawMenuStyle(const char **menulist,int menunum,int x,int y,int style,bool doshortcut,bool dopad)
{
	bool	gotus;
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

	gotus=false;
	for(unsigned j=0;j<menuWidth;j++)
		{
			if(j<strlen(menulist[menunum]))
				{
					if((menulist[menunum][j]=='_') && (doshortcut==true))
						{
							j++;
							printf("%s%c%s",UNDERSCOREON,menulist[menunum][j],UNDERSCOREOFF);
							gotus=true;
						}
					else
						printf("%c",menulist[menunum][j]);
				}
			else
				printf(" ");
		}
	if((gotus==true) && (dopad==true))
		printf(" ");
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
			drawMenuStyle(menuNames,menucnt,x,y,FLATNORM,true,false);
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
			if(strlen(menulist[maxitems])>=menuWidth)
				menuWidth=strlen(menulist[maxitems])+1;
			maxitems++;
		}

	if(doshortcut==true)
		menuWidth--;

	cnt=0;
	while((menulist[cnt+menuStart]!=NULL) && (y<menuHite+mBarHite))
		{
			if(prelight==cnt)
				drawMenuStyle(menulist,cnt+menuStart,sx,y++,FLATINVERT,doshortcut,true);
			else
				drawMenuStyle(menulist,cnt+menuStart,sx,y++,FLATNORM,doshortcut,true);
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
//			read(STDIN_FILENO,&buf,15);
//			DEBUGFUNC("0=%x 1=%x 2=%x 3=%x",buf[0],buf[1],buf[2],buf[3]);
			ret=termkey_waitkey(tk,&key);
			termkey_strfkey(tk, buffer, 50, &key, format);
			fprintf(stderr,">>>>Key from lib doMenuEvent %s mods=%i\n", buffer,key.modifiers);

			switch(key.type)
				{
					case TERMKEY_TYPE_KEYSYM:
						{
							switch(key.code.sym)
								{
									case TERMKEY_SYM_ESCAPE:
										loop=false;
										selection=CONT;
										continue;
										break;
									case TERMKEY_SYM_UP:
										fprintf(stderr,"Up key\n");
										selection--;
										if(selection<1)
											{
												selection=1;
												if(menuStart>0)
													menuStart--;
											}
										drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
										break;
									case TERMKEY_SYM_DOWN:
										fprintf(stderr,"----Down key\n");
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
									case TERMKEY_SYM_LEFT:
										fprintf(stderr,"Left key\n");
										menuStart=0;
										selection=MENULEFT;
										loop=false;
										continue;
										break;
									case TERMKEY_SYM_RIGHT:
										fprintf(stderr,"Right key\n");
										menuStart=0;
										selection=MENURITE;									
										loop=false;
										continue;
										break;
									case TERMKEY_SYM_HOME:
										selection=1;
										menuStart=0;
										drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
										break;
									case TERMKEY_SYM_END:
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
									case TERMKEY_SYM_PAGEUP:
										menuStart-=menuHite;
										if(menuStart<0)
											menuStart=0;
										drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
										break;
									case TERMKEY_SYM_PAGEDOWN:
										if(maxitems<menuHite)
											break;								
										menuStart+=menuHite-1;
										if((menuStart+menuHite)>maxitems)
											menuStart=maxitems-menuHite+1;
										drawMenuWindow(menunames,sx,2,selection-1,doshortcut);
										break;
									case TERMKEY_SYM_ENTER:
										SETNORMAL;
										SHOWCURS;
										printLines();
										moveCursToTemp(currentX,currentY);
										return(selection+menuStart);
										break;
									default:
										break;
								}
						}
					case TERMKEY_TYPE_UNICODE:
						tstr[1]=toupper(key.code.codepoint);
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
						break;
					default:
						break;
				}
				
		}
	SETNORMAL;
	clearScreen();
	printLines();
	moveCursToTemp(currentX,currentY);
	SHOWCURS;


return(selection);
#if 0
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
							fprintf(stderr,"[[[[[[[[[[[\n");
							switch(buf[2])
								{
//keys
								case PAGEDOWN:
									if(maxitems<menuHite)
										break;								
									menuStart+=menuHite-1;
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
								fprintf(stderr,">>>>>>>>>>>>>>>>>>>\n");
									menuStart=0;
									selection=MENULEFT;
									loop=false;
									continue;
									break;
								case KEYRITE:
								fprintf(stderr,">>>>>>>>>>>>>>>>>>>\n");
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
#endif
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

	cnt=0;
	tabsMenuNames=(char**)calloc(maxPages+1,sizeof(char*));
	for(int j=0;j<maxPages;j++)
		{
			if(pages[j]!=NULL)
				asprintf(&tabsMenuNames[cnt++]," %s%c%i",pages[j]->filePath,0,j);
		}
	tabsMenuNames[maxPages]=NULL;
}

void buildBMMenu(void)
{
	for(int j=2;j<MAXBOOKMARKS;j++)
		freeAndNull(&bookmarksMenuNames[j]);

	int cnt=2;
	for(int j=0;j<MAXBOOKMARKS;j++)
		{
			if(bookmarks[j].line!=-1)
				asprintf(&bookmarksMenuNames[cnt++]," Line %i, Page %i ",bookmarks[j].line,bookmarks[j].pageNum);
		}
}
