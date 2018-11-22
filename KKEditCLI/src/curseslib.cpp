/*
 *
 * ©K. D. Hedger. Sun 20 May 13:34:30 BST 2018 keithdhedger@gmail.com

 * This file (curseslib.cpp) is part of KKEditCLI.

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

#include <termios.h>
#include <termcap.h>
#include <sys/ioctl.h>

#include "globals.h"

termios	termConfig;

void moveCursToTemp(int x,int y)
{
	printf("\e[%i;%iH",y,x);
	fflush(NULL);
}

void initCursesLib(void)
{
	char	*dump;
	winsize w;

	asprintf(&dump,"tabs $(seq -s , 1 %i 256) 2>&1",tabs);
	system(dump);
	free(dump);
	tcgetattr(STDIN_FILENO,&termConfig);
	termConfig.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&termConfig);

    ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);

	if(forceCols==-1)
		cols=w.ws_col-1;
	else
		cols=forceCols;

	rows=w.ws_row;
	minY=mBarHite+2;
	maxRows=rows-minY;
	maxCols=cols-minX;
	startY=1;
	endY=startY+maxRows;

	menuHite=rows-mBarHite;
	for(int j=0;j<MAXBOOKMARKS;j++)
		{
			bookmarks[j].line=-1;
			bookmarks[j].pageNum=-1;
		}
}

void finalizeCursesLib(void)
{
	int	cnt=0;

	termConfig.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&termConfig);
	unlink("/dev/shm/src");
	system("/usr/bin/tabs -8 2>&1");

	clearTagList();
	for(int j=0;j<maxPages;j++)
		{
			if(pages[j]!=NULL)
				{
					page=pages[j];
					setTempEdFile(page->filePath);
					closePage();
				}
		}
	free(pages);

	if(tabsMenuNames!=NULL)
		{
			while(tabsMenuNames[cnt]!=NULL)
				free(tabsMenuNames[cnt++]);
			free(tabsMenuNames);
			tabsMenuNames=NULL;
		}
	freeAndNull(&tmpEdFile);
	freeAndNull(&tmpEdFilePath);
	end_dialog();

	SETNORMAL;
}

void clearScreen(void)
{
	printf("\e[2J\e[H"); 
}

void clearToEOL(void)
{
	printf("\e[2J\e[H");
}
