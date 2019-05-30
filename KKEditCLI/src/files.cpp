/*
 *
 * ©K. D. Hedger. Tue  7 May 20:41:19 BST 2019 keithdhedger@gmail.com

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

#include "globals.h"

void getTagList(const char *filepath)
{
	char		*command;
	const char	*sortcommand="sort -k 2rb,2rb -k 1b,1b";
	FILE		*fp;
	char		line[2048];
	char		*ptr;
	char		*ptr2;
	funcStruct	*fd;

	mainApp->menuBar->CTK_clearMenu(FUNCMENU);

	for(int j=0;j<functions.size();j++)
		{
			freeAndNull(&functions[j]->name);
			freeAndNull(&functions[j]->type);
			freeAndNull(&functions[j]->menuLabel);
			delete functions[j];
		}
	functions.clear();
	asprintf(&command,"ctags -x \"%s\" 2>/dev/null|%s|sed 's@ \\+@ @g' 2>/dev/null",filepath,sortcommand);
	fp=popen(command, "r");
	
	while(fgets(line,2048,fp))
		{
			fd=new funcStruct;
			fd->pageNumber=mainApp->pageNumber;
			if(strlen(line)>1)
				line[strlen(line)-1]=0;
			ptr=strchr((char*)&line,' ');
			*ptr=0;
			asprintf(&fd->name,"%s",line);
			ptr++;
			ptr2=strchr(ptr,' ');
			*ptr2=0;
			fd->type=strdup(ptr);
			ptr2++;
			ptr=strchr(ptr2,' ');
			*ptr=0;
			fd->line=atoi(ptr2);
			asprintf(&fd->menuLabel," %.4s %s ",fd->type,fd->name);
			functions.push_back(fd);
			mainApp->menuBar->CTK_addMenuItem(FUNCMENU,fd->menuLabel,false);
		}
	pclose(fp);
	free(command);
}
