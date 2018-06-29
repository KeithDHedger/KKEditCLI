/*
 *
 * ©K. D. Hedger. Sun 20 May 13:34:30 BST 2018 kdhedger68713@gmail.com

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

	asprintf(&dump,"tabs $(seq -s , 1 %i 256)",tabs);
	system(dump);
	free(dump);
	tcgetattr(STDIN_FILENO,&termConfig);
	termConfig.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&termConfig);

    ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);

	cols=w.ws_col;
	cols=159;
	rows=w.ws_row;
	maxRows=rows-minY;
	maxCols=cols-minX;
	startY=1;
	endY=startY+maxRows;

	menuHite=rows-mBarHite-statusBarHite-1;
	
//	system("tabs $(seq -s , 1 8 256)");
}

void finalizeCursesLib(void)
{
	termConfig.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&termConfig);
	unlink("/dev/shm/src");
	system("/usr/bin/tabs -8");
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
