/*
 *
 * ©K. D. Hedger. Tue  7 May 16:14:41 BST 2019 keithdhedger@gmail.com

 * This file (gui.cpp) is part of KKEditCLI.

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

void setInfoLabel(void)
{
	coloursStruct	cs;

	cs.foreCol=FORE_BLACK ;
	cs.backCol=BACK_GREEN;

	if(access((char*)mainApp->pages[mainApp->pageNumber].userData,R_OK|W_OK)!=0)
		cs.backCol=BACK_RED;

	if(fileInfoLabel==NULL)
		fileInfoLabel=mainApp->CTK_addNewLabel((windowCols/2)-(strlen((char*)mainApp->pages[mainApp->pageNumber].userData)/2),INFOLINE,strlen((char*)mainApp->pages[mainApp->pageNumber].userData),1,(char*)mainApp->pages[mainApp->pageNumber].userData);
	else
		{
		//fileInfoLabel->CTK_newLabel((windowCols/2)-(strlen((char*)mainApp->pages[mainApp->pageNumber].userData)/2),INFOLINE,strlen((char*)mainApp->pages[mainApp->pageNumber].userData),1,(char*)mainApp->pages[mainApp->pageNumber].userData);
			//fileInfoLabel->CTK_updateText((char*)mainApp->pages[mainApp->pageNumber].userData);
			//delete fileInfoLabel;
			fileInfoLabel=mainApp->CTK_addNewLabel((windowCols/2)-(strlen((char*)mainApp->pages[mainApp->pageNumber].userData)/2),INFOLINE,strlen((char*)mainApp->pages[mainApp->pageNumber].userData),1,(char*)mainApp->pages[mainApp->pageNumber].userData);
		}
	fileInfoLabel->CTK_setColours(cs);
}