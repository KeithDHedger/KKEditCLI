/*
 *
 * ©K. D. Hedger. Mon  6 May 17:15:38 BST 2019 keithdhedger@gmail.com

 * This file (menus.h) is part of KKEditCLI.

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

#ifndef _MENUS_
#define _MENUS_

#include "globals.h"

enum {FILEMENU=0,EDITMENU,TABMENU,NAVMENU,FUNCMENU,BMMENU,TOOLSMENU,HELPMENU};

enum {NEWITEM=0,OPENITEM,SAVEITEM,SAVEASITEM,CLOSEITEM,QUITITEM};
enum {COPYWORD=0,CUTWORD,COPYLINE,CUTLINE,PASTE};
enum {NEXTTAB=0,PREVTAB};
enum {NAVGOTODFINE=0,NAVOPENINCLUDE,NAVGOTOLINE,NAVOPENMANPAGE,NAVFIND,NAVFINDNEXT};
enum {RMALLBMS=0,TOGGLEBM};
enum {HELP=0,ABOUT};

void rebuildTabMenu(void);
void setupMenus(void);
void handleFileMenu(CTK_cursesMenuClass *mc);
void handleEditMenu(CTK_cursesMenuClass *mc);
void handleNavMenu(CTK_cursesMenuClass *mc);
void handleTabMenu(CTK_cursesMenuClass *mc);
void handleHelpMenu(CTK_cursesMenuClass *mc);

#endif
