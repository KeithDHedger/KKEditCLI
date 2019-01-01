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
	for(int j=page->topLine;j<page->topLine+(int)maxRows;j++)
		{
			if(j>(int)page->editLineArray.size()-1)
				break;

			moveCursToTemp(6,screenline);
			printf(" ");
			for(int k=0;k<MAXBOOKMARKS;k++)
				{
					if(page->lineNumber.at(j)>0)
						{
							if(bookmarks[k].pageNum==page->pageNum)
								{
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

	for(int j=page->topLine;j<page->topLine+(int)maxRows;j++)
		{
			if(j>(int)page->editLineArray.size()-1)
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

int handleFileMenu(bool doevent=true,int ms=0)
{
	int		menuselect;

	if(doevent==true)
		menuselect=doMenuEvent(fileMenuNames,1,2,true);
	else
		menuselect=ms;

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

int handleEditMenu(bool doevent=true,int ms=0)
{
	int menuselect;

	if(doevent==true)
		menuselect=doMenuEvent(editMenuNames,6,2,true);
	else
		menuselect=ms;

	switch(menuselect)
		{
			case EDITCUTW:
				freeAndNull(&cutBuffer);
				cutBuffer=strdup(wordBufPtr);
				page->editLineArray.at(page->currentLine).erase(wordStart,wordLen);
				return(MENUREFRESH);
				break;
			case EDITCOPYW:
				freeAndNull(&cutBuffer);
				cutBuffer=strdup(wordBufPtr);
				break;
			case EDITPASTE:
				if(cutBuffer!=NULL)
					{
						page->editLineArray.at(page->currentLine).insert(page->lineXCurs,cutBuffer,strlen(cutBuffer));
						return(MENUREFRESH);
					}
				break;			
			case EDITCUTL:
				freeAndNull(&cutBuffer);
				cutBuffer=strdup(page->editLineArray.at(page->currentLine).c_str());
				page->editLineArray.erase(page->editLineArray.begin()+page->currentLine);
				return(MENUREFRESH);
				break;
			case EDITCOPYL:
				freeAndNull(&cutBuffer);
				cutBuffer=strdup(page->editLineArray.at(page->currentLine).c_str());
				break;
		}
	return(menuselect);
}

int handleTabsMenu(void)
{
	int		menuselect;
	char	*ptr;

	if(tabsMenuNames==NULL)
		return(CONT);

	menuselect=doMenuEvent((const char**)tabsMenuNames,11,2,false);
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

int handleNavMenu(bool doevent=true,int ms=0)
{
	int		menuselect;
	int		cnt;
	bool	found;
	char	*gdef=NULL;

	if(doevent==true)
		menuselect=doMenuEvent(navMenuNames,16,2,true);
	else
		menuselect=ms;

	switch(menuselect)
		{
			case NAVFIND:
			case NAVFINDAGAIN:
				if(menuselect==NAVFIND)
					found=search(false);
				else
					found=search(true);
				clearScreen();
				refreshScreen();
				moveCursToTemp(currentX,currentY);
				if(found==true)
					printf(INVERTON "%s" INVERTOFF,needle.c_str());
				return(CONT);
				break;
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
									found=true;
									switchPage(-1,functionData[cnt]->line);
									break;
								}
							cnt++;
						}

					if(found==false)
						{
							char	*basedir;
							int		line;
							char	*fpath;
							char	*dirn;

							basedir=strdup(page->filePath);
							dirn=dirname(basedir);
							gdef=oneLiner(false,"ctags -x %s/*|sort -k 2rb,2rb -k 1b,1b|sed 's@ \\+@ @g'|grep %s|awk '{print $3 \" \" $4}'",dirn,wordBufPtr);
							if(strlen(gdef)>0)
								{
									line=atoi(gdef);
									fpath=strstr(gdef,"/");
									initEditor();
									setTempEdFile(fpath);
									page->filePath=strdup(fpath);
									oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
									openTheFile(tmpEdFilePath,hilite);
									currentX=minX;
									currentY=minY;
									switchPage(-1,line);
								}
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
					char		*retstr=NULL;
					CDKSCREEN	*cdkscreen;
					cdkscreen=initCDKScreen(NULL);
					initCDKColor ();
					retstr=getString(cdkscreen,"</5/B>Goto Line?","","");
					destroyCDKScreen(cdkscreen);
					endCDK ();

					if(retstr!=NULL)
						{
							switchPage(-1,atoi(retstr));
							free(retstr);
						}
					clearScreen();
					refreshScreen();
				}
				break;

			case NAVOPENMANPAGE:
				{
					char	*command;
					int		ret;

					asprintf(&command,"MANWIDTH=%i MAN_KEEP_FORMATTING=1 /usr/bin/man $(man -w %s 2>/dev/null) 2>/dev/null > /tmp/$(basename $(man -w %s) 2>/dev/null)",maxCols,wordBufPtr,wordBufPtr);
					ret=system(command);
					free(command);
					if(ret!=0)
						break;
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

int handleBMMenu(bool doevent=true,int ms=0)
{
	int menuselect;
	int	findline=page->topLine+currentY-minY;
	int	bm=-1;

	if(doevent==true)
		menuselect=doMenuEvent((const char**)bookmarksMenuNames,37,2,true);
	else
		menuselect=ms;

//	menuselect=doMenuEvent((const char**)bookmarksMenuNames,37,2,true);

	if(menuselect<=CONT)
		return(menuselect);

	switch(menuselect)
		{
			case BMREMOVEALL:
				for(int j=0;j<MAXBOOKMARKS;j++)
					{
						bookmarks[j].line=-1;
						bookmarks[j].pageNum=-1;
					}
				buildBMMenu();
				drawBM();
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

int handleHelpMenu(bool doevent=true,int ms=0)
{
	int menuselect;

	if(doevent==true)
		menuselect=doMenuEvent(helpMenuNames,53,2,true);
	else
		menuselect=ms;

	switch(menuselect)
		{
			case HELPHELP:
				initEditor();
				setTempEdFile(DATADIR "/help/help");
				oneLiner(true,"cp %s/help/help %s/%s",DATADIR,tmpEdDir,tmpEdFile);
				page->filePath=strdup(DATADIR "/help/help");
				openTheFile(tmpEdFilePath,false);
				refreshScreen();
				break;
			case HELPABOUT:
				initEditor();
				setTempEdFile(DATADIR "/help/about");
				oneLiner(true,"cp %s/help/about %s/%s",DATADIR,tmpEdDir,tmpEdFile);
				page->filePath=strdup(DATADIR "/help/about");
				openTheFile(tmpEdFilePath,false);
				refreshScreen();
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
						if(retval==MENUREFRESH)
							return(retval);
						break;
					case TABSMENU:
						retval=handleTabsMenu();
						break;
					case NAVIGATIONMENU:
						retval=handleNavMenu();
						break;
					case FUNCTIONSMENU:
						retval=handleFuncMenu();
						if(retval==MENUREFRESH)
							return(retval);
						break;
					case BOOKMARKSMENU:
						retval=handleBMMenu();
						break;
					case TOOLSMENU:
						retval=handleToolMenu();
						break;
					case HELPMENU:
						retval=handleHelpMenu();
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
#include <poll.h>
    int btn1=0;
  //  int middle=0;
   // int right=0;
    int fd, bytes;
bool loop=true;
bool pollmouse(void)
{
    int fd, bytes;
    unsigned char data[3];

    const char *pDevice = "/dev/input/mice";

    // Open Mouse
    fd = open(pDevice, O_RDWR);
    if(fd == -1)
    {
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
btn1=1;
data[0]=1;
data[1]=1;
data[2]=1;
data[3]=1;
int oldbtn=0;

//write(fd,data,4);
fprintf(stderr,"<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    signed char x, y;
    while(1)
    {
           if(btn1==0)
            {
            	loop=false;
           	return(true);
           	}
        //if(btn1==0 && oldbtn==1)
      // Read Mouse     
        bytes = read(fd, data, sizeof(data));

        if(bytes > 0)
        {
            btn1 = data[0] & 0x1;
        //if(btn1 != oldbtn)
           // right = data[0] & 0x2;
           // middle = data[0] & 0x4;

            x += data[1];
            y += data[2];
           //fprintf(stderr,"x=%d, y=%d, left=%d, oldbtn=%d, right=%d						\r", x, y, btn1, oldbtn, btn1);
       // if(btn1 != oldbtn)
        //if(btn1==0 && oldbtn==1)
        //	return(true);
             //if(btn1==0)
            //	return(true);
            if(btn1==1)
            	oldbtn=1;
         }   
           fprintf(stderr,"x=%d, y=%d, left=%d, oldbtn=%d, right=%d						\n", x, y, btn1, oldbtn, btn1);
    }
    
    
 //   return 0;
  return(false);
}
#include <termkey.h>
void eventLoop(void)
{
	bool			handled=false;
	bool			dorefresh=false;
	bool			needsrefresh=false;
	int				totallinelen=0;
	int				retval;
	int				shcnum;
	int				flip;
//const char *pDevice = "/dev/input/mice";
 //   fd = open(pDevice, O_RDWR);

	tk = termkey_new(0,TERMKEY_FLAG_CTRLC);
	//fprintf(stderr,"mouse0=%p\n",tk->);
	if(!tk)
		{
			fprintf(stderr, "Cannot allocate termkey instance\n");
			exit(1);
		}
//	int mouse = 100;

//printf("\033[?%dhMouse mode active\n", 1000);
	while(true)
		{
			printStatusBar();
			handled=false;
			SHOWCURS;
			fflush(NULL);
			fflush(STDIN_FILENO);
			ret=termkey_waitkey(tk,&key);

//char bufferm[50];
//							int line, col;
//termkey_strfkey(tk, buffer, sizeof bufferm, &key, format);
//fprintf(stderr,"%s at line=%d, col=%d\n", buffer, line, col);
	//				if(key.type == TERMKEY_TYPE_MOUSE)
		//				{
			//				termkey_interpret_mouse(tk, &key, NULL, NULL, &line, &col);
				///			fprintf(stderr,"%s at line=%d, col=%d\n", buffer, line, col);
					//	}



			//termkey_strfkey(tk, buffer, 50, &key, format);
			//fprintf(stderr,"Key from lib %s mods=%i\n", buffer,key.modifiers);
			switch(key.type)
				{
#if 0
					case TERMKEY_TYPE_MOUSE:
						{ 
						loop=true;
						int nextwait = 10;
						int line, col;
						  struct pollfd fd;


  char buffer[50];
 // termkey_strfkey(tk, buffer, sizeof buffer, key, TERMKEY_FORMAT_VIM);
  //printf(">>%s<<\n", buffer);
  
 // int line, col, btn;
 int btn=-1;
 int oldbtn=-1;
							TermKeyMouseEvent ev;
							termkey_interpret_mouse(tk, &key, &ev, &oldbtn, &line, &col);
//							fprintf(stderr,"\n\n\n%s button here no=%i is=%i at line=%d, col=%d\n\n\n",buffer, btn,ev,flip, col);
flip=btn;
//if(oldbtn==0)
//	break;


  fd.fd = 0; /* the file descriptor we passed to termkey_new() */
  fd.events = POLLIN;

//						printf("\033[?%dl\n", 1000);
//bool loop=true;
						while(loop==true)
						{
//						fprintf(stderr,"mouse0=%i %i %i %i\n",key.code.mouse[0],key.code.mouse[1],key.code.mouse[2],key.code.mouse[3]);
							termkey_interpret_mouse(tk, &key, &ev, &btn, &line, &col);
							//fprintf(stderr,"%s button no=%i is=%i at line=%d, col=%d\n",buffer, btn,ev,flip, col);
						//	if(btn
						//if(oldbtn!=btn)
						//	break;
						//if(flip!=btn)
						if(btn==0 && oldbtn==1)
						{
						loop=false;
							break;
							}
						//if(flip==btn)
						//	flip=-1;
						//else
						//	{
						//		fprintf(stderr,"\n\n\here\n\n");
						//		//break;
						//	}
						fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
						    if(poll(&fd, 1, nextwait) == 0)
    {
      // Timed out
      if(termkey_getkey_force(tk, &key) == TERMKEY_RES_KEY)
       if (pollmouse()==true)
       {
     	loop=false;
     	break;
     	}
     // on_key(tk, &key);
     	// pollmouse();
    //  	loop=false;
    }
//printf("\033[?%dhMouse mode active\n", 1000);
    if(fd.revents & (POLLIN|POLLHUP|POLLERR))
      termkey_advisereadable(tk);

    while(((ret = termkey_getkey(tk, &key)) == TERMKEY_RES_KEY) && loop==true)
    {
     // on_key(tk, &key);
     if (pollmouse()==true)
     {
     	loop=false;
     	break;
     	}
     //pollmouse();
//if(key.type == TERMKEY_TYPE_MOUSE)
//{
//	loop=false;
//	break;
//}
//      if(key.type == TERMKEY_TYPE_UNICODE &&
//         key.modifiers & TERMKEY_KEYMOD_CTRL &&
//         (key.code.codepoint == 'C' || key.code.codepoint == 'c'))
//        break;
    }

  //  if(ret == TERMKEY_RES_AGAIN)
   //   nextwait = termkey_get_waittime(tk);
    //else
      nextwait = 100;

						
						
						
						
						//fflush(NULL);
						//	pollmouse();
						//termkey_getkey(tk, &key);
						//fprintf(stderr,"mouse0=%i %i %i %i\r",key.code.mouse[0],key.code.mouse[1],key.code.mouse[2],key.code.mouse[3]);
//						  struct pollfd fd;
//
//						  fd.fd = 0; /* the file descriptor we passed to termkey_new() */
//						  fd.events = POLLIN;
//						if(poll(&fd, 1, nextwait) == 0)
//						{
//      // Timed out
//					      if(termkey_getkey_force(tk, &key) == TERMKEY_RES_KEY)
//					      	{
//					      		fprintf(stderr,"%s at line=%d, col=%d key=%i\r", buffer, line, col,key.type);
//					      	}
//					    if(fd.revents & (POLLIN|POLLHUP|POLLERR))
//					      termkey_advisereadable(tk);
//
//					    while((ret = termkey_getkey(tk, &key)) == TERMKEY_RES_KEY)
//					    {
//					      //on_key(tk, &key);
//							fprintf(stderr,"%s at line=%d, col=%d key=%i\r", buffer, line, col,key.type);
//					     // if(key.type == TERMKEY_TYPE_UNICODE && key.modifiers & TERMKEY_KEYMOD_CTRL && (key.code.codepoint == 'C' || key.code.codepoint == 'c'))
//        					if(key.type == TERMKEY_TYPE_MOUSE)
//	        					break;;
//    					}
//
//  					  if(ret == TERMKEY_RES_AGAIN)
//					      nextwait = termkey_get_waittime(tk);
//					   else
//					      nextwait = -1;
////					     	 //exit(0);
//					     	 
//					     	    if(ret == TERMKEY_RES_AGAIN)
//							      nextwait = termkey_get_waittime(tk);
//    							else
//     							 nextwait = -1;
//
//					      }
//							//termkey_getkey(tk,&key);
//							
//						//
//							//fprintf(stderr,">>>>>>>>>>>>>>>\n");
//							int line, col;
//							termkey_interpret_mouse(tk, &key, NULL, NULL, &line, &col);
//							fprintf(stderr,"%s at line=%d, col=%d key=%i\r", buffer, line, col,key.type);
//							
//							//termkey_advisereadable(tk);
//							//if(termkey_getkey(tk,&key)==TERMKEY_RES_NONE)
//							//	break;
//							}
						}
						fprintf(stderr,"\n\n------------------------------------------------------------------------------------>\n");
						break;
						}
						 break;
#endif
					case TERMKEY_TYPE_KEYSYM:
						{
							switch(key.code.sym)
								{
									case TERMKEY_SYM_BACKSPACE:
									case TERMKEY_SYM_DEL:
										if(deleteCharFromFile(true)==true)
											{
												dorefresh=true;
												needsrefresh=true;
												page->lineXCurs--;
												adjCursor();
											}
										//fprintf(stderr,"BACKSPACE key\n");
										break;
									case  TERMKEY_SYM_ENTER:
										writeCharToFile(RETURNKEY);
										currentX=minX;
										currentY++;
										page->currentLine++;
										page->lineXCurs=0;
										dorefresh=true;
										needsrefresh=true;
										handled=true;
										//fprintf(stderr,"ENTER key\n");
										break;
									case TERMKEY_SYM_ESCAPE:
										{
											int retval=BRAKE;
											menuStart=0;
											retval=handleAllMenus();
											if(retval==BRAKE)
												return;
											if(retval==MENUREFRESH)
												{
													writeFile();
													dorefresh=true;
													needsrefresh=true;
													break;
												}
											continue;
										}
										//fprintf(stderr,"Escape key\n");
										break;
									case TERMKEY_SYM_UP:
										moveCursUp();
										adjCursor();
										handled=true;
									//	fprintf(stderr,"Up key\n");
										break;
									case TERMKEY_SYM_DOWN:
										moveCursDown();
										adjCursor();
										handled=true;
										//fprintf(stderr,"Down key\n");
										break;
									case TERMKEY_SYM_LEFT:
										moveCursLeft();
										handled=true;
									//	fprintf(stderr,"Left key\n");
										break;
									case TERMKEY_SYM_RIGHT:
										moveCursRite();
										handled=true;
									//	fprintf(stderr,"Right key\n");
										break;
									case TERMKEY_SYM_PAGEUP:
										page->currentLine-=maxRows;
										page->topLine-=maxRows;
										if(page->currentLine<0)
											{
												page->topLine=0;
												page->currentLine=0;
												currentX=minX;
												currentY=minY;
												page->lineXCurs=0;
											}
										else
											{
												if(page->topLine<0)
													{
														currentY=minY;
														page->topLine=page->currentLine;
													}
											}
										handled=true;
										needsrefresh=true;
									//	fprintf(stderr,"PAGEUP key\n");
										break;
									case TERMKEY_SYM_PAGEDOWN:
										if(page->editLineArray.size()<maxRows)
											{
												SHOWCURS;
												handled=true;
												break;
											}
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
												if(page->topLine+maxRows>=page->editLineArray.size())
													{
														currentY=minY;
														page->currentLine=page->editLineArray.size()-maxRows;
														page->topLine=page->editLineArray.size()-maxRows;
													}
											}
										handled=true;
										needsrefresh=true;
										//fprintf(stderr,"PAGEDOWN key\n");
										break;
									case TERMKEY_SYM_HOME:
										page->lineXCurs=0;
										adjCursor();
										handled=true;
										needsrefresh=false;
									//	fprintf(stderr,"HOME key\n");
										break;
									case TERMKEY_SYM_END:
										page->lineXCurs=page->editLineArray.at(page->currentLine).length()-1;
										adjCursor();
										handled=true;
										needsrefresh=false;
									//	fprintf(stderr,"END key\n");
										break;
									case  TERMKEY_SYM_DELETE:
										if(deleteCharFromFile(false)==true)
											{
												dorefresh=true;
												needsrefresh=true;
												adjCursor();
												needsrefresh=true;
												handled=true;
											}
									//	fprintf(stderr,"DELETE key\n");
										break;
									case  TERMKEY_SYM_TAB:
										writeCharToFile('\t');
										refreshScreen();
										moveCursRite();
										break;
									default:
										//fprintf(stderr,"unchecked key %i\n",key.code.sym);
										break;
								}
						}
						break;

					case TERMKEY_TYPE_UNICODE:
						{
							switch(key.modifiers)
								{
									case TERMKEY_KEYMOD_ALT:
										{
											char tstr[3]={'_',0,0};
											int retval=BRAKE;
											for(int j=0;j<MAXMENUS;j++)
												{
													tstr[1]=toupper(key.code.codepoint);
													if(strcasestr((char*)menuNames[j],(char*)&tstr)!=NULL)
														{
															menuNumber=j;
															menuStart=0;
															retval=handleAllMenus();
															if(retval==BRAKE)
																return;
															if(retval==MENUREFRESH)
																{
																	writeFile();
																	dorefresh=true;
																	needsrefresh=true;
																	break;
																}
															break;
														}
												}
											}
										break;
									case TERMKEY_KEYMOD_CTRL:
										retval=CONT;
										shcnum=0;
										while(scs[shcnum].key!=0)
											{
												if(key.code.codepoint==scs[shcnum].key)
													{
														menuNumber=scs[shcnum].menuNum;
														switch(scs[shcnum].menuNum)
															{
																case FILEMENU:
																	retval=handleFileMenu(false,scs[shcnum].menuItem);
																	break;
																case EDITMENU:
																	retval=handleEditMenu(false,scs[shcnum].menuItem);
																	break;
																case NAVIGATIONMENU:
																	retval=handleNavMenu(false,scs[shcnum].menuItem);
																	break;
																case BOOKMARKSMENU:
																	retval=handleBMMenu(false,scs[shcnum].menuItem);
																	break;
																case HELPMENU:
																	retval=handleHelpMenu(false,scs[shcnum].menuItem);
																	break;
																default:
																	break;
															}
														break;
													}
												shcnum++;
											}

										if(retval==BRAKE)
											return;

										if(retval==MENUREFRESH)
											{
												writeFile();
												dorefresh=true;
												needsrefresh=true;
											}
										if(retval!=BRAKE)
											{
												handled=true;
												needsrefresh=false;
											}
									//	fprintf(stderr,"Do ctrl ...\n");
										break;

									default:
										//fprintf(stderr,"buf remain=%i\n",termkey_get_buffer_remaining(tk));
//										fprintf(stderr,"Key==%s = %x\n",key.utf8,key.code.sym);
										totallinelen=0;
										writeCharToFile(key.code.codepoint);
										page->lineXCurs++;

										while(termkey_getkey(tk,&key)!=TERMKEY_RES_NONE)
											{
										//fprintf(stderr,"buf remain=%i\n",termkey_get_buffer_remaining(tk));
												if(key.type==TERMKEY_TYPE_KEYSYM)
													{
														switch(key.code.sym)
															{
																case TERMKEY_SYM_TAB:
																	writeCharToFile('\t');
																	break;
																case TERMKEY_SYM_ENTER:
																	{
																		dorefresh=true;
																		writeCharToFile('\n');
																	}
																	break;
																default:
																	break;
															}
													}
												else
													writeCharToFile(key.code.codepoint);
												page->lineXCurs++;
											}
										page->lineXCurs--;

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
												if(liveUpdate==true)
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
							break;
					default:
						break;
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
		}
}

void printStatusBar(void)
{
	findWordUnderCursor();
	printf("\e[m\e[%i;%iHLine=%i Column=%i   ",rows,1,currentY-minY+1,currentX-minX+1);
	printf("%s%s",wordBufPtr,CLEARTOEOL);
	printf("\e[%i;%iH",currentY,currentX);
}
