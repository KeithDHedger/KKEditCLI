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
	char	*tfile;
	int		status;
	char	*message=NULL;

	menuselect=doMenuEvent(fileMenuNames,1,2);
	switch(menuselect)
		{
			case FILENEW:
				askSaveIfdirty();
				closePage();
				initEditor();
				asprintf(&page->filePath,"/tmp/Untitled-%i",newFileNum++);
				tmpedfile="/dev/shm/edfile";
				oneLiner(true,"echo \"New File\" > %s",page->filePath);
				oneLiner(true,"cp %s %s",page->filePath,tmpedfile);
				openTheFile(page->filePath);
				currentX=minX;
				currentY=minY;
				refreshScreen();
				moveCursToTemp(minX,currentY);			
				break;
			case FILEOPEN:
				askSaveIfdirty();
				asprintf(&message,"%s",page->filePath);
				closePage();
				init_dialog(stdin,stdout);
					status=dialog_fselect("Open File",message,rows-14,cols-14);
           		end_dialog();
				dlg_clear();
				clearScreen();
				initEditor();
				if(dialog_vars.input_result==NULL)
					{
						oneLiner(true,"cp %s %s",message,tmpedfile);
						page->filePath=strdup(message);
						openTheFile(tmpedfile);
						refreshScreen();
					}
				else
					{
						oneLiner(true,"cp %s %s",dialog_vars.input_result,tmpedfile);
						page->filePath=strdup(dialog_vars.input_result);
						openTheFile(tmpedfile);
						refreshScreen();
					}
				free(message);
				moveCursToTemp(minX,currentY);
				break;
			case FILESAVE:
				saveFile(page->filePath);
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

	menuselect=doMenuEvent(editMenuNames,6,2);
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

	menuselect=doMenuEvent(viewMenuNames,11,2);
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

	menuselect=doMenuEvent(navMenuNames,16,2);
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

void getRecursiveTagList(char *filepath,void *ptr)
{
	FILE		*fp;
	char		line[2048];
//	GString	*str=g_string_new(NULL);
	char		*command;
	char		*newstr=NULL;
	char		*sort=NULL;

	if(filepath==NULL)
		return;

//	switch(listFunction)
//		{
//		case 0:
//			sinkReturn=asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
//			break;
//		case 1:
//			sinkReturn=asprintf(&sort,"sort -k 2rb,2rb -k 3n,3n");
//			break;
//		case 2:
//			sinkReturn=asprintf(&sort,"sort -k 3n");
//			break;
//		case 4:
			asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
//			break;
//		default:
//			asprintf(&sort,"sort");
//			break;
//		}

	asprintf(&command,"find \"%s\" -maxdepth %i|%s -L - -x|%s|sed 's@ \\+@ @g'",filepath,1,"/usr/bin/ctags",sort);
	fp=popen(command, "r");
	while(fgets(line,2048,fp))
		{
			//newstr=globalSlice->deleteSlice(line,filepath);
			//if(globalSlice->getResult()==NOERROR)
			//	{
			//		g_string_append_printf(str,"%s",newstr);
			//		ERRDATA debugFree(&newstr);
			//	}
			fprintf(stderr,"%s",line);
			fflush(NULL);
		}
	pclose(fp);

//	*((char**)ptr)=str->str;
//	g_string_free(str,false);
//	ERRDATA debugFree(&command);
//	ERRDATA debugFree(&sort);
}


int handleFuncMenu(void)
{
	int menuselect;
getRecursiveTagList(page->filePath,NULL);
	menuselect=doMenuEvent(functionsMenuNames,27,2);
	switch(menuselect)
		{
			case FUNC0:
				break;
			case FUNC1:
				break;
		}
	return(menuselect);
}

int handleBMMenu(void)
{
	int menuselect;

	menuselect=doMenuEvent(bookmarksMenuNames,37,2);
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

	menuselect=doMenuEvent(toolsMenuNames,47,2);
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
			free(page->line[j].srcLine);
			free(page->line[j].edLine);
			page->line[j].srcLine=NULL;
			page->line[j].edLine=NULL;
		}
	page->maxLines=0;
	openTheFile(tmpedfile);
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
								case CURSHOMECONS:
									page->lineXCurs=0;
									adjCursor();
									handled=true;
									break;
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
								case KEYUP:
									moveCursUp();
									handled=true;
									break;
								case KEYDOWN:
									moveCursDown();
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
							free(page->line[j].srcLine);
							free(page->line[j].edLine);
						}
					page->maxLines=0;
					openTheFile(tmpedfile);
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
