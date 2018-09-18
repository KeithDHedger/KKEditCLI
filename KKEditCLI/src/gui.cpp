/*
 *
 * ©K. D. Hedger. Sun 20 May 14:12:24 BST 2018 keithdhedger@gmail.com

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

int	menuNumber=0;

void clearTrough(void)
{
	for(int j=1;j<rows;j++)
		{
			moveCursToTemp(minX,j);
			printf(CLEARTOSOL);
		}
}

void drawFilePath(void)
{
	char	*rp=realpath(page->filePath,NULL);
	moveCursToTemp(1,2);
	printf(CLEARTOEOL);
	moveCursToTemp((cols/2)-(strlen(rp)/2),mBarHite+1);
	printf(BACKRED FOREWHITE "%s" NORMAL,rp);
	free(rp);
}

void printLines(void)
{
	int	screenline=minY;

	HIDECURS;
	clearTrough();
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
	drawFilePath();
	SHOWCURS;
}

int handleFileMenu(void)
{
	int		menuselect;

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
				askOpenFile();
				break;
			case FILESAVE:
				saveFile(page->filePath);
				clearScreen();				 
				printLines();
				adjCursor();
				break;
			case FILESAVEAS:
				askSaveFile();
				break;
			case FILECLOSE:
				closePage();
				for(int j=0;j<maxPages;j++)
					{
						if(pages[j]!=NULL)
							{
								page=pages[j];
								currentPage=page->pageNum;
								setTempEdFile(page->filePath);
								currentX=page->saveX;
								currentY=page->saveY;
								moveCursToTemp(currentX,currentY);
								clearScreen();
								refreshScreen();
								clearTagList();
								adjCursor();
								buildTabMenu();
								return(menuselect);
							}
					}
			case FILEQUIT:
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
				//fprintf(stdin,"
				break;
			case EDITPASTE:
				break;
		}
	return(menuselect);
}

int handleTabsMenu(void)
{
	int menuselect;
	char	*ptr;

	if(tabsMenuNames==NULL)
		return(CONT);

	menuselect=doMenuEvent((const char**)tabsMenuNames,11,2,true);
	if(menuselect>CONT)
		{
			ptr=tabsMenuNames[menuselect-1];
			ptr+=strlen(tabsMenuNames[menuselect-1])+1;
			page->saveX=currentX;
			page->saveY=currentY;
			page->saveCurrentLine=page->currentLine;

			page=pages[atoi(ptr)];
			currentPage=page->pageNum;
			setTempEdFile(page->filePath);
			currentX=page->saveX;
			currentY=page->saveY;
			page->currentLine=page->saveCurrentLine;
			moveCursToTemp(currentX,currentY);
			clearScreen();
			printLines();
			clearTagList();
			adjCursor();
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
				{
					int	cnt=0;
					int	line=0;
					if(functionsMenuNames==NULL)
						getTagList();

					while(functionData[cnt]!=NULL)
						{
							if(strcmp(wordBufPtr,functionData[cnt]->name)==0)
								{
									line=findLineByLineNumber(functionData[cnt]->line)+1;
									page->currentLine=line;
									page->topLine=line;
									page->lineXCurs=0;
									currentY=minY;
									adjCursor();
									break;
								}
							cnt++;
						}
				}
				clearScreen();
				refreshScreen();
				return(CONT);
				break;

			case NAVEOPENINC:
				{
					char		*filename=NULL;
					const char	*includepath=".";

//check for local includes ("")
					filename=oneLiner(false,"echo -n '%s'|sed -n 's/^#include.*\"\\(.*\\)\"$/\\1/p'",page->line[page->currentLine].edLine);
					if(strlen(filename)<2)
						{
//check for local includes (<>)
							filename=oneLiner(false,"echo -n '%s'|sed -n 's/^#include.*<\\(.*\\)>$/\\1/p'",page->line[page->currentLine].edLine);
							if(strlen(filename)>2)
								includepath="/usr/include";
						}

					if(strlen(filename)>2)
						{
							char	*command;
							FILE	*fp;
							char	line[2048];

							asprintf(&command,"echo -n '%s'|xargs -I[] find '%s' -iname '[]' -exec realpath '{}' \\;",filename,includepath);
							fp=popen(command, "r");
							if(fp!=NULL)
								{
									while(fgets(line,2048,fp))
										{
											line[strlen(line)-1]=0;
											initEditor();
											setTempEdFile(line);
											page->filePath=strdup(line);
											oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
											openTheFile(tmpEdFilePath,hilite);
											page->topLine=0;
											page->currentLine=0;
											currentX=minX;
											currentY=minY;
											page->lineXCurs=0;
											printLines();
											adjCursor();
											moveCursToTemp(currentX,currentY);
										}
									pclose(fp);
								}
						}
				}
				break;

				case NAVGOTOLINE:
				{
					int	status=-1;
					init_dialog(stdin,stdout);
						dialog_vars.default_button=-1;
						status=dialog_inputbox("","Goto Line?",0,0,"",false);
					end_dialog();
					if(status==0)
						{
							int realline=findLineByLineNumber(atoi(dialog_vars.input_result));
							//page->currentLine=atoi(dialog_vars.input_result)-1;
							page->currentLine=realline;
							if(page->currentLine>page->maxLines)
								page->currentLine=page->maxLines-1;
							if(page->currentLine<1)
								page->currentLine=0;
							
							page->topLine=page->currentLine;
							page->lineXCurs=0;
							currentY=minY;
							adjCursor();
						}
					clearScreen();
					refreshScreen();
				}
				break;
			case NAVOPENMANPAGE:
				{
					char	*command;

					asprintf(&command,"MANWIDTH=%i MAN_KEEP_FORMATTING=1 man $(man -w %s) > /tmp/$(basename $(man -w %s))",maxCols,wordBufPtr,wordBufPtr);
					system(command);
					free(command);
					command=oneLiner(false,"echo /tmp/$(basename $(man -w %s))",wordBufPtr);					
					page->saveX=currentX;
					page->saveY=currentY;

					initEditor();
					setTempEdFile(command);
					page->filePath=command;
					oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
					openTheFile(tmpEdFilePath,hilite);
					clearScreen();
					refreshScreen();
					moveCursToTemp(currentX,currentY);
				}
				break;
		}
	return(menuselect);
}

int handleFuncMenu(void)
{
	int menuselect=0;
	int	line=0;

	if(functionsMenuNames==NULL)
		getTagList();

	menuselect=doMenuEvent((const char**)functionsMenuNames,27,2,false);
	if(menuselect>CONT)
		{
			menuselect--;
			line=findLineByLineNumber(functionData[menuselect]->line)+1;
			page->currentLine=line;
			page->topLine=line;
			clearScreen();				 
			printLines();
			page->lineXCurs=0;
			currentY=minY;
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
	int	retval=-1000;

	while(retval<BRAKE)
		{
			if(retval==MENURITE)
				menuNumber++;
			if(menuNumber==MAXMENUS)
				menuNumber--;
			if(retval==MENULEFT)
				menuNumber--;
			if(menuNumber==-1)
				menuNumber++;
			
			switch(menuNumber)
				{
					case FILEMENU:
						retval=handleFileMenu();
						break;
					case EDITMENU:
						retval=handleEditMenu();
						break;
					case TABSMENU:
						retval=handleTabsMenu();
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
	openTheFile(tmpEdFilePath,hilite);
	printLines();
}

void eventLoop(void)
{
	bool			handled=false;
	bool			dorefresh=false;
	bool			needsrefresh=false;
	unsigned char	buf[256]={0,};
	int				charsread;
	bool			donereadbuffer=true;
	int				totallinelen=0;
	int				cnt;
	char			tstr[3]={'_',0,0};

	while(true)
		{
			printStatusBar();
			fflush(NULL);
			memset(buf,0,255);
			charsread=read(STDIN_FILENO,&buf,255);
			handled=false;
			//for(int j=0;j<charsread;j++)
			//	fprintf(stderr,"char %i %c %x ",j,buf[j],buf[j]);
			//fprintf(stderr,"\n");

			//DEBUGFUNC(">>>>0=%x 0=%c 1=%x 1=%c 2=%x 2=%c 3=%x 3=%c",buf[0],buf[0],buf[1],buf[1],buf[2],buf[2],buf[3],buf[3]);
				cnt=0;
				tstr[1]=buf[0]+0x60;
				while(menuNames[cnt]!=NULL)
					{
						if(strcasestr((char*)menuNames[cnt],(char*)&tstr)!=NULL)
								{
									menuNumber=cnt;
									buf[0]=ESCCHAR;
									buf[1]=0;
									break;
								}
					cnt++;
				}

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
								if(charsread==1)
								{	
									writeCharToFile(buf[0]);
									refreshScreen();
									moveCursRite();
									break;
								}
							default:
								totallinelen=0;
								donereadbuffer=false;
								while(donereadbuffer==false)
									{
										for(int j=0;j<charsread;j++)
											{
												writeCharToFile(buf[j]);
												page->lineXCurs++;
												totallinelen++;
											}
										if(charsread<15)
											break;
										charsread=read(STDIN_FILENO,&buf,15);
									}
								page->lineXCurs--;
								fflush(NULL);
								if(totallinelen>1)
									dorefresh=true;

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
										if(totallinelen<2)
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
					openTheFile(tmpEdFilePath,hilite);
					printLines();
					adjCursor();
					continue;
				}
			fflush(NULL);
			fflush(STDIN_FILENO);
		}
}

void printStatusBar(void)
{
	findWordUnderCursor();
	printf("\e[m\e[%i;%iHLine=%i Column=%i   ",rows,1,currentY-minY+1,currentX-minX+1);
	printf("%s%s",wordBufPtr,CLEARTOEOL);
	printf("\e[%i;%iH",currentY,currentX);
}
