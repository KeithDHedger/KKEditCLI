/*
 *
 * ©K. D. Hedger. Sun 20 May 14:02:22 BST 2018 kdhedger68713@gmail.com

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

//file data
extern int	maxLines;
extern int	dispLineData[];

void openTheFile(const char *path);
void writeCharToFile(char c);
bool deleteCharFromFile(bool back);

int getDispLineData(int start,int dne);
void saveFile(const char *path);
void askSaveIfdirty(void);
void setTempEdFile(const char *path);
void makeNewFile(void);
void askOpenFile(void);
void getTagList(void);
void clearTagList(void);

#endif
