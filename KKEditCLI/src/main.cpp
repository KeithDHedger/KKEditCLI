/*
 *
 * ©K. D. Hedger. Sun 20 May 13:34:52 BST 2018 kdhedger68713@gmail.com  ©K. D. Hedger. Sun 20May13:34:52BST201kdhedger68713@gmail.com ©K. D. Hedger. Sun 20 May 13:34:52 BST 2018 kdhedger68713@gmail.com

 * This file (main.cpp) is part of KKEditCLI.

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "globals.h"

#define UNKNOWNARG -100

struct option long_options[] =
	{
		{"no-liveupdate",0,0,'l'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
printf("Usage: kkeditcli [OPTION]\n"
	"A CLI application\n"
	" -l, --no-liveupdate	Update source highlighting as you type.\n"
	" -v, --version	output version information and exit\n"
	" -h, -?, --help	print this help\n\n"
	"Report bugs to kdhedger@yahoo.co.uk\n"
	);
}

int main(int argc, char **argv)
{
	int c;
	while (1)
		{
		int option_index = 0;
		c = getopt_long (argc, argv, "v?hl",long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
			{
			case 'l':
				liveUpdate=false;
				break;

			case 'v':
				printf("kkeditcli %s\n",VERSION);
				return 0;
				break;

			case '?':
			case 'h':
				printhelp();
				return 0;
				break;

			default:
				fprintf(stderr,"?? Unknown argument ??\n");
				return UNKNOWNARG;
			break;
			}
		}

	clearScreen();
	initCursesLib();
	initEditor();
	oneLiner(true,"cp %s %s",argv[1],tmpedfile);
	page->filePath=strdup(argv[1]);
	openTheFile(tmpedfile);
	printLines();
	currentX=minX;
	currentY=minY;
	moveCursToTemp(currentX,currentY);
	printStatusBar();

	eventLoop();
	finalizeCursesLib();
	closePage();
	fprintf(stderr,">>>>>>>>>QUITING\n");
	moveCursToTemp(1,rows);
	printf("\n");
	return 0;
}
