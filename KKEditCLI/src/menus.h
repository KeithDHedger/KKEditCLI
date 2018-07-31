/*
 *
 * ©K. D. Hedger. Tue  5 Jun 15:53:40 BST 2018 keithdhedger@gmail.com

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

enum {FLATNORM=0,FLATINVERT};

enum {FILEMENU=0,EDITMENU,TABSMENU,NAVIGATIONMENU,FUNCTIONSMENU,BOOKMARKSMENU,TOOLSMENU,MAXMENUS};
enum {FILENEW=1,FILEOPEN,FILESAVE,FILESAVEAS,FILECLOSE,FILEQUIT,FILECNT};
enum {EDITCUT=1,EDITCOPY,EDITPASTE,EDITCNT};
enum {VIEWDOC0=1,VIEWDOC1,VIEWDOC2,VIEWCNT};
enum {NAVEGOTODEF=1,NAVEOPENINC,NAVGOTOLINE,NAVOPENMANPAGE,NAVCNT};
enum {FUNC0=1,FUNC1,FUNCCNT};
enum {BMREMOVEALL,BMTOGGL,BMCNT};
enum {TOOLMANAGE=1,TOOLCNT};

extern int			menuStart;
//menus
//static menus
extern const char	*menuNames[];
extern const char	*fileMenuNames[];
extern const char	*editMenuNames[];
extern const char	*navMenuNames[];
//dynamic menus
extern char			**tabsMenuNames;
extern funcStruct	**functionData;
extern char			**functionsMenuNames;
extern const char	*bookmarksMenuNames[];
extern const char	*toolsMenuNames[];

void drawMenuBar(void);
int doMenuEvent(const char **menunames,int sx,int sy,bool doshortcut);
void buildTabMenu(void);

#endif
