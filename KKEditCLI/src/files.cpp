
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

void openTheFile(const char *path,bool extsrc)
{
	FILE	*fp;
	size_t	len=0;
	ssize_t	read;
	int		linelen=0;
	char	*tline=NULL;
	int		srcchar=0;
	int		destchar=0;
	char	*dline=NULL;
	char	*eline=NULL;
	int		edest;
	int		linenumber=1;

	page->maxLines=0;

	if(extsrc==false)
		oneLiner(true,"source-highlight --infer-lang --style-file=esc.style -f esc --failsafe -i '%s' -o /dev/shm/src 2>/dev/null",path);
	
	fp=fopen("/dev/shm/src", "r");
	if(fp != NULL)
		{
			len=0;
			tline=NULL;
			page->maxLines=0;
			int	msize=4096;
			dline=(char*)malloc(msize);
			eline=(char*)malloc(msize);
			while((read=getline(&tline,&len,fp)) != -1)
				{
					if(msize<read+16)
						{
							dline=(char*)realloc((void*)dline,read+4096);
							eline=(char*)realloc((void*)eline,read+4096);
							msize=read+4096;
						}
					srcchar=0;
					destchar=0;
					edest=0;
					linelen=0;
					page->line[page->maxLines].lineNum=linenumber++;
					page->line[page->maxLines].lineLen=-1;
					while(srcchar<=read)
						{
							if(tline[srcchar]==0x1b)
								{
									while(tline[srcchar]!='m')
										{
											dline[destchar]=tline[srcchar];
											srcchar++;
											destchar++;
										}
									dline[destchar]=tline[srcchar];
								}
							else
								{
									dline[destchar]=tline[srcchar];
									eline[edest]=tline[srcchar];
									if(tline[srcchar]=='\t')
										linelen+=tabs;
									else
										linelen++;
									edest++;
								}
							destchar++;
							srcchar++;

							if(linelen>=maxCols)
								{
									page->line[page->maxLines].isSplitLine=true;
									dline[destchar++]='\n';
									dline[destchar++]=0;
									page->line[page->maxLines].srcLine=strdup(dline);
									destchar=0;
									
									page->line[page->maxLines].lineLen=edest;
									eline[edest++]='\n';
									eline[edest++]=0;
									page->line[page->maxLines++].edLine=strdup(eline);
									page->line[page->maxLines].lineNum=-1;
									edest=0;
									linelen=0;
									memset(eline,0,strlen(eline));
									memset(dline,0,strlen(dline));
								}
						}
					dline[destchar]=0;
					eline[edest]=0;
					page->line[page->maxLines].isSplitLine=false;
					page->line[page->maxLines].lineLen=strlen(eline);
					page->line[page->maxLines].srcLine=strdup(dline);
					page->line[page->maxLines++].edLine=strdup(eline);
					memset(eline,0,strlen(eline));
					memset(dline,0,strlen(dline));
					freeAndNull(&tline);
					len=0;
				}
			free(dline);
			free(eline);
			free(tline);
			fclose(fp);
			buildTabMenu();
		}
}

bool deleteCharFromFile(bool back)
{
	int		fh=0;
	char	*newline;
	bool	retval=false;
	int		newlinelen=0;

	if(back==false)
		{
			if(page->line[page->currentLine].edLine[page->lineXCurs]=='\n')
				{
					page->line[page->currentLine].edLine[page->lineXCurs]=0;
					asprintf(&newline,"%s%s",page->line[page->currentLine].edLine,&page->line[page->currentLine+1].edLine[(int)page->line[page->currentLine].isSplitLine]);
					newlinelen=page->line[page->currentLine+1].lineLen+page->line[page->currentLine].lineLen-1;
					freeAndNull(&page->line[page->currentLine+1].edLine);
					freeAndNull(&page->line[page->currentLine].edLine);
					page->line[page->currentLine].edLine=newline;
					page->line[page->currentLine].lineLen=newlinelen;
					retval=true;
				}
			else
				{
					asprintf(&newline,"%.*s%s",page->lineXCurs,page->line[page->currentLine].edLine,&page->line[page->currentLine].edLine[page->lineXCurs+1]);
					free(page->line[page->currentLine].edLine);
					page->line[page->currentLine].edLine=newline;
					page->line[page->currentLine].lineLen--;
				}
		}
	else
		{
			if(page->lineXCurs==0)
				{
					retval=true;
					currentY--;
					currentX=page->line[page->currentLine-1].lineLen;
					page->lineXCurs=page->line[page->currentLine-1].lineLen;
					asprintf(&newline,"%.*s%s",page->line[page->currentLine-1].lineLen-1,page->line[page->currentLine-1].edLine,page->line[page->currentLine].edLine);
					newlinelen=page->line[page->currentLine-1].lineLen+page->line[page->currentLine].lineLen-1;
					freeAndNull(&page->line[page->currentLine].edLine);
					freeAndNull(&page->line[page->currentLine-1].edLine);
					page->currentLine--;
					page->line[page->currentLine].edLine=newline;
					page->line[page->currentLine].lineLen=newlinelen;
				}
			else
				{
					asprintf(&newline,"%.*s%s",page->lineXCurs-1,page->line[page->currentLine].edLine,&page->line[page->currentLine].edLine[page->lineXCurs]);
					free(page->line[page->currentLine].edLine);
					page->line[page->currentLine].edLine=newline;
					page->line[page->currentLine].lineLen--;
					
				}
		}

	fh=open(tmpEdFilePath,O_WRONLY|O_TRUNC);
	if(fh != -1)
		{
			for(int j=0;j<page->maxLines;j++)
				{
					if(page->line[j].edLine!=NULL)
						write(fh,page->line[j].edLine,page->line[j].lineLen);
				}
			close(fh);
		}
	page->dirty=true;
	return(retval);
}

void writeCharToFile(char c)
{
	int		fh=0;
	char	*newline;

	asprintf(&newline,"%.*s%c%s",page->lineXCurs,page->line[page->currentLine].edLine,c,&page->line[page->currentLine].edLine[page->lineXCurs]);
	free(page->line[page->currentLine].edLine);
	page->line[page->currentLine].edLine=newline;
	page->line[page->currentLine].lineLen++;
	fh=open(tmpEdFilePath,O_WRONLY|O_CREAT|O_TRUNC);
	if(fh != -1)
		{
			for(int j=0;j<page->maxLines;j++)
				write(fh,page->line[j].edLine,page->line[j].lineLen);

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
			for(int j=0;j<page->maxLines;j++)
				write(fh,page->line[j].edLine,page->line[j].lineLen);

			close(fh);
		}

	fh=open(path,O_WRONLY|O_CREAT|O_TRUNC);
	if(fh != -1)
		{
			for(int j=0;j<page->maxLines;j++)
				write(fh,page->line[j].edLine,page->line[j].lineLen);

			close(fh);
		}
	page->dirty=false;
}

void askSaveIfdirty(void)
{
	char	*message=NULL;
	int		status;

	if(page->dirty==true)
		{
			asprintf(&message,"\"%s\"\n\nHas been changed, save?",page->filePath);
			init_dialog(stdin,stdout);
				status=dialog_yesno("",message,8,strlen(page->filePath)+8);
			end_dialog();
			free(message);
			if(status==0)
				{
					dlg_clear();
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
	openTheFile(tmpEdFilePath,false);
	buildTabMenu();
}

void askOpenFile(void)
{
	int	status=-100;

	init_dialog(stdin,stdout);
		status=dialog_fselect("Open File",page->filePath,rows-14,cols-14);
	end_dialog();
	dlg_clear();
	clearScreen();
	if(status==0)
		{
			initEditor();
			setTempEdFile(dialog_vars.input_result);
			page->filePath=strdup(dialog_vars.input_result);
			oneLiner(true,"cp %s %s/%s",page->filePath,tmpEdDir,tmpEdFile);
			openTheFile(tmpEdFilePath,false);
			currentX=minX;
			currentY=minY;
		}
	printLines();
	moveCursToTemp(currentX,currentY);
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
			functionData[cnt]->line=atoi(ptr2)-1;
			asprintf(&functionsMenuNames[cnt]," %s%.4s%s %s",BACKRED,functionData[cnt]->type,BACKBLACK,functionData[cnt]->name);
			cnt++;
		}
	functionsMenuNames[cnt]=NULL;
	functionData[cnt]=NULL;
	pclose(fp);
	free(command);
//	free(retval);
}
