/*
 *
 * ©K. D. Hedger. Sat 26 May 12:07:24 BST 2018 kdhedger68713@gmail.com

 * This file (editor.cpp) is part of KKEditCLI.

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

int			pageNumber=0;
int			currentPage=0;
int			totalPages=0;
char		*wordBuf[1024]={0,};
const char	*wordBufPtr=(const char*)wordBuf;

void initEditor(void)
{
	page=new pageStruct;
	page->filePath=NULL;
	page->topLine=0;
	page->currentLine=0;
	page->lineXCurs=0;
	page->dirty=false;
	for(int j=0;j<MAXLINES;j++)
		{
			page->line[j].edLine=NULL;
			page->line[j].srcLine=NULL;
		}
	clearTagList();
	currentX=minX;
	currentY=minY;
	page->saveX=currentX;
	page->saveY=currentY;

	if(pages==NULL)
		{
			pages=(pageStruct**)calloc(4,sizeof(pageStruct*));
			maxPages=4;
			currentPage=0;
			totalPages=1;
		}
	else
		{
			currentPage=-1;
			for(int j=0;j<maxPages;j++)
				{
					if(pages[j]==NULL)
						{
							currentPage=j;
							break;
						}
				}
			if(currentPage==-1)
				{
					maxPages+=4;
					pages=(pageStruct**)realloc((void*)pages,maxPages*sizeof(pageStruct*));
					for(int j=maxPages-4;j<maxPages;j++)
						pages[j]=NULL;
					currentPage=totalPages;
					totalPages++;
				}
			else
				{
					totalPages++;
				}
		}
	page->pageNum=currentPage;
	pages[currentPage]=page;
}

void closePage(void)
{
	int	freenum=page->pageNum;

	if(page!=NULL)
		askSaveIfdirty();

	for(int j=0;j<page->maxLines;j++)
		{
			free(page->line[j].srcLine);
			free(page->line[j].edLine);
		}

	free(page->filePath);
	delete page;
	pages[freenum]=NULL;
	unlink(tmpEdFilePath);
	clearTagList();
}

void adjCursor(void)
{
	unsigned char	buf[16];
	char			*ptr=NULL;

	HIDECURS;
	moveCursToTemp(minX,currentY);

	if(page->lineXCurs>strlen(page->line[page->currentLine].edLine)-1)
		page->lineXCurs=strlen(page->line[page->currentLine].edLine)-1;
	if(page->lineXCurs<0)
		page->lineXCurs=0;

	for(int j=0;j<page->lineXCurs;j++)
		{
			switch(page->line[page->currentLine].edLine[j])
				{
					case '\t':
						fprintf(stdout,"\t");
						break;
					default:
						printf(CURSFORWARD);
				}
		}

	fprintf(stdout,"\e[6n");
	memset(buf,0,16);
	fflush(NULL);
	usleep(100);
	read(STDIN_FILENO,&buf,15);
	ptr=strstr((char*)&buf,";");
	if(ptr!=NULL)
		{
			ptr++;
			currentX=atoi(ptr);
		}

	moveCursToTemp(currentX,currentY);
	SHOWCURS;
	fflush(NULL);
}

void moveCursLeft(void)
{
	if((page->lineXCurs==0) && (page->currentLine==0))
		return;
	if(page->lineXCurs>0)
		page->lineXCurs--;
	else
		{
			page->currentLine--;
			currentY--;
			page->lineXCurs=strlen(page->line[page->currentLine].edLine)-1;
		}

	adjCursor();
}

void moveCursRite(void)
{
	if(page->line[page->currentLine].edLine[page->lineXCurs]=='\n')
		{
			if(page->currentLine+1==page->maxLines)
				return;
			page->currentLine++;
			currentY++;
			if(currentY>rows-1)
				{
					currentY=rows-1;
					page->topLine++;
					printLines();
				}
			page->lineXCurs=0;
			adjCursor();
			return;
		}

	page->lineXCurs++;
	adjCursor();
}

void moveCursUp(void)
{
	if(page->currentLine==0)
		return;

	currentY--;
	page->currentLine--;
	if(currentY<minY)
		{
			currentY=minY;
			page->topLine--;
			printLines();	
		}

	if(page->topLine<0)
		page->topLine=0;
	if(page->currentLine<0)
		page->currentLine=0;
}

void moveCursDown(void)
{
	if(page->currentLine+1==page->maxLines)
		return;
	page->currentLine++;
	currentY++;
	if(currentY>rows-1)
		{
			currentY=rows-1;
			page->topLine++;
			printLines();
		}
}

int findLineByLineNumber(int linenumber)
{
	int cnt=0;
	while(page->line[cnt].edLine!=NULL)
		{
			if(page->line[cnt].lineNum==linenumber)
				return(cnt);
			cnt++;
		}
	return(0);
}

void findWordUnderCursor(void)
{
	int	startx;
	int	endx;

	wordBuf[0]=0;
	if(!isalnum(page->line[page->currentLine].edLine[page->lineXCurs]))
		return;

	startx=page->lineXCurs;
	endx=startx;

	while(((isalnum(page->line[page->currentLine].edLine[startx])) || (page->line[page->currentLine].edLine[startx]=='_')) && (startx>=0))
		startx--;

	while(((isalnum(page->line[page->currentLine].edLine[endx])) || (page->line[page->currentLine].edLine[endx]=='_')) && (endx<=page->line[page->currentLine].lineLen))
		endx++;

	snprintf((char*)&wordBuf,endx-startx,"%s",&page->line[page->currentLine].edLine[startx+1]);
}












