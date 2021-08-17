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

#define TOOL_IGNORE_OP		0
#define TOOL_PASTE_OP		1
#define TOOL_REPLACE_OP		2
#define	TOOL_VIEW_OP 		4
#define TOOL_RELOAD_FILE	8

#include "globals.h"

struct shortcutStruct
{
	int		menu;
	int		item;
	char	key;
};

enum {FILEMENU=0,EDITMENU,TABMENU,NAVMENU,FUNCMENU,SYNTAXMENU,BMMENU,TOOLSMENU,HELPMENU};

enum {NEWITEM=0,OPENITEM,SAVEITEM,SAVEASITEM,CLOSEITEM,SHELLITEM,QUITITEM};
enum {COPYWORD=0,CUTWORD,PASTE};
enum {NEXTTAB=0,PREVTAB};
enum {NAVGOTODFINE=0,NAVOPENINCLUDE,NAVGOTOLINE,NAVOPENMANPAGE,NAVFIND,NAVFINDNEXT};
enum {RMALLBMS=0,TOGGLEBM};
enum {HELP=0,ABOUT};
enum {HILITEPLAIN=0,HILITESHELL,HILITECPP,HILITEJS,HILITEPHP,HILITEPERL};

void rebuildTabMenu(void);
void setupMenus(void);
void handleFileMenu(CTK_cursesMenuClass *mc);
void handleEditMenu(CTK_cursesMenuClass *mc);
void handleNavMenu(CTK_cursesMenuClass *mc);
void handleTabMenu(CTK_cursesMenuClass *mc);
void handleHelpMenu(CTK_cursesMenuClass *mc);
void handleSyntaxMenu(CTK_cursesMenuClass *mc);

#endif
