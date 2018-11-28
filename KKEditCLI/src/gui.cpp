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
#include <libgen.h>

int			menuNumber=0;
const char	*chkfname=NULL;

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

void drawBM(void)
{
	int	screenline=minY;
	for(int j=page->topLine; j<page->topLine+maxRows; j++)
		{
			if(j>page->editLineArray.size()-1)
				break;

			moveCursToTemp(6,screenline);
			printf(" ");
			for(int k=0;k<MAXBOOKMARKS;k++)
				{
					if(page->lineNumber.at(j)>0)
						{
							if(bookmarks[k].pageNum==page->pageNum)
								{
									//if(bookmarks[k].line==page->line[j].lineNum)
									if(bookmarks[k].line==page->lineNumber.at(j))
										{
											moveCursToTemp(6,screenline);
											SETINVERSEON
											printf("X");
											SETINVERSEOFF
										}
								}
						}
				}
			screenline++;
		}
}

void printLines(void)
{
	int	screenline=minY;

	HIDECURS;
	clearTrough();
	drawMenuBar();

	for(int j=page->topLine;j<page->topLine+maxRows;j++)
		{
			if(j>page->editLineArray.size()-1)
				break;

			if(page->lineNumber.at(j)!=-1)
				{
					moveCursToTemp(1,screenline);
					printf("\e[38;5;%im%.4i\e[m",lineColour,page->lineNumber.at(j));
				}
			moveCursToTemp(minX,screenline++);
			printf("%s%s",CLEARTOEOL,page->printLineArray.at(j).c_str());
		}
	printf(CLEARTOEOS);
	drawFilePath();
	drawBM();
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
			switchPage(atoi(ptr),-1);
		}
	return(menuselect);
}

int openInclude(const char *fpath,const struct stat *sb,int tflag,struct FTW *ftwbuf)
{
	if(tflag==FTW_F)
		{
			if(strcasecmp(fpath+ftwbuf->base,chkfname)==0)
				{
					initEditor();
					setTempEdFile(fpath);
					page->filePath=strdup(fpath);
					oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
					openTheFile(tmpEdFilePath,hilite);
					page->topLine=0;
					page->currentLine=0;
					currentX=minX;
					currentY=minY;
					page->lineXCurs=0;
				}
		}
	return(0);
}

int handleNavMenu(void)
{
	int		menuselect;
	int		cnt;
	bool	found;
	char	*gdef=NULL;

	menuselect=doMenuEvent(navMenuNames,16,2,true);
	switch(menuselect)
		{
			case NAVEGOTODEF:
				{
					cnt=0;
					if(functionsMenuNames==NULL)
						getTagList();

					found=false;
					while(functionData[cnt]!=NULL)
						{
							if(strcmp(wordBufPtr,functionData[cnt]->name)==0)
								{
									switchPage(-1,functionData[cnt]->line);
									found=true;
									break;
								}
							cnt++;
						}
					if(found==true)
						DEBUGFUNC("switchPage(-1,functionData[cnt]->line);","");
					else
						{
							char	*basedir;
							basedir=strdup(page->filePath);
							char	*dirn=dirname(basedir);
							gdef=oneLiner(false,"ctags -x %s/*|sort -k 2rb,2rb -k 1b,1b|sed 's@ \\+@ @g'|grep %s|awk '{print $3 \" \" $4}'",dirn,wordBufPtr);
							DEBUGFUNC("gdef=%s dirn=%s",gdef,dirn);
							free(gdef);
						}
				}
				clearScreen();
				refreshScreen();
				return(CONT);
				break;

			case NAVEOPENINC:
				{
					char		*thefilename=NULL;
					const char	*includepath=".";
					char		*basedir=NULL;
					char		*ptr=NULL;

					//save thefilename to free later
					basedir=strdup(page->filePath);
					thefilename=strdup(page->editLineArray.at(page->currentLine).c_str());
					ptr=thefilename;
					if(strstr(thefilename,"#include")==thefilename)
						{
							//local include ""
							if(strchr(thefilename,'"')!=NULL)
								{
									*strrchr(thefilename,'"')=0;
									thefilename=strchr(thefilename,'"')+1;
									includepath=(const char*)dirname(basedir);
								}
							else if(strchr(thefilename,'<')!=NULL)
								{
									//global include <>
									*strrchr(thefilename,'>')=0;
									thefilename=strchr(thefilename,'<')+1;
									includepath="/usr/include";
								}
						}

					chkfname=basename(thefilename);
					nftw(includepath,openInclude,20,FTW_PHYS);
					printLines();
					adjCursor();
					moveCursToTemp(currentX,currentY);
					free(basedir);
					free(ptr);
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
							switchPage(-1,atoi(dialog_vars.input_result));
						}
					clearScreen();
					refreshScreen();
				}
				break;

			case NAVOPENMANPAGE:
				{
					char	*command;

					asprintf(&command,"MANWIDTH=%i MAN_KEEP_FORMATTING=1 /usr/bin/man $(man -w %s) > /tmp/$(basename $(man -w %s))",maxCols,wordBufPtr,wordBufPtr);
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
	//int	line=0;

	if(functionsMenuNames==NULL)
		getTagList();

	menuselect=doMenuEvent((const char**)functionsMenuNames,27,2,false);
	if(menuselect>CONT)
		switchPage(-1,functionData[menuselect-1]->line);
	return(menuselect);
}

int findBM(int line,int pagenum)
{
	int	retval=0;

	while(retval!=MAXBOOKMARKS)
		{
			if((bookmarks[retval].line==line) && (bookmarks[retval].pageNum==pagenum))
				return(retval);
			retval++;
		}
	return(-1);
}

int handleBMMenu(void)
{
	int menuselect;
	int	findline=page->topLine+currentY-minY;
	int	bm=-1;

	menuselect=doMenuEvent((const char**)bookmarksMenuNames,37,2,true);

	if(menuselect<=CONT)
		return(menuselect);

	switch(menuselect)
		{
			case BMREMOVEALL:
				break;
			case BMTOGGLE:
				while(page->lineNumber.at(findline)==0)
					findline--;

				bm=findBM(page->lineNumber.at(findline),currentPage);
				if(bm==-1)
					{
						for(int j=0;j<MAXBOOKMARKS;j++)
							{
								if(bookmarks[j].line==-1)
									{
										bookmarks[j].line=page->lineNumber[findline];
										bookmarks[j].pageNum=currentPage;
										j=MAXBOOKMARKS;
									}
							}
					}
				else
					{
						for(int j=bm;j<MAXBOOKMARKS-1;j++)
							{
								bookmarks[j].line=bookmarks[j+1].line;
								bookmarks[j].pageNum=bookmarks[j+1].pageNum;
								bookmarks[j+1].line=-1;
								bookmarks[j+1].pageNum=-1;
							}
					}
				drawBM();
				buildBMMenu();
				break;
			default:
				{
					switchPage(bookmarks[menuselect-3].pageNum,bookmarks[menuselect-3].line);
				}
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
									page->lineXCurs=page->editLineArray.at(page->currentLine).length()-1;
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
									page->lineXCurs=page->editLineArray.at(page->currentLine).length()-1;
									adjCursor();
									handled=true;
									break;
//keys
								case PAGEDOWN:
									if(page->topLine+maxRows>=page->editLineArray.size())
										{
											currentX=minX;
											currentY=maxRows+minY-1;
											page->currentLine=page->editLineArray.size()-1;
											page->topLine=page->editLineArray.size()-maxRows;
											page->lineXCurs=0;
										}
									else
										{
											page->topLine+=maxRows;
											page->currentLine+=maxRows;
											currentY=minY;
											if(page->topLine+maxRows>=page->editLineArray.size())
												{
													currentY=minY;
													page->currentLine=page->editLineArray.size()-maxRows;
													page->topLine=page->editLineArray.size()-maxRows;
												}
										}
									printLines();
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
											if(deleteCharFromFile(false)==true)
												{
													dorefresh=true;
													needsrefresh=true;
													adjCursor();
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
								needsrefresh=true;
								handled=true;
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
										needsrefresh=true;
										page->lineXCurs--;
										adjCursor();
									}
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
											printf("%s",page->editLineArray.at(page->currentLine).c_str());
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
