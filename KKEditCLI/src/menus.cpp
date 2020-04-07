/*
 *
 * ©K. D. Hedger. Mon  6 May 17:15:46 BST 2019 keithdhedger@gmail.com

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

#include <cursesGlobals.h>

#include "menus.h"

const char	*menuNames[]= {"File","Edit","Tabs","Navigation","Functions","Bookmarks","Tools","Help",NULL};
const char	*fileMenuNames[]= {" _New"," _Open"," _Save"," Save _As"," Clos_e"," S_hell"," _Quit",NULL};
const char	*editMenuNames[]= {" _Copy"," C_ut"," _Paste",NULL};
const char	*tabMenuNames[]= {" Next Tab"," Prev Tab",NULL};
const char	*navMenuNames[]= {" Goto _Define"," _Open Include"," Goto L_ine"," Open _Manpage"," _Find"," Find A_gain",NULL};
const char	*bookmarkMenuNames[]= {" _Remove All Marks"," _Toggle BM",NULL};
//const char	*toolsMenuNames[]= {" Manage Tools",NULL};
const char	*helpMenuNames[]= {" _Help"," A_bout",NULL};

shortcutStruct	scKeys[]={{FILEMENU,QUITITEM,'q'},{FILEMENU,NEWITEM,'n'},{FILEMENU,SAVEITEM,'s'},{FILEMENU,SAVEASITEM,'a'},{FILEMENU,CLOSEITEM,'w'},{FILEMENU,SHELLITEM,'h'},{FILEMENU,OPENITEM,'o'},{EDITMENU,COPYWORD,'c'},{EDITMENU,CUTWORD,'x'},{EDITMENU,PASTE,'v'},
{NAVMENU,NAVGOTODFINE,'d'},
{NAVMENU,NAVOPENINCLUDE,'u'},
{NAVMENU,NAVGOTOLINE,'l'},
{NAVMENU,NAVOPENMANPAGE,'p'},
{NAVMENU,NAVFIND,'f'},
{NAVMENU,NAVFINDNEXT,'g'},
{HELPMENU,HELP,'h'},
{HELPMENU,ABOUT,'b'},
{-1,-1,0}};

int			foundX=-1;
int			foundY=0;
std::string	findString="";
std::string	lowerneedle="";

void rebuildBMMenu(void)
{
	int		cnt=0;
	char	buffer[4096]={0,};
	CTK_cursesEditBoxClass	*edbox;

	mainApp->menuBar->CTK_clearMenu(BMMENU);
	while(bookmarkMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(BMMENU,bookmarkMenuNames[cnt++]);

	mainApp->menuBar->CTK_setMenuShortCut(BMMENU,RMALLBMS,'k');
	mainApp->menuBar->CTK_setMenuShortCut(BMMENU,TOGGLEBM,'t');

	for(int j=0;j<bms.size();j++)
		freeAndNull(&bms[j].label);
	bms.clear();

	for(unsigned j=0;j<mainApp->pages.size();j++)//TODO// duplicate code
		{
			//if(mainApp->pages[j].srcEditBoxes.size()!=0)
			edbox=static_cast<CTK_cursesEditBoxClass*>(mainApp->CTK_getGadgetNum(j,SRCGADGET,1));
			if(edbox!=NULL)
				{
					for(int k=0;k<edbox->CTK_getLineCnt();k++)
						{
							if(edbox->CTK_getBookMark(k)==true)
								{
									bookmarkStruct bm;
									bm.pageNum=j;
									bm.lineNum=edbox->CTK_getLineAtY(k);
									sprintf(buffer," Tab %i, Line %i ",bm.pageNum,bm.lineNum);
									bm.label=strdup(buffer);
									bm.isSrc=true;
									bms.push_back(bm);
									mainApp->menuBar->CTK_addMenuItem(BMMENU,(const char*)buffer);
									//free(buffer);
								}
						}
				}

			//if(mainApp->pages[j].editBoxes.size()!=0)
			edbox=static_cast<CTK_cursesEditBoxClass*>(mainApp->CTK_getGadgetNum(j,EDITGADGET,1));
			if(edbox!=NULL)
			//if(mainApp->pages[j].editBoxes.size()!=0)
				{
					for(int k=0;k<edbox->CTK_getLineCnt();k++)
						{
							if(edbox->CTK_getBookMark(k)==true)
								{
									bookmarkStruct bm;
									bm.pageNum=j;
									bm.lineNum=edbox->CTK_getLineAtY(k);
									sprintf(buffer," Tab %i, Line %i ",bm.pageNum,bm.lineNum);
									bm.label=strdup(buffer);
									bm.isSrc=false;
									bms.push_back(bm);
									mainApp->menuBar->CTK_addMenuItem(BMMENU,(const char*)buffer);
								}
						}
				}
		}
}

void rebuildTabMenu(void)
{
	int		cnt=0;
	char	*buffer;

	mainApp->menuBar->CTK_clearMenu(TABMENU);
	while(tabMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(TABMENU,tabMenuNames[cnt++]);

	for(unsigned j=0; j<mainApp->pages.size(); j++)
		{
			asprintf(&buffer," %s ",(const char*)mainApp->pages[j].userData);
			mainApp->menuBar->CTK_addMenuItem(TABMENU,buffer);
			free(buffer);
		}
	getTagList((const char*)mainApp->pages[mainApp->pageNumber].userData);
	rebuildBMMenu();
}

void handleFileMenu(CTK_cursesMenuClass *mc)
{
	CTK_cursesEditBoxClass	*box=NULL;
	char					buffer[PATH_MAX]={0,};
	CTK_cursesGadgetClass	*srcbox;
	CTK_cursesGadgetClass	*edbox;
	CTK_cursesSourceEditBoxClass	*sourcebox;

	srcbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1);
	edbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,EDITGADGET,1);
	if(srcbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(srcbox);//static_cast<CTK_cursesEditBoxClass*>(app->pages[app->pageNumber].srcEditBoxes[0]);
	else if(edbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(edbox);

	if(box==NULL)
		return;

//	if(mainApp->pages[mainApp->pageNumber].srcEditBoxes.size()>0)
//		box=static_cast<CTK_cursesEditBoxClass*>(mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]);
//	else if(mainApp->pages[mainApp->pageNumber].editBoxes.size()>0)
//		box=mainApp->pages[mainApp->pageNumber].editBoxes[0];

	switch(mc->menuItemNumber)
		{
			case NEWITEM:
				{
					char	*uddata;
					asprintf(&uddata,"/tmp/Untitled-%i",++newCnt);
					sprintf(buffer,"touch %s",uddata);
					system(buffer);
					box->CTK_setRunLoop(false);
					mainApp->CTK_addPage();
					sourcebox=mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,uddata);
					sourcebox->CTK_setShowLineNumbers(showLineNumbers);
					mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)uddata);
					setInfoLabel();
					rebuildTabMenu();
					mainApp->CTK_clearScreen();
				}
			break;

			case OPENITEM:
				{
					std::string	str;
					char		*buffer=get_current_dir_name();

					mainApp->utils->CTK_fileChooserDialog(buffer,CUOPENFILE);
					if(mainApp->utils->dialogReturnData.isValidData==true)
						{
							box->CTK_setRunLoop(false);
							mainApp->CTK_clearScreen();
							mainApp->CTK_addPage();
							sourcebox=mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,mainApp->utils->dialogReturnData.stringValue.c_str());
							sourcebox->CTK_setShowLineNumbers(showLineNumbers);
							mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(mainApp->utils->dialogReturnData.stringValue.c_str()));
							setInfoLabel();
							rebuildTabMenu();
						}
					free(buffer);
				}
				break;

			case SAVEITEM:
				{
					char	*buf;
					buf=box->CTK_getBuffer();
					FILE *f=fopen((char*)mainApp->pages[mainApp->pageNumber].userData,"w+");
					if(f!=NULL)
						{
							box->CTK_setRunLoop(false);
							fprintf(f,"%s",buf);
							box->isDirty=false;
							fclose(f);
							getTagList((const char*)mainApp->pages[mainApp->pageNumber].userData);
						}
					free(buf);
					setInfoLabel();
				}
				break;

			case SAVEASITEM:
				{
					char				*buf;
					char					*buffer=(char*)alloca(PATH_MAX);
					CTK_cursesUtilsClass	cu;
					char					*holdstr=strdup((char*)mainApp->pages[mainApp->pageNumber].userData);

					buf=box->CTK_getBuffer();
					cu.CTK_fileChooserDialog(dirname(holdstr),CUSAVEFILE,NULL,basename(holdstr));
					if(cu.dialogReturnData.isValidData==true)
						{
							sprintf(buffer,"%s",cu.dialogReturnData.stringValue.c_str());
							FILE *f=fopen(buffer,"w+");
							if(f!=NULL)
								{
									fprintf(f,"%s",buf);
									freeAndNull((char**)&mainApp->pages[mainApp->pageNumber].userData);
									mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(buffer));
									fclose(f);
									box->isDirty=false;
									rebuildTabMenu();
									fflush(NULL);
								}
							setInfoLabel();
						}
					free(buf);
					free(holdstr);
				}
				break;

			case CLOSEITEM:
				box->CTK_setRunLoop(false);
				if(box->isDirty==true)
					{
						CTK_cursesUtilsClass	cu;
					//	cu.CTK_queryDialog(mainApp,"File has changed ...\nDo you want to save it?",(const char*)mainApp->pages[mainApp->pageNumber].userData,"Save ...",YESBUTTON|NOBUTTON);
						cu.CTK_queryDialog("File has changed ...\nDo you want to save it?",(const char*)mainApp->pages[mainApp->pageNumber].userData,"Save ...",YESBUTTON|NOBUTTON);
						if((cu.dialogReturnData.isValidData) && (cu.dialogReturnData.intValue==CUQUERYOK))
							{
								FILE *f=fopen((char*)mainApp->pages[mainApp->pageNumber].userData,"w+");
								if(f!=NULL)
									{
										box->CTK_setRunLoop(false);
										fprintf(f,"%s",box->CTK_getBuffer());
										box->isDirty=false;
										fclose(f);
										getTagList((const char*)mainApp->pages[mainApp->pageNumber].userData);
									}
							}
					}
				freeAndNull((char**)&(mainApp->pages[mainApp->pageNumber].userData));
				mainApp->CTK_removePage(mainApp->pageNumber);
				mainApp->CTK_clearScreen();
				if(mainApp->pageNumber==-1)
					mainApp->runEventLoop=false;
				else
					rebuildTabMenu();
				break;

			case SHELLITEM:
				{
					CTK_cursesShellClass shell(mainApp);
					shell.CTK_startShell();
				}
				break;

			case QUITITEM:
				for(int j=0;j<mainApp->pages.size();j++)
					{
						sourcebox=static_cast<CTK_cursesSourceEditBoxClass*>(mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1));
						if(sourcebox!=NULL)
							{
								if(sourcebox->isDirty==true)
									{
										CTK_cursesUtilsClass	cu;
										cu.CTK_queryDialog("File has changed ...\nDo you want to save it?",(const char*)mainApp->pages[j].userData,"Save ...",ALLBUTTONS);
										//fprintf(stderr,"Button pressed=%i\n",cu.intResult);
										if((cu.dialogReturnData.isValidData) && (cu.dialogReturnData.intValue==CUENTRYCANCEL))
										//if(cu.intResult & CANCELBUTTON)
											{
												return;
											}

										//if(cu.intResult & YESBUTTON)
										if((cu.dialogReturnData.isValidData) && (cu.dialogReturnData.intValue==CUQUERYOK))
											{
												FILE *f=fopen((char*)mainApp->pages[mainApp->pageNumber].userData,"w+");
												if(f!=NULL)
													{
														box->CTK_setRunLoop(false);
														fprintf(f,"%s",box->CTK_getBuffer());
														box->isDirty=false;
														fclose(f);
														getTagList((const char*)mainApp->pages[mainApp->pageNumber].userData);
													}
											}
									}
								sourcebox->CTK_setRunLoop(false);
							}
					}
				mainApp->runEventLoop=false;
				break;
		}
}

void handleEditMenu(CTK_cursesMenuClass *mc)
{
	CTK_cursesGadgetClass	*srcbox;
	CTK_cursesGadgetClass	*edbox;
	CTK_cursesEditBoxClass	*box=NULL;

	srcbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1);
	edbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,EDITGADGET,1);
	if(srcbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(srcbox);
	else if(edbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(edbox);

	if(box==NULL)
		return;

	switch(mc->menuItemNumber)
		{
			case COPYWORD:
				if(box->isSelecting==true)
					{
						clip=box->CTK_getSelection();
						box->CTK_finishSelecting();
					}
				else
					clip=box->CTK_getCurrentWord();
				break;

			case CUTWORD:
				if(box->isSelecting==true)
					{
						clip=box->CTK_getSelection();
						box->CTK_deleteSelection();
					}
				else
					{
						clip=box->CTK_getCurrentWord();
						box->CTK_deleteCurrentWord();
					}
				box->isDirty=true;
				box->CTK_getBuffer();
				break;

			case PASTE:
				box->CTK_insertText(clip.c_str());
				box->isDirty=true;
				box->CTK_getBuffer();
				break;
		}
}

void handleNavMenu(CTK_cursesMenuClass *mc)
{
	std::string	tclip;
	char		*basedir;
	const char	*dirn;
	char		*gdef=NULL;
	int			line;
	const char	*fpath;
	CTK_cursesGadgetClass	*srcbox;
	CTK_cursesGadgetClass	*edbox;
	CTK_cursesEditBoxClass	*box=NULL;
	CTK_cursesSourceEditBoxClass	*sourcebox;

	srcbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1);
	edbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,EDITGADGET,1);
	if(srcbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(srcbox);
	else if(edbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(edbox);

	switch(mc->menuItemNumber)
		{
			case NAVGOTODFINE:
				tclip=box->CTK_getCurrentWord();
				for(int j=0;j<functions.size();j++)
					{
						if(strcmp(tclip.c_str(),functions[j]->name)==0)
							{
								box->CTK_gotoLine(functions[j]->line);
								return;
								break;
							}
						}
					basedir=strdup((char*)mainApp->pages[mainApp->pageNumber].userData);
					dirn=dirname(basedir);
					gdef=oneLiner(false,"ctags -x %s/* 2>/dev/null|sort -k 2rb,2rb -k 1b,1b|sed 's@ \\+@ @g'|grep %s|awk '{print $3 \" \" $4}' 2>/dev/null",dirn,tclip.c_str());
					free(basedir);
					
					if(strlen(gdef)>0)
						{
							line=atoi(gdef);
							fpath=strstr(gdef,"/");
							box->CTK_setRunLoop(false);
							for(int j=0;j<mainApp->pages.size();j++)
								{
									if(strcmp((char*)mainApp->pages[j].userData,fpath)==0)
										{
											sourcebox=getSrcBox(mainApp->pageNumber);//TODO//
											sourcebox->CTK_setRunLoop(false);
											mainApp->CTK_setPage(j);
											sourcebox->CTK_gotoLine(line);
											mainApp->CTK_updateScreen(mainApp,NULL);
											freeAndNull(&gdef);
											return;
										}
								}
							mainApp->CTK_addPage();
							sourcebox=mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,fpath);
							sourcebox->CTK_setShowLineNumbers(showLineNumbers);
							mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(fpath));
							sourcebox->CTK_gotoLine(line);
							setInfoLabel();
							rebuildTabMenu();
							mainApp->CTK_updateScreen(mainApp,NULL);
						}
					freeAndNull(&gdef);
					break;

			case NAVOPENINCLUDE:
				tclip=box->CTK_getCurrentLine();
				if(strstr(tclip.c_str(),"#include")!=NULL)
					{
						if(strstr(tclip.c_str(),"\"")!=NULL)
							{
								char	*ptr0=strdup(tclip.c_str());
								char	*ptr=strstr(ptr0,"\"");
								char	*filepath;
								ptr++;
								ptr[strlen(ptr)-2]=0;
								fprintf(stderr,"%s\n",ptr);
								basedir=strdup((char*)mainApp->pages[mainApp->pageNumber].userData);
								dirn=dirname(basedir);
								asprintf(&filepath,"%s/%s",dirn,ptr);
								free(basedir);
								free(ptr0);
								for(int j=0;j<mainApp->pages.size();j++)
									{
										if(strcmp((char*)mainApp->pages[j].userData,filepath)==0)
											{
												sourcebox=getSrcBox(mainApp->pageNumber);//TODO//
												sourcebox->CTK_setRunLoop(false);
												mainApp->CTK_setPage(j);
												mainApp->CTK_updateScreen(mainApp,NULL);
												free(filepath);
												return;
											}
									}
							
							}

						if(strstr(tclip.c_str(),"<")!=NULL)
							{
								char	*ptr0=strdup(tclip.c_str());
								char	*ptr=strstr(ptr0,"<");
								char	*filepath;
								char	*command;
								FILE	*fp;

								ptr++;
								ptr[strlen(ptr)-2]=0;
								filepath=basename(ptr);
								asprintf(&command,"find /usr/include -name \"%s\"",filepath);
								filepath=(char*)alloca(2048);
								box->CTK_setRunLoop(false);
								fp=popen(command,"r");
								if(fp!=NULL)
									{
										while(fgets(filepath,2048,fp))
											{
												if(strlen(filepath)>1)
													filepath[strlen(filepath)-1]=0;
										
												mainApp->CTK_addPage();
												sourcebox=mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,filepath);
												sourcebox->CTK_setShowLineNumbers(showLineNumbers);
												mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(filepath));
												setInfoLabel();
											}
										pclose(fp);
									}
								free(ptr0);
								free(command);
								rebuildTabMenu();
								mainApp->CTK_clearScreen();
								mainApp->CTK_updateScreen(mainApp,NULL);
							}
					}
				break;

			case NAVGOTOLINE:
				{
					if(mainApp->utils->CTK_entryDialog("Goto Line Number?","","Jump To Line ...","",true)==true)
						{
							sourcebox=getSrcBox(mainApp->pageNumber);
							sourcebox->CTK_gotoLine(atoi(mainApp->utils->dialogReturnData.stringValue.c_str()));
							mainApp->menuBar->CTK_drawDefaultMenuBar();
							break;
						}
				}
				break;

			case NAVOPENMANPAGE:
				{
					char	*command;
					FILE	*fp;
					char	filepath[2048];
					tclip=box->CTK_getCurrentWord();
					box->CTK_setRunLoop(false);
					asprintf(&command,"man -aw %s",tclip.c_str());
					fp=popen(command,"r");
					free(command);
					if(fp!=NULL)
						{
							while(fgets(filepath,2048,fp))
								{
									if(strlen(filepath)>1)
										filepath[strlen(filepath)-1]=0;
									mainApp->CTK_addPage();
									asprintf(&command,"MAN_KEEP_FORMATTING=1 MANWIDTH=%i man %s|sed 's/" UNDERLINEON BOLDOFF "/" UNDERLINE "/g;s/" UNDERLINEON "/" UNDERLINE "/g;s/" UNDERLINEOFF "/" DEFFORECOL "/g;s/" BOLDON "/" BOLD "/g;s/" BOLDOFF "/" DEFFORECOL "/g' > %s",windowCols-2,filepath,manFile);
									system(command);
									free(command);

									box=mainApp->CTK_addNewEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,manFile);
									box->CTK_setShowLineNumbers(false);
									box->CTK_setEditable(false);
									mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(filepath));

									setInfoLabel();
									rebuildTabMenu();
									mainApp->CTK_clearScreen();
									mainApp->CTK_updateScreen(mainApp,NULL);
								}
							pclose(fp);
						}					
				}
				break;

			case NAVFIND:
				{
					CTK_cursesUtilsClass	cu;

					foundX=-1;
					foundY=0;
					if(cu.CTK_entryDialog("Find Text",findString.c_str(),"Find ...","",1)==false)
						return;

					findString=cu.dialogReturnData.stringValue;
					lowerneedle=findString;
					transform(lowerneedle.begin(),lowerneedle.end(),lowerneedle.begin(),::tolower );
				}

			case NAVFINDNEXT:
				{
					const std::vector<std::string>	str=box->CTK_getStrings();
					std::string						lowerhaystack;
					int								tfind=foundX;

					for(int j=foundY;j<str.size();j++)
						{
							lowerhaystack=str[j];
							transform(lowerhaystack.begin(),lowerhaystack.end(),lowerhaystack.begin(),::tolower );
							if(foundX==-1)
								tfind=lowerhaystack.find(lowerneedle);
							else
								tfind=lowerhaystack.find(lowerneedle,foundX+1);
							if(tfind!=-1)
								{
									box->CTK_gotoXY(tfind,j);//TODO//highlight
									foundX=tfind;
									foundY=j;
									return;
								}
							foundX=-1;
						}
					foundX=-1;
					foundY=0;
				}
				break;
		}
}

void handleTabMenu(CTK_cursesMenuClass *mc)
{
	switch(mc->menuItemNumber)
		{
			case NEXTTAB:
				mainApp->CTK_nextPage();
				mainApp->CTK_updateScreen(mainApp,NULL);
				break;

			case PREVTAB:
				mainApp->CTK_previousPage();
				mainApp->CTK_updateScreen(mainApp,NULL);
				break;

			default:
				mainApp->CTK_setPage(mc->menuItemNumber-2);
				mainApp->CTK_updateScreen(mainApp,NULL);
				break;
		}
	getTagList((const char*)mainApp->pages[mainApp->pageNumber].userData);
}

void handlefuncMenu(CTK_cursesMenuClass *mc)
{
	CTK_cursesSourceEditBoxClass	*srcbox;

	srcbox=getSrcBox(mainApp->pageNumber);
	srcbox->CTK_gotoLine(functions[mc->menuItemNumber]->line);
}

void handleBMMenu(CTK_cursesMenuClass *mc)
{
	CTK_cursesGadgetClass	*srcbox;
	CTK_cursesGadgetClass	*edbox;
	CTK_cursesEditBoxClass	*box=NULL;
	CTK_cursesSourceEditBoxClass	*sourcebox;

	srcbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1);
	edbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,EDITGADGET,1);
	if(srcbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(srcbox);
	else if(edbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(edbox);

	switch(mc->menuItemNumber)
		{
			case REMOVEMARKS:
				for(unsigned j=0;j<mainApp->pages.size();j++)
					{
						srcbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1);
						edbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,EDITGADGET,1);
						if(srcbox!=NULL)
							box=static_cast<CTK_cursesEditBoxClass*>(srcbox);
						else if(edbox!=NULL)
							box=static_cast<CTK_cursesEditBoxClass*>(edbox);
//						if(mainApp->pages[j].srcEditBoxes.size()!=0)
//							{
//								for(int k=0;k<mainApp->pages[j].srcEditBoxes[0]->CTK_getLineCnt();k++)
//									mainApp->pages[j].srcEditBoxes[0]->CTK_setBookMark(k,false);
//							}
						//if(mainApp->pages[j].editBoxes.size()!=0)
						if(box!=NULL)
							{
								for(int k=0;k<box->CTK_getLineCnt();k++)
									box->CTK_setBookMark(k,false);
							}
					}
				rebuildBMMenu();
				break;

			case TOGGLEMARK:
//				if(mainApp->pages[mainApp->pageNumber].srcEditBoxes.size()!=0)
//					mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]->CTK_toggleBookMark(mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]->CTK_getCursLine());
					box->CTK_toggleBookMark(box->CTK_getCursLine());
//				else
//					if(mainApp->pages[mainApp->pageNumber].editBoxes.size()!=0)
//						mainApp->pages[mainApp->pageNumber].editBoxes[0]->CTK_toggleBookMark(mainApp->pages[mainApp->pageNumber].editBoxes[0]->CTK_getCursLine());
				rebuildBMMenu();
				break;
			default:
				mainApp->CTK_setPage(bms[mc->menuItemNumber-2].pageNum);
		//		if(bms[mc->menuItemNumber-2].isSrc==true)
		//			mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]->CTK_gotoLine(bms[mc->menuItemNumber-2].lineNum);
					box->CTK_gotoLine(bms[mc->menuItemNumber-2].lineNum);
		//		else
		//			mainApp->pages[mainApp->pageNumber].editBoxes[0]->CTK_gotoLine(bms[mc->menuItemNumber-2].lineNum);
				getTagList((const char*)mainApp->pages[mainApp->pageNumber].userData);
				break;
		}
}

void replaceAll(std::string &str,const std::string from,const std::string to)
{
    size_t start_pos=0;
    while((start_pos=str.find(from,start_pos))!=std::string::npos)
    	{
			str.replace(start_pos,from.length(),to);
			start_pos+=to.length();
		}
}

void handleToolsMenu(CTK_cursesMenuClass *mc)
{
	const char				*vars[]= {"%f","%d","%t","%c",NULL};
	int						cnt=0;
	std::string				str;
	char					*path;
	char					*dir;
	FILE					*fp=NULL;
	char					line[1024];
	char					commandpath[PATH_MAX];

	CTK_cursesGadgetClass	*srcbox;
	CTK_cursesGadgetClass	*edbox;
	CTK_cursesEditBoxClass	*box=NULL;
	CTK_cursesSourceEditBoxClass	*sourcebox;

	srcbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,SRCGADGET,1);
	edbox=mainApp->CTK_getGadgetNum(mainApp->pageNumber,EDITGADGET,1);
	if(srcbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(srcbox);
	else if(edbox!=NULL)
		box=static_cast<CTK_cursesEditBoxClass*>(edbox);

	if(mc->menuItemNumber==0)
		{
			mainApp->menuBar->enableShortcuts=false;
			manageTools();
			return;
		}
	str=tools[mc->menuItemNumber-1].command;
	path=strdup((char*)mainApp->pages[mainApp->pageNumber].userData);
	dir=dirname(path);

	while(vars[cnt]!=NULL)
		switch(cnt)
			{
				case 0:
					replaceAll(str,vars[cnt++],(char*)mainApp->pages[mainApp->pageNumber].userData);
					break;
				case 1:
					replaceAll(str,vars[cnt++],dir);
					break;
				case 2:
					replaceAll(str,vars[cnt++],box->CTK_getCurrentWord().c_str());
					setenv("KKEDIT_SELECTION",box->CTK_getCurrentWord().c_str(),1);
					break;
				case 3:
					replaceAll(str,vars[cnt++],box->CTK_getSelection().c_str());
					setenv("KKEDIT_SELECTION",box->CTK_getSelection().c_str(),1);
					break;
			}

	setenv("KKEDIT_CURRENTFILE",(char*)mainApp->pages[mainApp->pageNumber].userData,1);
	setenv("KKEDIT_CURRENTDIR",dir,1);

//fprintf(stderr,"this folder=%s\n",dir);
//fprintf(stderr,"tools folder=%s/tools\n",configFolder);
//fprintf(stderr,"command=%s\n",str.c_str());
//fprintf(stderr,"command to run=(cd %s/tools;%s) 2>&1\n",configFolder,str.c_str());
//fprintf(stderr,"path=%s\n",getenv("PATH"));

	sprintf(commandpath,"(cd \"%s/tools\";%s)",configFolder,str.c_str());
	fp=popen(commandpath,"r");
	free(path);
	if(fp!=NULL)
		{
			str.clear();
			while(fgets(line,1024,fp))
				str.append(line);
			pclose(fp);
		}
//fprintf(stderr,"--->%s<---\n",commandpath);
	if(tools[mc->menuItemNumber-1].flags & TOOL_PASTE_OP)
		{
			//fprintf(stderr,"TOOL_PASTE_OP\n");
			box->CTK_deleteCurrentWord();
			box->CTK_insertText(str.c_str());
			box->isDirty=true;
		}

	if(tools[mc->menuItemNumber-1].flags & TOOL_REPLACE_OP)
		{
			//fprintf(stderr,"TOOL_REPLACE_OP\n");
			box->CTK_updateText(str.c_str());
			box->isDirty=true;
		}

	if(tools[mc->menuItemNumber-1].flags & TOOL_VIEW_OP)
		{
			//fprintf(stderr,"TOOL_VIEW_OP\n");
			mainApp->CTK_addPage();
			box=mainApp->CTK_addNewEditBox(mainApp,1,TOPLINE,windowCols,windowRows,false,str.c_str());
			box->CTK_setShowLineNumbers(false);
			box->CTK_setEditable(false);
			box->isDirty=true;
			mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup("Tool Output"));

			setInfoLabel();
			rebuildTabMenu();
			//mainApp->CTK_clearScreen();
			//mainApp->CTK_updateScreen(mainApp,NULL);
		}
}

void handleHelpMenu(CTK_cursesMenuClass *mc)
{
	CTK_cursesEditBoxClass	*box=NULL;
	switch(mc->menuItemNumber)
		{
			case HELP:
				mainApp->CTK_addPage();				
				box=mainApp->CTK_addNewEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,DATADIR "/help/help");
				box->CTK_setShowLineNumbers(false);
				mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(DATADIR "/help/help"));
				setInfoLabel();
				rebuildTabMenu();
				mainApp->CTK_clearScreen();
				mainApp->CTK_updateScreen(mainApp,NULL);
				break;
			case ABOUT:
			 	{
					mainApp->utils->CTK_aboutDialog("KKEditCli","Cli Text Editor","Copyright ©2019 K.D.Hedger","keithdhedger@gmail.com","http://keithhedger.freeddns.org","K.D.Hedger",DATADIR "/help/LICENSE",80);
				}
				break;
		}
}

bool menuSelectCB(void *inst,void *userdata)
{
	CTK_cursesMenuClass	*mc=static_cast<CTK_cursesMenuClass*>(inst);

//	fprintf(stderr,"Menu item (%i) '%s' of menu (%i) '%s' selected.\n",mc->menuItemNumber,mc->menuNames[mc->menuNumber]->menuItem[mc->menuItemNumber]->menuName,mc->menuNumber,mainApp->menuBar->menuNames[mc->menuNumber]->menuName);

	switch(mc->menuNumber)
		{
			case FILEMENU:
				handleFileMenu(mc);
				break;

			case EDITMENU:
				handleEditMenu(mc);
				break;

			case TABMENU:
				handleTabMenu(mc);
				break;

			case NAVMENU:
				handleNavMenu(mc);
				break;

			case FUNCMENU:
				handlefuncMenu(mc);
				break;

			case BMMENU:
				handleBMMenu(mc);
				break;

			case TOOLSMENU:
				handleToolsMenu(mc);
				break;

			case HELPMENU:
				handleHelpMenu(mc);
				break;
		}
	return(true);
}

void setupMenus(void)
{
	coloursStruct cs;

	cs.hiliteBackCol=BACK_BLACK;
	cs.hiliteForeCol=FORE_GREEN;
	cs.disabledForeCol=FORE_WHITE;

	mainApp->CTK_addNewMenuBar();
	mainApp->menuBar->CTK_addMenuToBar(menuNames[FILEMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[EDITMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[TABMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[NAVMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[FUNCMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[BMMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[TOOLSMENU]);
	mainApp->menuBar->CTK_addMenuToBar(menuNames[HELPMENU]);

	int cnt=0;
	while(fileMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(FILEMENU,fileMenuNames[cnt++]);

	cnt=0;
	while(editMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(EDITMENU,editMenuNames[cnt++]);

	cnt=0;
	while(tabMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(TABMENU,tabMenuNames[cnt++]);

	cnt=0;
	while(navMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(NAVMENU,navMenuNames[cnt++]);

	cnt=0;
	while(bookmarkMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(BMMENU,bookmarkMenuNames[cnt++]);

	mainApp->menuBar->CTK_addMenuItem(TOOLSMENU,MANAGETOOLSLABEL);

	cnt=0;
	while(helpMenuNames[cnt]!=NULL)
		mainApp->menuBar->CTK_addMenuItem(HELPMENU,helpMenuNames[cnt++]);

	cnt=0;
	do
		mainApp->menuBar->CTK_setMenuShortCut(scKeys[cnt].menu,scKeys[cnt].item,scKeys[cnt].key);
	while(scKeys[++cnt].menu!=-1);

	mainApp->menuBar->CTK_setColours(&cs);
	mainApp->menuBar->CTK_setSelectCB(menuSelectCB,NULL);
}