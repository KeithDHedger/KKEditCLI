/*
 *
 * ©K. D. Hedger. Tue  7 May 20:41:19 BST 2019 keithdhedger@gmail.com

 * This file (files.cpp) is part of KKEditCLI.

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

struct varStrings
{
	char				*name;
	char				*data;
};

args					toolVars[]=
{
	//strings
	{"name",TYPESTRING,&menuname},
	{"command",TYPESTRING,&commandarg},
	{"comment",TYPESTRING,&commentarg},
	//ints
	{"interm",TYPEINT,&intermarg},
	{"flags",TYPEINT,&flagsarg},
	{"inpopup",TYPEINT,&inpopup},
	{"alwayspopup",TYPEINT,&alwayspopup},
	{"clearview",TYPEINT,&clearview},
	{"runasroot",TYPEINT,&rootarg},
	{"usebar",TYPEINT,&usebar},
	{"shortcutkey",TYPEINT,&keycode},
	{NULL,0,NULL}
};

struct manageToolsGadgetsStruct
{
	CTK_cursesListBoxClass	*toolList;
	CTK_cursesCheckBoxClass	*checkBoxes[NOMORECHECKS];
	CTK_cursesInputClass	*inputs[NOMOREINPUTS];
	CTK_cursesButtonClass	*buttons[NOMOREBUTTONS];
	CTK_cursesLabelClass	*labels[LASTLABEL];
};

struct manageToolsGadgetsStruct	manageToolsData;

void getTagList(const char *filepath)
{
	char		*command;
	const char	*sortcommand="sort -k 2rb,2rb -k 1b,1b";
	FILE		*fp;
	char		line[2048];
	char		*ptr;
	char		*ptr2;
	funcStruct	*fd;

	mainApp->menuBar->CTK_clearMenu(FUNCMENU);

	if(getSrcBox(mainApp->pageNumber)==NULL)
		{
			mainApp->menuBar->menuNames[FUNCMENU]->menuEnabled=false;
			return;
		}

	for(int j=0;j<functions.size();j++)
		{
			freeAndNull(&functions[j]->name);
			freeAndNull(&functions[j]->type);
			freeAndNull(&functions[j]->menuLabel);
			delete functions[j];
		}
	functions.clear();
	asprintf(&command,"ctags -x \"%s\" 2>/dev/null|%s|sed 's@ \\+@ @g' 2>/dev/null",filepath,sortcommand);
	fp=popen(command, "r");
	
	while(fgets(line,2048,fp))
		{
			fd=new funcStruct;
			fd->pageNumber=mainApp->pageNumber;
			if(strlen(line)>1)
				line[strlen(line)-1]=0;
			ptr=strchr((char*)&line,' ');
			*ptr=0;
			asprintf(&fd->name,"%s",line);
			ptr++;
			ptr2=strchr(ptr,' ');
			*ptr2=0;
			fd->type=strdup(ptr);
			ptr2++;
			ptr=strchr(ptr2,' ');
			*ptr=0;
			fd->line=atoi(ptr2);
			asprintf(&fd->menuLabel," %.4s %s ",fd->type,fd->name);
			functions.push_back(fd);
			mainApp->menuBar->CTK_addMenuItem(FUNCMENU,fd->menuLabel,false);
		}
	pclose(fp);
	free(command);
	if(functions.size()==0)
		mainApp->menuBar->menuNames[FUNCMENU]->menuEnabled=false;
	else
		mainApp->menuBar->menuNames[FUNCMENU]->menuEnabled=true;
}

varStrings* allocVStrings(char *string)
{
	int	namelen=0;
	int	datalen=0;
	int	totalcnt=0;

	varStrings	*vs=(varStrings*)calloc(1,sizeof(varStrings));
	vs->name=NULL;
	vs->data=NULL;

//var name
	sscanf(string,"%*s%n",&namelen);
	vs->name=strndup(string,namelen);
	totalcnt=namelen;

//var type
	sscanf(string,"%*s %n%*[^\n]s",&totalcnt);
	datalen=(int)(long)((long)strchr(string,'\n')-(long)string)-totalcnt;
	vs->data=strndup(&string[totalcnt],datalen);

	return(vs);
}

int loadVarsFromFile(char *filepath,args *dataptr)
{
	FILE		*fd=NULL;
	char		buffer[2048];
	int			cnt;
	int			retval=NOERR;
	varStrings	*vs=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					sinkReturnStr=fgets(buffer,2048,fd);
					vs=allocVStrings(buffer);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((vs->data!=NULL) &&(vs->name!=NULL) &&(strcmp(vs->name,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
											case TYPEINT:
												*(int*)dataptr[cnt].data=atoi(vs->data);
												break;
											case TYPESTRING:
												if(*(char**)(dataptr[cnt].data)!=NULL)
													freeAndNull((char**)dataptr[cnt].data);
												*(char**)(dataptr[cnt].data)=(char*)strdup(vs->data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(vs->data);
												break;
											//case TYPELIST:
											//	*(GSList**)dataptr[cnt].data=g_slist_append(*(GSList**)dataptr[cnt].data,strdup(vs->data));
												break;
										}
								}
							cnt++;
						}
					freeAndNull(&vs->name);
					freeAndNull(&vs->data);
					free(vs);
				}
			fclose(fd);
		}
	else
		{
			retval=NOOPENFILE;
		}

	return(retval);
}

void destroyTools(void)
{
	if(tools.size()==0)
		return;
/*
	char				*menuName=NULL;
	char				*filePath=NULL;
	char				*command=NULL;
	char				*comment;

*/
	for(int j=0;j<tools.size();j++)
		{
			if(tools[j].menuName!=NULL)
				freeAndNull(&tools[j].menuName);
			if(tools[j].filePath!=NULL)
				freeAndNull(&tools[j].filePath);
			if(tools[j].command!=NULL)
				freeAndNull(&tools[j].command);
			if(tools[j].comment!=NULL)
				freeAndNull(&tools[j].comment);
		}
	tools.clear();
}

void buildToolsList(void)
{
	toolStruct		tool;
	char			*datafolder;
	LFSTK_findClass	*files=new LFSTK_findClass();

	mainApp->menuBar->CTK_clearMenu(TOOLSMENU,false);
	mainApp->menuBar->CTK_addMenuItem(TOOLSMENU,MANAGETOOLSLABEL);
	destroyTools();
	sinkReturn=asprintf(&datafolder,"%s/tools",configFolder);

	files->LFSTK_setFollowLinks(true);
	files->LFSTK_setFindType(FILETYPE);
	files->LFSTK_setFullPath(true);
	files->LFSTK_findFiles(datafolder);
	files->LFSTK_setSort(false);
	files->LFSTK_sortByTypeAndName();

	for(int j=0;j<files->data.size();j++)
		{
			intermarg=0;
			flagsarg=0;
			inpopup=0;
			alwayspopup=0;
			rootarg=0;
			clearview=0;
			keycode=0;
			menuname=NULL;
			commandarg=NULL;
			commentarg=NULL;
			usebar=0;

			loadVarsFromFile((char*)files->data[j].path.c_str(),toolVars);

			if((menuname!=NULL) &&(strlen(menuname)>0) &&(commandarg!=NULL))
				{
					tool.menuName=strdup(menuname);
					tool.command=strdup(commandarg);
					tool.flags=flagsarg;
					tool.inTerminal=(bool)intermarg;
					tool.inPopUp=(bool)inpopup;
					tool.alwaysPopup=(bool)alwayspopup;
					tool.filePath=strdup(files->data[j].path.c_str());
					tool.clearView=(bool)clearview;
					tool.runAsRoot=(bool)rootarg;
					tool.keyCode=keycode;
					tool.useBar=(bool)usebar;

					if(commentarg!=NULL)
						tool.comment=strdup(commentarg);
					else
						tool.comment=NULL;
					tool.global=false;
					tools.push_back(tool);
					mainApp->menuBar->CTK_addMenuItem(TOOLSMENU,tool.menuName);
					freeAndNull(&menuname);
					freeAndNull(&commandarg);
					freeAndNull(&commentarg);
				}
		}
	delete files;
	free(datafolder);
}

bool toolListCB(void *inst,void *userdata)
{
	char						*buffer=(char*)alloca(256);
	CTK_cursesListBoxClass		*ls=static_cast<CTK_cursesListBoxClass*>(inst);

//	fprintf(stderr,"List %i List item/num '%s/%i' clicked, user data=%p.\n",(long)userdata,ls->listItems[ls->listItemNumber]->label.c_str(),ls->listItemNumber,ls->listItems[ls->listItemNumber]->userData);

	//mainApp->pages[mainApp->pageNumber].inputs[0]->CTK_setText(ls->listItems[ls->listItemNumber]->label.c_str());
	manageToolsData.inputs[NAMEINPUT]->CTK_setText(ls->listItems[ls->listItemNumber]->label.c_str());

	for(int j=0;j<NOMORECHECKS;j++)
		manageToolsData.checkBoxes[j]->CTK_setValue(false);

	if(ls->listItemNumber==0)
		{
			manageToolsData.checkBoxes[IGNOREOP]->CTK_setValue(true);
			manageToolsData.inputs[COMMANDINPUT]->CTK_setText("");
			return(true);
		}

	manageToolsData.inputs[COMMANDINPUT]->CTK_setText(tools[ls->listItemNumber-1].command);
	if(tools[ls->listItemNumber-1].flags & TOOL_PASTE_OP)
//		mainApp->pages[mainApp->pageNumber].checkBoxes[PASTEOP]->CTK_setValue(true);
		manageToolsData.checkBoxes[PASTEOP]->CTK_setValue(true);
	else if(tools[ls->listItemNumber-1].flags & TOOL_REPLACE_OP)
		//mainApp->pages[mainApp->pageNumber].checkBoxes[REPLACEOP]->CTK_setValue(true);
		manageToolsData.checkBoxes[REPLACEOP]->CTK_setValue(true);
	else if(tools[ls->listItemNumber-1].flags & TOOL_VIEW_OP)
		//mainApp->pages[mainApp->pageNumber].checkBoxes[VIEWOP]->CTK_setValue(true);
		manageToolsData.checkBoxes[VIEWOP]->CTK_setValue(true);
	else
		//mainApp->pages[mainApp->pageNumber].checkBoxes[IGNOREOP]->CTK_setValue(true);
		manageToolsData.checkBoxes[IGNOREOP]->CTK_setValue(true);
	return(true);
}

bool checkCB(void *inst,void *userdata)
{
	CTK_cursesCheckBoxClass	*cb=static_cast<CTK_cursesCheckBoxClass*>(inst);

	for(int j=0;j<NOMORECHECKS;j++)
		manageToolsData.checkBoxes[j]->CTK_setValue(false);
	cb->CTK_setValue(!cb->CTK_getValue());
//	fprintf(stderr,"ud=%i cbsize=%i\n",userdata,mainApp->pages[mainApp->pageNumber].checkBoxes.size());
	return(true);
}

bool toolButtonCB(void *inst,void *userdata)
{
	CTK_cursesButtonClass	*bc=static_cast<CTK_cursesButtonClass*>(inst);
	char					*newfilepath;
	FILE					*fp=NULL;
	int						flags=0;

	//fprintf(stderr,"Button '%s' clicked ud=%i.\n",bc->label,userdata);
	switch((long)userdata)
		{
			case APPLYBUTTON:
					sinkReturn=asprintf(&newfilepath,"%s/tools/%s",configFolder,manageToolsData.inputs[NAMEINPUT]->CTK_getText());
					fp=fopen(newfilepath,"w");
					if(fp!=NULL)
						{
							fprintf(fp,"name	%s\n",manageToolsData.inputs[NAMEINPUT]->CTK_getText());
							fprintf(fp,"command	%s\n",manageToolsData.inputs[COMMANDINPUT]->CTK_getText());
							if(manageToolsData.checkBoxes[IGNOREOP]->CTK_getValue()==true)
								flags=TOOL_IGNORE_OP;
							if(manageToolsData.checkBoxes[PASTEOP]->CTK_getValue()==true)
								flags=TOOL_PASTE_OP;
							if(manageToolsData.checkBoxes[REPLACEOP]->CTK_getValue()==true)
								flags=TOOL_REPLACE_OP;
							if(manageToolsData.checkBoxes[VIEWOP]->CTK_getValue()==true)
								flags=TOOL_VIEW_OP;
							fprintf(fp,"flags	%i\n",flags);
							fclose(fp);

							manageToolsData.toolList->CTK_clearList();
							manageToolsData.toolList->CTK_addListItem("New Tool");
							buildToolsList();
							for(int j=0;j<tools.size();j++)
								manageToolsData.toolList->CTK_addListItem(tools[j].menuName);
						}
						free(newfilepath);
				break;
			case DELETEBUTTON:
				unlink(tools[manageToolsData.toolList->listItemNumber-1].filePath);
				manageToolsData.toolList->CTK_clearList();
				manageToolsData.toolList->CTK_addListItem("New Tool");
				buildToolsList();
				for(int j=0;j<tools.size();j++)
					manageToolsData.toolList->CTK_addListItem(tools[j].menuName);
				break;
			case FINISHEDBUTTON:
				{
					coloursStruct	cs;

					mainApp->CTK_removePage(mainApp->pageNumber);
					fileInfoLabel==NULL;
					mainApp->CTK_clearScreen();
					rebuildTabMenu();
					mainApp->menuBar->CTK_setMenuBarEnable(true);
					mainApp->menuBar->enableShortcuts=true;
					mainApp->menuBar->CTK_setMenuBarVisible(true);
					mainApp->colours.windowBackCol=BACK_BLACK;
					mainApp->CTK_clearScreen();
					mainApp->menuBar->CTK_drawDefaultMenuBar();
				}
				break;
			case EDITBUTTON:
				{
				//TODO//horrible hack!
					char							path[PATH_MAX];
					char							*rp;
					CTK_cursesSourceEditBoxClass	*srcbox;
					int								holdpn=mainApp->pageNumber;
					
					if(strlen(manageToolsData.inputs[COMMANDINPUT]->CTK_getText())==0)
						return(true);

					sprintf(path,"%s/tools/%s",configFolder,manageToolsData.inputs[COMMANDINPUT]->CTK_getText());
					rp=realpath(path,NULL);
					fprintf(stderr,">%s<\n",rp);
					if(rp==NULL)
						{
							oneLiner(true,"touch %s",path);
							oneLiner(true,"chmod +x %s",path);
							rp=realpath(path,NULL);
							if(rp==NULL)
								return(true);
						}

					mainApp->CTK_addPage();
					srcbox=mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,rp);
					srcbox->CTK_setShowLineNumbers(showLineNumbers);
					mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)strdup(rp));
					free(rp);
					setInfoLabel();
					mainApp->menuBar->CTK_setMenuBarEnable(true);
					mainApp->menuBar->enableShortcuts=true;
					mainApp->menuBar->CTK_setMenuBarVisible(true);
					mainApp->colours.windowBackCol=BACK_BLACK;
					mainApp->menuBar->CTK_drawDefaultMenuBar();
					mainApp->CTK_removePage(holdpn);
					mainApp->CTK_clearScreen();
					rebuildTabMenu();
				}
				break;
		}
	mainApp->CTK_updateScreen(mainApp,SCREENUPDATEUNHILITE);
	return(true);
}

void manageTools(void)
{
	int						newpage=0;
	coloursStruct			cs;
	CTK_cursesUtilsClass	cu;
	char					folder[PATH_MAX];

	sprintf(folder,"mkdir -p \"%s\"/{tools,scripts}",configFolder);
	system(folder);
	mainApp->menuBar->CTK_setMenuBarEnable(false);
	mainApp->menuBar->CTK_setMenuBarVisible(false);
	newpage=mainApp->CTK_addPage();
	mainApp->CTK_setPageUserData(newpage,(void*)strdup("Manage Tools"));
	setInfoLabel();
	cs.foreCol=FORE_BLACK;
	cs.backCol=BACK_WHITE;
	cs.use256Colours=false;
	cs.fancyGadgets=true;

	cs.windowBackCol=BACK_WHITE;
	mainApp->colours.windowBackCol=BACK_WHITE;

//tools
	manageToolsData.labels[LABELLIST]=mainApp->CTK_addNewLabel(2,3,TOOLNAMELISTWIDTH,1,"Tool Name");
	manageToolsData.toolList=new CTK_cursesListBoxClass(mainApp);
	manageToolsData.toolList->CTK_newListBox(2,5,TOOLNAMELISTWIDTH,mainApp->maxRows-5);
	manageToolsData.toolList->CTK_addListItem("New Tool");

	for(int j=0;j<tools.size();j++)
		manageToolsData.toolList->CTK_addListItem(tools[j].menuName);
	manageToolsData.toolList->CTK_setSelectCB(toolListCB,(void*)1);
	manageToolsData.toolList->CTK_setColours(cs);
	manageToolsData.toolList->CTK_setSelectDeselects(false);
	mainApp->CTK_addListBox(manageToolsData.toolList);

//output
//ignore
	manageToolsData.checkBoxes[IGNOREOP]=mainApp->CTK_addNewCheckBox(4+TOOLNAMELISTWIDTH,4,20,"Ignore Output   ");
	manageToolsData.checkBoxes[IGNOREOP]->CTK_setColours(cs);
	manageToolsData.checkBoxes[IGNOREOP]->CTK_setSelectDeselects(false);
	manageToolsData.checkBoxes[IGNOREOP]->CTK_setSelectCB(checkCB,(void*)IGNOREOP);
	manageToolsData.checkBoxes[IGNOREOP]->CTK_setSelectKey(' ');
	manageToolsData.checkBoxes[IGNOREOP]->CTK_setValue(true);
//paste
	manageToolsData.checkBoxes[PASTEOP]=mainApp->CTK_addNewCheckBox(4+TOOLNAMELISTWIDTH,6,20,"Paste Output    ");
	manageToolsData.checkBoxes[PASTEOP]->CTK_setColours(cs);
	manageToolsData.checkBoxes[PASTEOP]->CTK_setSelectDeselects(false);
	manageToolsData.checkBoxes[PASTEOP]->CTK_setSelectCB(checkCB,(void*)PASTEOP);
	manageToolsData.checkBoxes[PASTEOP]->CTK_setSelectKey(' ');
//replace
	manageToolsData.checkBoxes[REPLACEOP]=mainApp->CTK_addNewCheckBox(4+TOOLNAMELISTWIDTH,8,20,"Replace Contents");
	manageToolsData.checkBoxes[REPLACEOP]->CTK_setColours(cs);
	manageToolsData.checkBoxes[REPLACEOP]->CTK_setSelectDeselects(false);
	manageToolsData.checkBoxes[REPLACEOP]->CTK_setSelectCB(checkCB,(void*)REPLACEOP);
	manageToolsData.checkBoxes[REPLACEOP]->CTK_setSelectKey(' ');
//view
	manageToolsData.checkBoxes[VIEWOP]=mainApp->CTK_addNewCheckBox(4+TOOLNAMELISTWIDTH,10,20,"View Output     ");
	manageToolsData.checkBoxes[VIEWOP]->CTK_setColours(cs);
	manageToolsData.checkBoxes[VIEWOP]->CTK_setSelectDeselects(false);
	manageToolsData.checkBoxes[VIEWOP]->CTK_setSelectCB(checkCB,(void*)VIEWOP);
	manageToolsData.checkBoxes[VIEWOP]->CTK_setSelectKey(' ');

#define SXOFF 1+TOOLNAMELISTWIDTH-4
#define BOXWID INPUTWIDTH+16

//buttons
//apply
	manageToolsData.buttons[APPLYBUTTON]=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(SXOFF,BOXWID,3,14,0),20,14,1," Apply  ");
	manageToolsData.buttons[APPLYBUTTON]->CTK_setColours(cs);
	manageToolsData.buttons[APPLYBUTTON]->CTK_setSelectCB(toolButtonCB,(void*)APPLYBUTTON);
//delete
	manageToolsData.buttons[DELETEBUTTON]=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(SXOFF,BOXWID,3,14,1),20,14,1," Delete ");
	manageToolsData.buttons[DELETEBUTTON]->CTK_setColours(cs);
	manageToolsData.buttons[DELETEBUTTON]->CTK_setSelectCB(toolButtonCB,(void*)DELETEBUTTON);
//open
	manageToolsData.buttons[EDITBUTTON]=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(SXOFF,BOXWID,3,14,2),20,14,1,"  Edit  ");
	manageToolsData.buttons[EDITBUTTON]->CTK_setColours(cs);
	manageToolsData.buttons[EDITBUTTON]->CTK_setSelectCB(toolButtonCB,(void*)EDITBUTTON);
//fin
	manageToolsData.buttons[FINISHEDBUTTON]=mainApp->CTK_addNewButton(cu.CTK_getGadgetPosX(SXOFF,BOXWID,3,14,3),20,14,1,"Finished");
	manageToolsData.buttons[FINISHEDBUTTON]->CTK_setColours(cs);
	manageToolsData.buttons[FINISHEDBUTTON]->CTK_setSelectCB(toolButtonCB,(void*)FINISHEDBUTTON);
	
//toolname
	manageToolsData.labels[LABELTOOLNAME]=mainApp->CTK_addNewLabel(4+TOOLNAMELISTWIDTH,13,20,1,"Tool Name:");
	manageToolsData.inputs[NAMEINPUT]=mainApp->CTK_addNewInput(16+TOOLNAMELISTWIDTH,13,INPUTWIDTH-1,1,"");
	manageToolsData.inputs[NAMEINPUT]->CTK_setColours(cs);
//command
	manageToolsData.labels[LABELCOMMAND]=mainApp->CTK_addNewLabel(4+TOOLNAMELISTWIDTH,16,20,1,"Command:");
	manageToolsData.inputs[COMMANDINPUT]=mainApp->CTK_addNewInput(16+TOOLNAMELISTWIDTH,16,INPUTWIDTH-1,1,"");
	manageToolsData.inputs[COMMANDINPUT]->CTK_setColours(cs);
//instructions
	const char	*instructions="PLACEHOLDERS:\n\
%c - Currently selected text. Passed to command as $KKEDIT_SELECTION\n\
%t - Word under cursor. Passed to command as $KKEDIT_SELECTION\n\
%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n\
%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n";

	manageToolsData.labels[LABELINFO]=mainApp->CTK_addNewLabel(5+TOOLNAMELISTWIDTH+20,4,mainApp->maxCols-(5+TOOLNAMELISTWIDTH+20),6,instructions);

	cs.foreCol=FORE_BLACK;
	cs.backCol=BACK_WHITE;
	cs.fancyGadgets=false;
	for(int j=0;j<LASTLABEL;j++)
		manageToolsData.labels[j]->CTK_setColours(cs);

	manageToolsData.labels[LABELLIST]->CTK_setJustify(CENTREJUSTIFY);
	mainApp->CTK_setDefaultGadget(manageToolsData.toolList);

	rebuildTabMenu();
	mainApp->CTK_clearScreen();
	mainApp->CTK_updateScreen(mainApp,NULL);
}




