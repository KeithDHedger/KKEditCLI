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

pageStruct	*page=NULL;

void initEditor(void)
{
	page=new pageStruct;
	page->topLine=0;
	page->currentLine=0;
	page->lineXCurs=0;
	page->dirty=false;
	for(int j=0;j<MAXLINES;j++)
		page->line[j].edLine=NULL;
	currentX=minX;
	currentY=minY;
}

void closePage(void)
{
	for(int j=0;j<page->maxLines;j++)
		{
			free(page->line[j].srcLine);
			free(page->line[j].edLine);
		}
	free(page->filePath);
	delete page;
	page=NULL;
}

void adjustCursForTabs(bool rite)
{
	int	direction=-1;
	if(rite==true)
		direction=1;
	
	currentX+=(tabs*direction);
	currentX=currentX / tabs;
	currentX=currentX * tabs;
	currentX++;
}

void moveInsert(void)
{
	size_t	tx=0;

	currentX=minX;
	while(tx<(size_t)page->lineXCurs)
		{
			if(tx+1==strlen(page->line[page->currentLine].edLine))
				{
					page->lineXCurs=tx;
					moveCursToTemp(currentX,currentY);
					return;
				}
			if(page->line[page->currentLine].edLine[tx]=='\t')
				adjustCursForTabs(true);
			else
				currentX++;
			tx++;
		}
	page->lineXCurs=tx;
	moveCursToTemp(currentX,currentY);
}

void moveCursLeft(void)
{
	if((page->lineXCurs==0) && (page->currentLine==0))
		return;
	if(page->lineXCurs>0)
		page->lineXCurs--;

	currentX--;
	if(currentX<minX)
		{
			page->currentLine--;
			currentY--;
			currentX=strlen(page->line[page->currentLine].edLine);
			page->lineXCurs=currentX;
		}
	moveInsert();
}

void moveCursRite(void)
{
	if(page->lineXCurs+1==(int)strlen(page->line[page->currentLine].edLine))
		{
			if(page->currentLine+1==page->maxLines)
				return;
			page->lineXCurs=0;
			currentY++;
			page->currentLine++;
		}
	else
		page->lineXCurs++;

	moveInsert();
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

	moveInsert();
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

	moveInsert();
}
