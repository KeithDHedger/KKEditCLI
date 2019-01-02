/*
 *
 * ©K. D. Hedger. Sun 20 May 14:02:15 BST 2018 keithdhedger@gmail.com

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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "globals.h"

void setTempEdFile(const char *path)
{
	char	*basec=NULL;

	freeAndNull(&tmpEdFile);
	freeAndNull(&tmpEdFilePath);
	basec=strdup(path);
	asprintf(&tmpEdFile,"%i.%s",page->pageNum,basename(basec));
	asprintf(&tmpEdFilePath,"%s/%s",tmpEdDir,tmpEdFile);
	free(basec);
}

#include <iostream>
#include "srchilite/sourcehighlight.h"
#include "srchilite/langmap.h"
#include <srchilite/languageinfer.h>

using namespace std;
using namespace srchilite;
#define SHDATADIR "/usr/share/source-highlight/"

//oneLiner(true,"source-highlight --infer-lang --style-file=esc.style -f esc --failsafe -i '%s' -o /dev/shm/src 2>/dev/null",path);

void makeSrc(const char *path)
{
	LanguageInfer inf;
	SourceHighlight sourceHighlight("esc.outlang");
	LangMap langMap(SHDATADIR,"lang.map");
	string inputLang="nohilite.lang";

	sourceHighlight.setDataDir(SHDATADIR);

	string lang=langMap.getMappedFileNameFromFileName(path);

	if(lang != "")
		inputLang=lang;
	else
		{
			lang=inf.infer(path);
			if(lang != "")
				{
					langMap.open();
					inputLang=langMap.getFileName(lang);
				}
		}

	if(inputLang=="")
		inputLang="nohilite.lang";
	sourceHighlight.setStyleFile("esc.style");
	sourceHighlight.highlight(path,srcPath,inputLang);
}

void openTheFile(const char *path,bool extsrc)
{
	std::string	editline;
	std::string	printline;
	FILE		*fp;
	char		*linebuffer=NULL;
	size_t		len=0;
	ssize_t		read;
	int			srcchar=0;
	int			linelen=0;
	int			linenum=1;
	bool		addlinenumber;

	if(extsrc==true)
		makeSrc(path);
	else
		oneLiner(true,"cp '%s' %s 2>/dev/null",path,srcPath);  

	if(access(page->filePath,W_OK)==0)
		page->writable=true;
	else
		page->writable=false;

	page->editLineArray.clear();
	page->printLineArray.clear();
	page->lineNumber.clear();

	fp=fopen(srcPath,"r");
	if(fp != NULL)
		{
			while((read=getline(&linebuffer,&len,fp)) != -1)
				{
					editline="";
					printline="";
					srcchar=0;
					linelen=0;
					page->lineNumber.push_back(linenum);
					addlinenumber=false;
					while(srcchar<read)
						{
							if(linebuffer[srcchar]==0x1b)
								{
									while(linebuffer[srcchar]!='m')
										{
											printline.append(1,linebuffer[srcchar]);
											srcchar++;
										}
									printline.append(1,linebuffer[srcchar]);
									srcchar++;
								}
							else
								{
									editline.append(1,linebuffer[srcchar]);
									printline.append(1,linebuffer[srcchar]);
									if(linebuffer[srcchar]=='\t')
										linelen+=tabs;
									else
										linelen++;
									srcchar++;
								}

							if(linelen>=maxCols)
								{
									page->editLineArray.push_back(editline);
									page->printLineArray.push_back(printline);
									if(addlinenumber==false)
										addlinenumber=true;
									else
										page->lineNumber.push_back(-1);
									editline="";
									printline="";
									linelen=0;
								}
						}
					page->editLineArray.push_back(editline);
					page->printLineArray.push_back(printline);
					if(addlinenumber==false)
						addlinenumber=true;
					else
						page->lineNumber.push_back(-1);

					free(linebuffer);
					linebuffer=NULL;
					linenum++;
					len=0;
				}
			free(linebuffer);
			fclose(fp);
			buildTabMenu();
		}
	if(page->editLineArray.size()==0)
		{
			page->editLineArray.push_back("\n");
			page->printLineArray.push_back("\n");
			page->lineNumber.push_back(1);
			page->lineXCurs=0;
		}
	page->maxLines=page->editLineArray.size();
}

bool deleteCharFromFile(bool back)
{
	int		fh=0;
	char	*newline;

	if(back==false)
		{
			if(page->editLineArray.at(page->currentLine).at(page->lineXCurs)=='\n')
				{
					if(page->currentLine+1==(int)page->editLineArray.size())
						return(false);
					page->editLineArray.at(page->currentLine).erase(page->editLineArray.at(page->currentLine).end()-1);
					page->editLineArray.at(page->currentLine).append(page->editLineArray.at(page->currentLine+1));
					page->editLineArray.erase(page->editLineArray.begin()+page->currentLine+1);
					page->maxLines--;
				}
			else
				{
					asprintf(&newline,"%s%s",page->editLineArray.at(page->currentLine).substr(0,page->lineXCurs).c_str(),page->editLineArray.at(page->currentLine).substr(page->lineXCurs+1,page->editLineArray.at(page->currentLine).length()).c_str());
					page->editLineArray.at(page->currentLine)=newline;
					free(newline);
				}
		}
	else
		{
			if(page->lineXCurs==0)
				{
					if(page->currentLine==0)
						return(false);
					page->lineXCurs=page->editLineArray.at(page->currentLine-1).length();
					page->editLineArray.at(page->currentLine-1).erase(page->editLineArray.at(page->currentLine-1).end()-1);
					page->editLineArray.at(page->currentLine-1)+=page->editLineArray.at(page->currentLine);
					page->editLineArray.erase(page->editLineArray.begin()+page->currentLine);
					page->currentLine--;
					currentY--;
				}
			else
				{
					asprintf(&newline,"%s%s",page->editLineArray.at(page->currentLine).substr(0,page->lineXCurs-1).c_str(),page->editLineArray.at(page->currentLine).substr(page->lineXCurs,page->editLineArray.at(page->currentLine).length()-1).c_str());
					page->editLineArray.at(page->currentLine)=newline;
					free(newline);
				}
		}

	fh=open(tmpEdFilePath,O_WRONLY|O_CREAT|O_TRUNC);
	if(fh != -1)
		{
			for(unsigned j=0;j<page->editLineArray.size();j++)
				write(fh,page->editLineArray.at(j).c_str(),page->editLineArray.at(j).size());
			close(fh);
		}
	page->dirty=true;
	return(true);
}

void writeCharToFile(char c)
{
	int		fh=0;

	page->editLineArray.at(page->currentLine).insert(page->lineXCurs,std::string(1,c));

	fh=open(tmpEdFilePath,O_WRONLY|O_CREAT|O_TRUNC);
	if(fh != -1)
		{
			for(unsigned j=0;j<page->editLineArray.size();j++)
				write(fh,page->editLineArray.at(j).c_str(),page->editLineArray.at(j).size());
			close(fh);
		}
	page->dirty=true;
}

void writeFile(void)
{
	int		fh=0;

	fh=open(tmpEdFilePath,O_WRONLY|O_CREAT|O_TRUNC);
	if(fh != -1)
		{
			for(unsigned j=0;j<page->editLineArray.size();j++)
				write(fh,page->editLineArray.at(j).c_str(),page->editLineArray.at(j).size());
			close(fh);
		}
	page->dirty=true;
}


void saveFile(const char *path)
{
	int		fh=0;
	fh=open(tmpEdFilePath,O_WRONLY|O_CREAT|O_TRUNC);
	if(fh != -1)
		{
			for(unsigned j=0;j<page->editLineArray.size();j++)
				write(fh,page->editLineArray.at(j).c_str(),page->editLineArray.at(j).length());
			close(fh);
		}

	fh=open(path,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(fh != -1)
		{
			for(unsigned j=0;j<page->editLineArray.size();j++)
				write(fh,page->editLineArray.at(j).c_str(),page->editLineArray.at(j).length());
			close(fh);
		}
	page->dirty=false;
}

char *getFile(const char *msg)
{
	char		*filename;
	CDKSCREEN	*cdkscreen;

	cdkscreen=initCDKScreen(NULL);
	initCDKColor ();

	filename=selectFile(cdkscreen,msg);
	destroyCDKScreen(cdkscreen);
	endCDK ();

	return(filename);
}

int askSaveDialog(const char **msg)
{
	CDKSCREEN	*cdkscreen=0;
	int			ret=1;
	const char	*buttons[]={" Ok "," Cancel "};

	cdkscreen=initCDKScreen(NULL);
  	initCDKColor ();

	ret=popupDialog(cdkscreen,(char**)msg,2,(char**)buttons,2);
	destroyCDKScreen (cdkscreen);
	endCDK ();
	return(ret);
}

void askSaveIfdirty(void)
{
	int		status;
	char	*body[2];
	if(page->dirty==true)
		{
			asprintf(&body[0],"<C>\"%s\"",page->filePath);
			asprintf(&body[1],"<C></2/B>Has been changed, save?");
			fflush(NULL);
			status=askSaveDialog((const char**)&body);
			free(body[0]);
			free(body[1]);
			if(status==0)
				{
					saveFile(page->filePath);
					refreshScreen();
					moveCursToTemp(minX,currentY);
					fflush(NULL);
				}
		}
}

void makeNewFile(void)
{
	initEditor();
	asprintf(&page->filePath,"/tmp/Untitled-%i",newFileNum++);
	setTempEdFile(page->filePath);
	oneLiner(true,"echo \"New File\" > %s",page->filePath);
	oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
	openTheFile(tmpEdFilePath,hilite);
	buildTabMenu();
}

void askOpenFile(void)
{
	char	*retstr=NULL;

	retstr=getFile("Select File To Open ...");
	if(retstr!=NULL)
		{
			if(access(retstr,F_OK)!=0)
				oneLiner(true,"touch \"%s\"",retstr);
			initEditor();
			setTempEdFile(retstr);
			page->filePath=retstr;
			oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
			openTheFile(tmpEdFilePath,hilite);
			currentX=minX;
			currentY=minY;
		}
	printLines();
	moveCursToTemp(currentX,currentY);
}

void askSaveFile(void)
{
	char	*retstr=NULL;

	retstr=getFile("Select Save File ...");
	if(retstr!=NULL)
		{
			saveFile(retstr);
			fflush(NULL);
			unlink(tmpEdFilePath);
			setTempEdFile(retstr);
			page->filePath=retstr;
			oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
			openTheFile(tmpEdFilePath,hilite);
			printLines();
		}
}

void clearTagList(void)
{
	int	cnt=0;

	if(functionData!=NULL)
		{
			while(functionData[cnt]!=NULL)
				{
					free(functionData[cnt]->name);
					free(functionData[cnt]->type);
					delete functionData[cnt++];
				}
			free(functionData);
		}

	cnt=0;
	if(functionsMenuNames!=NULL)
		{
			while(functionsMenuNames[cnt]!=NULL)
				{
					free(functionsMenuNames[cnt++]);
				}
			free(functionsMenuNames);
		}

	functionData=NULL;
	functionsMenuNames=NULL;
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
	functionsMenuNames=(char**)calloc(sizeof(char*),funcs);
	functionData=(funcStruct**)calloc(sizeof(funcStruct*),funcs);
	free(command);
	free(retval);	
	asprintf(&command,"ctags -x \"%s\"|%s|sed 's@ \\+@ @g'",tmpEdFilePath,sortcommand);
	fp=popen(command, "r");
	while(fgets(line,2048,fp))
		{
			functionData[cnt]=new funcStruct;
			if(strlen(line)>1)
				line[strlen(line)-1]=0;
			ptr=strchr((char*)&line,' ');
			*ptr=0;
			asprintf(&functionData[cnt]->name,"%s",line);
			ptr++;
			ptr2=strchr(ptr,' ');
			*ptr2=0;
			functionData[cnt]->type=strdup(ptr);
			ptr2++;
			ptr=strchr(ptr2,' ');
			*ptr=0;
			functionData[cnt]->line=atoi(ptr2);
			asprintf(&functionsMenuNames[cnt]," %s%.4s%s %s",BACKRED,functionData[cnt]->type,BACKBLACK,functionData[cnt]->name);
			cnt++;
		}
	functionsMenuNames[cnt]=NULL;
	functionData[cnt]=NULL;
	pclose(fp);
	free(command);
}

void openFileList(const char *files)
{
	DEBUGFUNC(">>>path list=%s<<<",files);
}

