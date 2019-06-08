/*
 *
 * ©K. D. Hedger. Mon  6 May 16:40:22 BST 2019 keithdhedger@gmail.com

 * This file (main.cpp) is part of KKEditCLI.

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <fstream>
#include <curses.h>

#include <cursesGlobals.h>

#include "globals.h"

void mainloopCB(void *mainc,void *data)
{
//	fprintf(stderr,">>>>>\n");
}

int main(int argc, char **argv)
{
	char			tmpfoldertemplate[]="/dev/shm/KKEditCLI-XXXXXX";
	coloursStruct	cs;
	char			*path=NULL;

	tmpEdDir=mkdtemp(tmpfoldertemplate);
	if(tmpEdDir==NULL)
		{
			fprintf(stderr,"Can't create tempory folder in /dev/shm, quiting ...\n");
			exit(1);
		}
	asprintf(&manFile,"%s/manfile",tmpEdDir);
	sinkReturn=asprintf(&configFolder,"%s/.KKEditCLI",getenv("HOME"));
	oneLiner(false,"mkdir -p %s",configFolder);

	setupMenus();
	buildToolsList();

	mainApp->CTK_setTabWidth(TABWIDTH);

	cs.hiliteBackCol=BACK_CYAN;
	cs.hiliteForeCol=FORE_BLACK;
	cs.foreCol=FORE_WHITE;
	cs.backCol=BACK_BLACK;

	mainApp->CTK_setColours(cs);

	if(argc==1)
		{
			mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,false,"\n");
			mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]->CTK_setShowLineNumbers(showLineNumbers);
			mainApp->CTK_setPageUserData(0,(void*)strdup("/tmp/Untitled-1"));
		}
	else
		{
			for(int j=1;j<argc-1;j++)
				{
					path=realpath(argv[j],NULL);
					if(path==NULL)
						path=strdup(argv[j]);
					mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,path);
					mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]->CTK_setShowLineNumbers(showLineNumbers);

					mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)path);
					setInfoLabel();
					mainApp->CTK_addPage();
				}
			path=realpath(argv[argc-1],NULL);
			mainApp->CTK_addNewSourceEditBox(mainApp,1,TOPLINE,windowCols,windowRows,true,path);
			mainApp->pages[mainApp->pageNumber].srcEditBoxes[0]->CTK_setShowLineNumbers(showLineNumbers);
			mainApp->CTK_setPageUserData(mainApp->pageNumber,(void*)path);
		}

	setInfoLabel();
	rebuildTabMenu();
	//mainApp->eventLoopCB=mainloopCB;
	mainApp->CTK_mainEventLoop();
	for(int k=0;k<mainApp->pages.size();k++)
		free(mainApp->pages[k].userData);
	delete mainApp;
	SETSHOWCURS;
	unlink(manFile);
	rmdir(tmpEdDir);
	free(manFile);
	free(configFolder);
	return 0;
}