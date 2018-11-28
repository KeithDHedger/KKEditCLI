/*
 *
 * ©K. D. Hedger. Sat 26 May 12:07:24 BST 2018 keithdhedger@gmail.com

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

int				currentPage=0;
int				totalPages=0;
char			*wordBuf[1024]={0,};
const char		*wordBufPtr=(const char*)wordBuf;
bookmarkStruct	bookmarks[MAXBOOKMARKS];

void initEditor(void)
{
	if(page!=NULL)
		{
			page->saveX=currentX;
			page->saveY=currentY;
			page->saveCurrentLine=page->currentLine;
		}

	page=new pageStruct;
	page->filePath=NULL;
	page->topLine=0;
	page->currentLine=0;
	page->lineXCurs=0;
	page->dirty=false;

	page->editLineArray.clear();
	page->printLineArray.clear();
	page->lineNumber.clear();

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

	for(int j=0;j<MAXBOOKMARKS;j++)
		{
			if(bookmarks[j].pageNum==freenum)
				{
					bookmarks[j].line=bookmarks[j+1].line;
					bookmarks[j].pageNum=bookmarks[j+1].pageNum;
					bookmarks[j+1].line=-1;
					bookmarks[j+1].pageNum=-1;
				}
		}
	buildBMMenu();
	if(page!=NULL)
		askSaveIfdirty();

	page->editLineArray.clear();
	page->printLineArray.clear();
	page->lineNumber.clear();

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
	int				ccnt=0;

	HIDECURS;
	moveCursToTemp(minX,currentY);

	if(page->lineXCurs>page->editLineArray.at(page->currentLine).length()-1)
		page->lineXCurs=page->editLineArray.at(page->currentLine).length()-1;
	if(page->lineXCurs<0)
		page->lineXCurs=0;

	for(int j=0;j<page->lineXCurs;j++)
		{
			switch(page->editLineArray.at(page->currentLine).at(j))
				{
					case '\t':
						printf(TABCHAR);
						break;
					default:
						printf(CURSFORWARD);
				}
		}

	printf(GETCURSPOS);
	memset(buf,0,16);
	fflush(NULL);
	usleep(500);
	while(true)
		{
			buf[ccnt]=fgetc(stdin);
			if(buf[ccnt++]=='R')
				break;
		}

	ptr=strstr((char*)&buf,";");
	if(ptr!=NULL)
		{
			ptr++;
			currentX=atoi(ptr);
		}

	moveCursToTemp(currentX,currentY);
	SHOWCURS;
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
			page->lineXCurs=page->editLineArray.at(page->currentLine).size()-1;
		}

	adjCursor();
}

void moveCursRite(void)
{
	if(page->editLineArray.at(page->currentLine).at(page->lineXCurs)=='\n')
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
	
	if(linenumber<=0)
		return(0);
	for(int j=0;j<page->lineNumber.size();j++)
		{
			if(page->lineNumber.at(cnt)==linenumber)
				return(cnt);
			cnt++;
		}
	return(cnt-1);
}

void findWordUnderCursor(void)
{
	int	startx;
	int	endx;

	if(!isalnum(page->editLineArray.at(page->currentLine)[page->lineXCurs]))
		{
			//sprintf((char*)&wordBuf,"");
			wordBuf[0]=0;
			return;
		}

	startx=page->lineXCurs;
	endx=page->lineXCurs;
	while(((isalnum(page->editLineArray.at(page->currentLine)[startx])) || (page->editLineArray.at(page->currentLine)[startx]=='_')) && (startx>0))
		startx--;
	while(((isalnum(page->editLineArray.at(page->currentLine)[endx])) || (page->editLineArray.at(page->currentLine)[endx]=='_')) && (endx<page->editLineArray.at(page->currentLine).length()))
		endx++;

	if(startx!=0)
		startx++;
	sprintf((char*)&wordBuf,"%s",page->editLineArray.at(page->currentLine).substr(startx,endx-startx).c_str());
}

void switchPage(int newpagenum,int gotoline)
{
	int	realline;

	if(newpagenum>-1)
		{
			if(pages[newpagenum]==NULL)
				return;
			page->saveX=currentX;
			page->saveY=currentY;
			page->saveCurrentLine=page->currentLine;

			page=pages[newpagenum];
			currentPage=page->pageNum;
			setTempEdFile(page->filePath);
			currentX=page->saveX;
			currentY=page->saveY;
			page->currentLine=page->saveCurrentLine;
		}

	if(gotoline>0)
		{
			realline=findLineByLineNumber(gotoline);
			page->currentLine=realline;
			if(page->currentLine>page->maxLines)
				page->currentLine=page->maxLines-1;
			if(page->currentLine<1)
				page->currentLine=0;
			page->lineXCurs=0;
		}

	page->topLine=page->currentLine;
	currentY=minY;
	clearScreen();
	printLines();
	clearTagList();
	adjCursor();
}

