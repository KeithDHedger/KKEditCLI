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
	char					*name;
	char					*data;
};

args						toolVars[]=
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

	if(mainApp->pages[mainApp->pageNumber].srcEditBoxes.size()==0)
		return;

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
												//	{
												//		ERRDATA debugFree(&*(char**)(dataptr[cnt].data));
												//	}
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

	for(int j=0;j<tools.size();j++)
		{
			if(tools[j].menuName!=NULL)
				freeAndNull(&tools[j].menuName);
			if(tools[j].filePath!=NULL)
				freeAndNull(&tools[j].filePath);
			if(tools[j].command!=NULL)
				freeAndNull(&tools[j].command);
		}
	tools.clear();
}

void buildToolsList(void)
{
	toolStruct		tool;
	char			*datafolder;
	LFSTK_findClass	*files=new LFSTK_findClass();

	destroyTools();
	sinkReturn=asprintf(&datafolder,"%s/.KKEdit/tools",getenv("HOME"));

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
				}
		}
}

