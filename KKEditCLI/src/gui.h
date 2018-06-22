/*
 *
 * ©K. D. Hedger. Sun 20 May 14:12:16 BST 2018 kdhedger68713@gmail.com

 * This file (gui.h) is part of KKEditCLI.

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

#ifndef _GUI_
#define _GUI_

//keys
#define KEYUP 'A'
#define KEYDOWN 'B'
#define KEYRITE 'C'
#define KEYLEFT 'D'
#define PAGEUP '5'
#define PAGEDOWN '6'
#define BACKSPACE 0x7f
#define DELETEKEY '3'
#define RETURNKEY 0xa
#define CURSHOME 'H'
#define CURSEND 'F'
#define CURSHOMECONS '1'
#define CURSENDCONS '4'

#define CTRL_B 0x2
#define CTRL_E 0x5
#define CTRL_F 0x6
#define CTRL_HASH 0x23
#define CTRL_N 0xe
#define CTRL_T 0x14
#define CTRL_U 0x15
#define CTRL_V 0x16

//ui
#define CONT 0
#define BRAKE -1
#define MENURITE -2
#define MENULEFT -3

//mouse
#define BUTTON1DOWN ' '
#define BUTTON1UP '#'

void initSrcHilite(void);
void printSrcLines(void);
void eventLoop(void);
void printStatusBar(void);
void printLines(void);
void refreshScreen(void);

#endif
