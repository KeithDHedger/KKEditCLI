/*
 *
 * ©K. D. Hedger. Sat 26 May 12:06:58 BST 2018 keithdhedger@gmail.com

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

using namespace std;

struct bookmarkStruct
{
	int					line;
	int					pageNum;
};

struct pageStruct
{
	int					maxLines;
	int					currentLine;
	int					topLine;
	int					lineXCurs;
	char				*filePath;
	bool				dirty;
	int					pageNum;
	int					saveX;
	int					saveY;
	int					saveCurrentLine;
	vector<string>		editLineArray;
	vector<string>		printLineArray;
	vector<int>			lineNumber;
};

struct funcStruct
{
	char				*name;
	char				*type;
	int					line;
};

#define MAXBOOKMARKS	100

extern char				*wordBuf[1024];
extern const char		*wordBufPtr;
extern unsigned			wordStart;
extern unsigned			wordEnd;
extern unsigned			wordLen;
extern char				*cutBuffer;
extern int				searchStart;
//e//xtern int				searchStartX;
//extern int				searchStartY;
extern string			needle;

extern int				currentPage;
extern bookmarkStruct	bookmarks[MAXBOOKMARKS];

void initEditor(void);
void moveCursRite(void);
void moveCursDown(void);
void moveCursUp(void);
void moveCursLeft(void);
void closePage(void);
void adjCursor(void);
int findLineByLineNumber(int linenumber);
void findWordUnderCursor(void);
void switchPage(int newpagenum,int gotoline);
bool search(bool again);

#endif
