/*
 *
 * ©K. D. Hedger. Tue  7 May 20:41:38 BST 2019 keithdhedger@gmail.com

 * This file (files.h) is part of KKEditCLI.

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

#ifndef _FILES_
#define _FILES_

#define TOOLNAMELISTWIDTH 40
#define INPUTWIDTH 42
enum {IGNOREOP=0,PASTEOP,REPLACEOP,VIEWOP,NOMORECHECKS};
//enum {INFOLABEL=0,TOOLSLABEL,NAMELABEL,COMMANDLABEL,INSTRUCTIONLABEL,NOMORELABELS};
enum {LABELINFO=0,LABELLIST,LABELTOOLNAME,LABELCOMMAND,LASTLABEL};
enum {NAMEINPUT=0,COMMANDINPUT,NOMOREINPUTS,};
enum {APPLYBUTTON=1,DELETEBUTTON,EDITBUTTON,FINISHEDBUTTON,NOMOREBUTTONS};

void getTagList(const char *filepath);
int loadVarsFromFile(char *filepath,args *dataptr);
void buildToolsList(void);
void manageTools(void);

#endif
