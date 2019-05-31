/*
 *
 * ©K. D. Hedger. Mon  6 May 17:24:33 BST 2019 keithdhedger@gmail.com

 * This file (globals.h) is part of KKEditCLI.

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


#ifndef _GLOBALS_
#define _GLOBALS_


#include <string>
#include <vector>

#include <fstream>
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <cursesGlobals.h>

#include "menus.h"
#include "gui.h"
#include "files.h"

#define TABWIDTH 4
#define TOPLINE 4
#define INFOLINE 2

#define MAXBUFFER 2048
#define MAXLINES 50000

#define BOLDCOL "31"
#define UNDELINECOL "32"
#define DEFFORECOL "\\x1b[0;39m"

#define UNDERLINEON "\\x1b\\[4m"
#define UNDERLINEOFF "\\x1b\\[24m"
#define BOLDON "\\x1b\\[1m"
#define BOLDOFF "\\x1b\\[22m"
#define BOLD "\\x1b\\[1;" BOLDCOL "m"
#define UNDERLINE "\\x1b\\[1;" UNDELINECOL "m"

struct funcStruct
{
	char	*name;
	char	*type;
	int		line;
	int		pageNumber;
	char	*menuLabel;
};

struct bookmarkStruct
{
	int		pageNum=-1;
	int		lineNum=-1;
	char	*label=NULL;
	bool	isSrc=false;
};

enum {REMOVEMARKS=0,TOGGLEMARK};

static inline void freeAndNull(char **data)
{
	if((data!=NULL) && (*data!=NULL))
		{
			free(*data);
			*data=NULL;
		}
}

extern CTK_mainAppClass			*mainApp;
extern int						windowRows;
extern int						windowCols;
extern int						showLineNumbers;
extern const char				*tmpEdDir;
extern char						*manFile;

extern int						newCnt;
extern std::string				clip;

//functions
extern std::vector<funcStruct*>	functions;
//bms
extern std::vector<bookmarkStruct>	bms;

char* oneLiner(bool noreturn,const char* fmt,...);

#endif
