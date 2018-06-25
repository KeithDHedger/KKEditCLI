/*
 *
 * ©K. D. Hedger. Sat 26 May 12:06:58 BST 2018 kdhedger68713@gmail.com

 * This file (editor.h) is part of KKEditCLI.

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

#ifndef _EDITOR_
#define _EDITOR_

struct lineStruct
{
	char			*edLine;
	char			*srcLine;
	int				lineNum;
	int				lineLen;
	bool			isSplitLine;
};

struct pageStruct
{
	int				maxLines;
	int				currentLine;
	int				topLine;
	int				lineXCurs;
	lineStruct		line[MAXLINES];
	char			*filePath;
	bool			dirty;
};

extern pageStruct	*page;

void initEditor(void);
void moveInsert(void);
void moveCursRite(void);
void moveCursDown(void);
void moveCursUp(void);
void moveCursLeft(void);
void closePage(void);
void adjCursor(void);

#endif
