/*
 *
 * ©K. D. Hedger. Sun 20 May 14:12:24 BST 2018 kdhedger68713@gmail.com

 * This file (gui.cpp) is part of KKEditCLI.

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

#include <string.h>
#include "globals.h"

void printLines(void)
{
	int	screenline=minY;

	HIDECURS;
	drawMenuBar();
	for(int j=page->topLine; j<page->topLine+maxRows; j++)
		{
			if(page->line[j].srcLine==NULL)
				break;
			if(page->line[j].lineNum>0)
				{
					moveCursToTemp(1,screenline);
					printf("\e[38;5;%im%.4i\e[m",lineColour,page->line[j].lineNum);
				}
			moveCursToTemp(minX,screenline++);
			printf("%s%s",CLEARTOEOL,page->line[j].srcLine);
		}

	printf(CLEARTOEOS);
	SHOWCURS;
}

int handleFileMenu(void)
{
	int		menuselect;
	char	*message=NULL;

	menuselect=doMenuEvent(fileMenuNames,1,2,true);
	switch(menuselect)
		{
			case FILENEW:
				makeNewFile();
				clearScreen();				 
				refreshScreen();
				moveCursToTemp(minX,currentY);			
				break;
			case FILEOPEN:
				askSaveIfdirty();
				askOpenFile();
				break;
			case FILESAVE:
				saveFile(page->filePath);
				clearScreen();				 
				printLines();
				adjCursor();
				break;
			case FILESAVEAS:
				break;
			case FILEQUIT:
				askSaveIfdirty();
				return(BRAKE);
				break;
		}
	return(menuselect);
}

int handleEditMenu(void)
{
	int menuselect;

	menuselect=doMenuEvent(editMenuNames,6,2,true);
	switch(menuselect)
		{
			case EDITCUT:
				break;
			case EDITCOPY:
				break;
			case EDITPASTE:
				break;
		}
	return(menuselect);
}

int handleViewMenu(void)
{
	int menuselect;

	menuselect=doMenuEvent(viewMenuNames,11,2,true);
	switch(menuselect)
		{
			case VIEWDOC0:
				break;
			case VIEWDOC1:
				break;
			case VIEWDOC2:
				break;
		}
	return(menuselect);
}

int handleNavMenu(void)
{
	int menuselect;

	menuselect=doMenuEvent(navMenuNames,16,2,true);
	switch(menuselect)
		{
			case NAVEGOTODEF:
				break;
			case NAVEOPENINC:
				break;
			case NAVGOTOLINE:
				break;
			case NAVSEARCHGTK:
				break;
		}
	return(menuselect);
}

void getTagList(void)
{
	char		*command;
	const char	*sortcommand="sort -k 2rb,2rb -k 1b,1b";
	char		*retval=NULL;
	FILE		*fp;
	char		line[2048];
	int			funcs=0;
	int			cnt=0;
	char		*ptr;
	char		*ptr2;

	asprintf(&command,"ctags -x \"%s\"|wc -l",tmpEdFilePath);
	retval=oneLiner(false,command);
	funcs=atoi(retval)+1;
	functionsMenuNames=(const char**)calloc(sizeof(char*),funcs);
	functionData=(funcStruct**)calloc(sizeof(funcStruct*),funcs);

	free(command);	
	asprintf(&command,"ctags -x \"%s\"|%s|sed 's@ \\+@ @g'",tmpEdFilePath,sortcommand);
	fp=popen(command, "r");
	while(fgets(line,2048,fp))
		{
			functionData[cnt]=new funcStruct;
			if(strlen(line)>1)
				line[strlen(line)-1]=0;
			ptr=strchr((char*)&line,' ');
			*ptr=0;
			asprintf(&functionData[cnt]->name," %s",line);
			ptr++;
			ptr2=strchr(ptr,' ');
			*ptr2=0;
			functionData[cnt]->type=strdup(ptr);
			ptr2++;
			ptr=strchr(ptr2,' ');
			*ptr=0;
			functionData[cnt]->line=atoi(ptr2)-1;
			functionsMenuNames[cnt]=functionData[cnt]->name;
				cnt++;
		}
	functionsMenuNames[cnt]=NULL;
	functionData[cnt]=NULL;
	pclose(fp);
}

int handleFuncMenu(void)
{
	int menuselect=0;
	if(functionsMenuNames==NULL)
		getTagList();

	menuselect=doMenuEvent((const char**)functionsMenuNames,27,2,false);
	if(menuselect>CONT)
		{
			menuselect--;
			page->currentLine=functionData[menuselect]->line;
			page->topLine=functionData[menuselect]->line;
			clearScreen();				 
			printLines();
			adjCursor();	
			return(CONT);
		}
	return(menuselect);
}

int handleBMMenu(void)
{
	int menuselect;

	menuselect=doMenuEvent(bookmarksMenuNames,37,2,true);
	switch(menuselect)
		{
			case BMREMOVEALL:
				break;
			case BMTOGGL:
				break;
		}
	return(menuselect);
}

int handleToolMenu(void)
{
	int menuselect;

	menuselect=doMenuEvent(toolsMenuNames,47,2,true);
	switch(menuselect)
		{
			case TOOLMANAGE:
				break;
		}
	return(menuselect);
}

int handleAllMenus(void)
{
	int	mnum=0;
	int	retval=-1000;

	while(retval<BRAKE)
		{
			if(retval==MENURITE)
				mnum++;
			if(mnum==MAXMENUS)
				mnum--;
			if(retval==MENULEFT)
				mnum--;
			if(mnum==-1)
				mnum++;
			
			switch(mnum)
				{
					case FILEMENU:
						retval=handleFileMenu();
						break;
					case EDITMENU:
						retval=handleEditMenu();
						break;
					case VIEWMENU:
						retval=handleViewMenu();
						break;
					case NAVIGATIONMENU:
						retval=handleNavMenu();
						break;
					case FUNCTIONSMENU:
						retval=handleFuncMenu();
						break;
					case BOOKMARKSMENU:
						retval=handleBMMenu();
						break;
					case TOOLSMENU:
						retval=handleToolMenu();
						break;
				}
		}
	return(retval);
}

void refreshScreen(void)
{
	for(int j=0; j<page->maxLines;j++)
		{
			freeAndNull(&page->line[j].srcLine);
			freeAndNull(&page->line[j].edLine);
		}
	page->maxLines=0;
	openTheFile(tmpEdFilePath);
	printLines();
}

void eventLoop(void)
{
	bool			handled=false;
	bool			dorefresh=false;
	bool			needsrefresh=false;
	unsigned char	buf[16];
		//	DEBUGFUNC(">>>>0=%x 0=%c 1=%x 1=%c 2=%x 2=%c 3=%x 3=%c",buf[0],buf[0],buf[1],buf[1],buf[2],buf[2],buf[3],buf[3]);
	while(true)
		{
			memset(buf,0,16);
			fflush(NULL);
			read(STDIN_FILENO,&buf,15);
			handled=false;

			if(buf[0]==ESCCHAR)
				{
					switch(buf[1])
						{
//cursor home and end
						case 'O':
							switch(buf[2])
								{
								case CURSHOME:
									page->lineXCurs=0;
									adjCursor();
									handled=true;
									break;
								case CURSEND:
									page->lineXCurs=page->line[page->currentLine].lineLen;
									adjCursor();
									handled=true;
									break;
								}
							break;
//cursor keys
						case '[':
							switch(buf[2])
								{
//console keys
									case CURSHOME:
									case CURSHOMECONS:
									page->lineXCurs=0;
									adjCursor();
									handled=true;
									break;
								case CURSEND:
								case CURSENDCONS:
									page->lineXCurs=page->line[page->currentLine].lineLen;
									adjCursor();
									handled=true;
									break;
//keys
								case PAGEDOWN:
									if(page->topLine+maxRows>page->maxLines)
										{
											currentX=minX;
											currentY=minY;
											page->currentLine=page->topLine;
											page->lineXCurs=0;
										}
									else
										{
											page->topLine+=maxRows;
											page->currentLine+=maxRows;
										}
									printLines();
									if(page->currentLine>page->maxLines-1)
										{
											page->currentLine=page->maxLines-1;
											currentY=minY+(page->maxLines-page->topLine-1);
										}
									adjCursor();
									handled=true;
									break;
								case PAGEUP:
									if(page->currentLine-maxRows<0)
										{
											page->topLine=0;
											page->currentLine=0;
											currentX=minX;
											currentY=minY;
											page->lineXCurs=0;
										}
									else
										{
											page->currentLine-=maxRows;
											page->topLine-=maxRows;
											if(page->topLine<0)
												{
													page->topLine=0;
													page->currentLine=maxRows-1;
												}
										}
									printLines();
									adjCursor();
									handled=true;
									break;
//TODO//
								case KEYUP:
									moveCursUp();
									adjCursor();
									handled=true;
									break;
								case KEYDOWN:
									moveCursDown();
									adjCursor();
									handled=true;
									break;
								case KEYLEFT:
									moveCursLeft();
									handled=true;
									break;
								case KEYRITE:
									moveCursRite();
									handled=true;
									break;
//TODO needs tidying
								case DELETEKEY:
										{
											handled=true;
											if(deleteCharFromFile(false)==false)
												{
													if(liveUpdate==true)
														{
															dorefresh=true;
														}
													else
														{
															dorefresh=false;
															moveCursToTemp(minX,currentY);
															printf("%.*s%s",page->line[page->currentLine].lineLen-(int)(!page->line[page->currentLine].isSplitLine),page->line[page->currentLine].edLine,CLEARTOEOL);
															moveCursToTemp(currentX,currentY);
															needsrefresh=true;
														}
												}
											else
												{
													dorefresh=true;
													needsrefresh=true;
												}
										}
									break;
								}
							break;
						}
//end esc char
				}

			if(handled==false)
				{
					switch(buf[0])
						{
							case RETURNKEY:
								writeCharToFile(RETURNKEY);
								currentX=minX;
								currentY++;
								page->currentLine++;
								page->lineXCurs=0;
								dorefresh=true;
								break;
							case ESCCHAR:
								menuStart=0;
								if(handleAllMenus()==BRAKE)
									return;
								continue;
								break;
							case BACKSPACE:
								if(deleteCharFromFile(true)==true)
									{
										dorefresh=true;
									}
								else
									{
										if(liveUpdate==true)
											{
												dorefresh=true;
											}
										else
											{
												dorefresh=false;
												moveCursToTemp(minX,currentY);
												printf("%.*s%s",page->line[page->currentLine].lineLen-(int)(!page->line[page->currentLine].isSplitLine),page->line[page->currentLine].edLine,CLEARTOEOL);
											}
									}
								needsrefresh=true;
								page->lineXCurs--;
								currentX--;
								adjCursor();
								break;

							case TABKEY:
								writeCharToFile(buf[0]);
								refreshScreen();
								moveCursRite();
								break;
							default:
								writeCharToFile(buf[0]);
								if(currentX==cols-1)
									{
										currentY++;
										currentX=minX;
										page->lineXCurs=0;
										page->currentLine++;
										moveCursToTemp(currentX,currentY);
										refreshScreen();
										moveCursRite();
										continue;
									}
								else
									{
										needsrefresh=true;
										moveCursToTemp(minX,currentY);
										printf("%s",page->line[page->currentLine].edLine);
										currentX++;
										moveCursRite();
									}
								break;
						}
				}

			if((handled==true) && (needsrefresh==true))
				{
					dorefresh=true;
					needsrefresh=false;
				}

			if(dorefresh==true)
				{
					dorefresh=false;
					for(int j=0; j<page->maxLines;j++)
						{
							freeAndNull(&page->line[j].srcLine);
							freeAndNull(&page->line[j].edLine);
						}
					page->maxLines=0;
					openTheFile(tmpEdFilePath);
					printLines();
					adjCursor();
					continue;
				}
			printStatusBar();
			fflush(NULL);
			fflush(STDIN_FILENO);
		}
}

void printStatusBar(void)
{
	printf("\e[m\e[%i;%iHLine=%i Column=%i   ",rows,1,currentY-minY+1,currentX-minX+1);
	printf("\e[%i;%iH",currentY,currentX);
}
