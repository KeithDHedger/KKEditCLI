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
return;
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

void adjCursor(void)
{
	unsigned char	buf[16];
	char			*ptr=NULL;

	currentX=minX;
	HIDECURS;
	moveCursToTemp(currentX,currentY);
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
						//fprintf(stdout,"\e[6n");
						//memset(buf,0,16);
						//fflush(NULL);
						//fflush(STDIN_FILENO);
						//usleep(1000);
						//read(STDIN_FILENO,&buf,15);
						////ptr=strstr((char*)&buf,";");
						//if(ptr!=NULL)
						//	{
						//		ptr++;
						//		currentX=atoi(ptr);
						//	}
						break;
					default:
						printf(CURSFORWARD);
						//currentX++;
				}
//			moveCursToTemp(currentX,currentY);
		}

	fprintf(stdout,"\e[6n");
	memset(buf,0,16);
	fflush(NULL);
	fflush(STDIN_FILENO);
	usleep(1000);
	read(STDIN_FILENO,&buf,15);
	ptr=strstr((char*)&buf,";");
	if(ptr!=NULL)
		{
			ptr++;
			currentX=atoi(ptr);
		}

	moveCursToTemp(currentX,currentY);
	SHOWCURS;
	fflush(STDIN_FILENO);
}




void adjCursorxxxx(void)
{
	unsigned char	buf[16];
	char			*ptr=NULL;

	currentX=minX;
	HIDECURS;
	moveCursToTemp(currentX,currentY);
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
						fprintf(stdout,"\e[6n");
						memset(buf,0,16);
						fflush(NULL);
						fflush(STDIN_FILENO);
						usleep(1000);
						read(STDIN_FILENO,&buf,15);
						ptr=strstr((char*)&buf,";");
						if(ptr!=NULL)
							{
								ptr++;
								currentX=atoi(ptr);
							}
						break;
					default:
						currentX++;
				}
			moveCursToTemp(currentX,currentY);
		}
	SHOWCURS;
	fflush(STDIN_FILENO);
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

//void moveCursLeftHOLD(void)
//{
//	if((page->lineXCurs==0) && (page->currentLine==0))
//		return;
//	if(page->lineXCurs>0)
//		page->lineXCurs--;
//
//	currentX--;
//	if(currentX<minX)
//		{
//			page->currentLine--;
//			currentY--;
//			currentX=strlen(page->line[page->currentLine].edLine);
//			page->lineXCurs=currentX;
//		}
//	moveInsert();
//}
//
//void moveCursRite(void);
//void moveCursLeftzzzz(void)
//{
//	unsigned char	buf[16];
//	char			*ptr=NULL;
//
////	if((page->lineXCurs==0) && (page->currentLine==0))
////		return;
////
////	if(page->lineXCurs>0)
////		page->lineXCurs--;
////	else
////		{
////			currentY--;
////			page->currentLine--;
////			moveCursToTemp(currentX,currentY);
////			return;
////		}
//
//	page->lineXCurs--;
//	adjCursor();
//	return;
//fprintf(stderr,"%c",page->line[page->currentLine].edLine[page->lineXCurs]);
//	switch(page->line[page->currentLine].edLine[page->lineXCurs])
//		{
//			case '\t':
//				fprintf(stdout,"\e[Z\e[Z\t");
//				fflush(NULL);
//				fprintf(stdout,"\e[6n");
//				memset(buf,0,16);
//				fflush(NULL);
//				read(STDIN_FILENO,&buf,15);
//				ptr=strstr((char*)&buf,";");
//				ptr++;
//				currentX=atoi(ptr);
//				//currentX++;
//				//moveCursRite();
//				
//				break;
////			case '\n':
////				if(page->currentLine+1==page->maxLines)
////					return;
////				page->lineXCurs=0;
////				currentY++;
////				currentX=minX;
////				page->currentLine++;
////				break;
//			default:
//				//page->lineXCurs--;
//				currentX--;
//		}
//
//	moveCursToTemp(currentX,currentY);
//	fflush(NULL);
//}

void moveCursRite(void)
{
	unsigned char	buf[16];
	char			*ptr=NULL;

	switch(page->line[page->currentLine].edLine[page->lineXCurs])
		{
			case '\n':
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
//				currentX=minX;
		//		page->currentLine++;
				adjCursor();
				return;
				break;
//			default:
//				page->lineXCurs++;
//				currentX++;
		}
//	moveCursToTemp(currentX,currentY);
page->lineXCurs++;
adjCursor();
}

void moveCursRiteqqqqq(void)
{
	unsigned char	buf[16];
	char			*ptr=NULL;

	switch(page->line[page->currentLine].edLine[page->lineXCurs])
		{
			case '\t':
				fprintf(stdout,"\t");
				fprintf(stdout,"\e[6n");
				memset(buf,0,16);
				fflush(NULL);
				read(STDIN_FILENO,&buf,15);
				ptr=strstr((char*)&buf,";");
				ptr++;
				currentX=atoi(ptr);
				page->lineXCurs++;
				break;
			case '\n':
				if(page->currentLine+1==page->maxLines)
					return;
				page->lineXCurs=0;
				currentY++;
				currentX=minX;
				page->currentLine++;
				break;
			default:
				page->lineXCurs++;
				currentX++;
		}
	moveCursToTemp(currentX,currentY);
}

void moveCursRiteZZZZ(void)
{
	unsigned char	buf[16];
	char			*ptr=NULL;

	if(page->line[page->currentLine].edLine[page->lineXCurs]=='\t')
		{
			fprintf(stdout,"\t");
			fprintf(stdout,"\e[6n");
			memset(buf,0,16);
			fflush(NULL);
			read(STDIN_FILENO,&buf,15);
			ptr=strstr((char*)&buf,";");
			ptr++;
			currentX=atoi(ptr);
			page->lineXCurs++;
			moveCursToTemp(currentX,currentY);
		}
	else
		{
			//if(page->lineXCurs+1==(int)strlen(page->line[page->currentLine].edLine))
			if(page->line[page->currentLine].edLine[page->lineXCurs]=='\n')
				{
					if(page->currentLine+1==page->maxLines)
						return;
					page->lineXCurs=0;
					currentY++;
					currentX=minX;
					page->currentLine++;
				}
			else
				{
					page->lineXCurs++;
					currentX++;
					moveCursToTemp(currentX,currentY);
				}
//			moveInsert();
			moveCursToTemp(currentX,currentY);
		}
}

void moveCursRiteqq(void)
{
fprintf(stdout,"\t");
fflush(NULL);
fprintf(stdout,"\e[6n");
	unsigned char	buf[16];
			memset(buf,0,16);
//sleep(2);
fflush(NULL);
			read(STDIN_FILENO,&buf,15);

char *ptr=strstr((char*)&buf,";");
ptr++;
int col=atoi(ptr);
DEBUGFUNC("buf=%s xpos=%i",buf,col);
fflush(NULL);

//if(page->line[page->currentLine].edLine[page->lineXCurs]=='\t')
//printf("\t");
//else
//printf("\e[1C");
//char *tl=oneLiner(false,"/bin/echo -e \"\e[6n\" 2>&1");
//DEBUGFUNC("tl=%s",tl);
//currentX=atoi(&tl[1]);
//moveInsert();
//page->lineXCurs++;
//return;
	if(page->lineXCurs+1==(int)strlen(page->line[page->currentLine].edLine))
		{
			if(page->currentLine+1==page->maxLines)
				return;
			page->lineXCurs=0;
			currentY++;
			page->currentLine++;
		}
	else
		{
			//if(page->line[page->currentLine].edLine[page->lineXCurs]=='\t')
			//	page->lineXCurs+=tabs;
//			/else
//DEBUGFUNC("page->lineXCurs=%x",page->line[page->currentLine].edLine[page->lineXCurs]);

//	int	direction=-1;
//	//if(rite==true)
//		direction=1;
//int toadd=currentX;
//	toadd++;
//	//toadd+=(tabs*direction);
//	
//	toadd=toadd / tabs;
//	//toadd=toadd * tabs;
//
//DEBUGFUNC("toadd=%i",toadd);

				page->lineXCurs++;
//currentX=currentX-toadd;
		}
	//currentX--;
	moveInsert();
	moveCursToTemp(currentX,currentY);
//DEBUGFUNC("page->lineXCurs=%x",page->line[page->currentLine].edLine[page->lineXCurs]);

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

