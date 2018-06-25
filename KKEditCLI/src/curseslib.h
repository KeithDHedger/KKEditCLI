/*
 *
 * ©K. D. Hedger. Sun 20 May 13:34:40 BST 2018 kdhedger68713@gmail.com

 * This file (curseslib.h) is part of KKEditCLI.

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

#ifndef _CURSESLIB_
#define _CURSESLIB_

#define ESCCHAR 0x1b
#define CURSFORWARD "\e[1C"
#define NEXTSCRLINE "\n\e[8C"

#define	FOREBLACK "\e[30m"
#define FORERED "\e[31m"
#define FOREGREEN "\e[32m"
#define FOREYELLOW "\e[33m"
#define FOREBLUE "\e[34m"
#define FOREMAGENTA "\e[35m"
#define FORECYAN "\e[36m"
#define FOREWHITE "\e[37m"

#define	BACKBLACK "\e[40m"
#define BACKRED "\e[41m"
#define BACKGREEN "\e[42m"
#define BACKYELLOW "\e[43m"
#define BACKBLUE "\e[44m"
#define BACKMAGENTA "\e[45m"
#define BACKCYAN "\e[46m"
#define BACKWHITE "\e[47m"

#define BLACK "0"
#define RED "1"
#define GREEN "2"
#define YELLOW "3"
#define BLUE "4"
#define MAGENTA "5"
#define CYAN "6"
#define WHITE "7"
#define FORECOL(x) "\e[3" x "m"
#define BACKCOL(x) "\e[4" x "m"
#define SETFORECOL(x) printf(FORECOL(x));
#define SETBACKCOL(x) printf(BACKCOL(x));
#define	SETNORMAL printf("\e[39;49m\e[0m")

#define NORMAL "\e[0m"

#define UNDERSCOREON "\e[4m"
#define UNDERSCOREOFF "\e[24m"
#define SETINVERSEON printf("\e[7m");
#define SETINVERSEOFF printf("\e[27m");

#define FORECOL256(col) "\e[38;5;" #col "m"
#define BACKCOL256(col) "\e[48;5;" #col "m"

#define CLEARTOEOL "\e[0K"

#define HIDECURS printf("\e[?25l")
#define SHOWCURS printf("\e[?25h")

void initCursesLib(void);
void finalizeCursesLib(void);
void moveCursToTemp(int x,int y);
void clearScreen(void);
void clearSrcLines(void);

#endif

