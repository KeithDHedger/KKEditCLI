/*
 *
 * ©K. D. Hedger. Sun 20 May 14:02:15 BST 2018 kdhedger68713@gmail.com

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
#include <dialog.h>

#include "globals.h"

int tf(int argc, char **argv)
       {
            int status;
            init_dialog(stdin, stdout);
            status = dialog_yesno(
                      "Hello, in dialog-format",
                      "Hello World!",
                      0, 100);
 
           status = dialog_fselect(
                      "Hello, in dialog_fselect",
                     argv[1],
                      0, 0);
dlg_clear();
 
// int ord=0;
// ord=dlg_box_x_ordinate(3);
 dialog_vars.begin_set=1;
 dialog_vars.begin_x=2;
 dialog_vars.begin_y=2;

           status = dialog_inputbox(
                      "Hello, in dialog_inputbox",
                    "prompt",
                    
                      0, 0,
                      "data",
                      false);
			fflush(NULL);
            end_dialog();
            
            printf("status=%i\n",status);
            printf("len=%i inp=%s\n",dialog_vars.input_length,dialog_vars.input_result);
            char *filepath=NULL;
            filepath=realpath(dialog_vars.input_result,filepath);
            printf("fullpath=%s\n",filepath);
            free(filepath);
            return status;
       }

void openTheFile(const char *path)
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

	tline=oneLiner(false,"source-highlight --infer-lang --style-file=esc.style -f esc --failsafe -i '%s' -o /dev/shm/src",path);
	free(tline);
	
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
										linelen+=4;
									else
										linelen++;
									edest++;
								}
							destchar++;
							srcchar++;

							if(linelen>=149)
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
					free(tline);
					tline=NULL;
					len=0;
				}
			free(dline);
			free(eline);
			free(tline);
			fclose(fp);
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
					free(page->line[page->currentLine+1].edLine);
					free(page->line[page->currentLine].edLine);
					page->line[page->currentLine+1].edLine=NULL;
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
					free(page->line[page->currentLine].edLine);
					free(page->line[page->currentLine-1].edLine);
					page->line[page->currentLine].edLine=NULL;
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

	fh=open(tmpedfile,O_WRONLY|O_TRUNC);
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
	fh=open(tmpedfile,O_WRONLY|O_CREAT|O_TRUNC);
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
	fh=open(tmpedfile,O_WRONLY|O_CREAT|O_TRUNC);
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
			DEBUGFUNC("save dirty","");
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
