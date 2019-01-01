/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:10:14 BST 2018 keithdhedger@gmail.com

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
		{"no-hilite",0,0,'n'},
		{"tabs",1,0,'t'},
		{"cols",1,0,'c'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
	printf("kkeditcli %s\n"
	"Usage: kkeditcli [OPTION]\n"
	" -l, --no-liveupdate	Don't update source highlighting as you type.\n"
	" -n, --no-hilite	No source highlighting.\n"
	" -t, --tabs		Tab width.\n"
	" -c, --cols		Force number of coloumns.\n"
	" -v, --version		Output version information and exit\n"
	" -h, -?, --help		Print this help\n\n"
	"Report bugs to kdhedger68713@gmail.com\n"
	,VERSION);
}

int main(int argc, char **argv)
{
	int		c;
	int		option_index=0;
	char	tmpfoldertemplate[]="/dev/shm/KKEditCLI-XXXXXX";

	tmpEdDir=mkdtemp(tmpfoldertemplate);
	asprintf(&srcPath,"%s/src",tmpEdDir);

	while (1)
		{
			c = getopt_long (argc, argv, "t:c:v?hln",long_options, &option_index);
			if (c == -1)
				break;

			switch (c)
				{
					case 'l':
						liveUpdate=false;
						break;

					case 'n':
						hilite=false;
						break;

					case 't':
						tabs=atoi(optarg);
						break;

					case 'c':
						forceCols=atoi(optarg);
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
				}
		}

	clearScreen();
	initCursesLib();
//	int mouse = 0;
////////////////////printf("\033[?%dh",1000);

	if(optind < argc)
		{
			while(optind < argc)
				{
					if(access(argv[optind],F_OK)!=F_OK)
						{
							oneLiner(true,"touch \"%s\" &>/dev/null",argv[optind]);
							if(access(argv[optind],F_OK)!=F_OK)
								{
									makeNewFile();
									optind++;
									continue;
								}
						}
					initEditor();
					setTempEdFile(argv[optind]);
					oneLiner(true,"cp %s %s/%s",argv[optind],tmpEdDir,tmpEdFile);
					page->filePath=strdup(argv[optind]);
					openTheFile(tmpEdFilePath,hilite);
					optind++;
				}
		}
	else
		{
			makeNewFile();
		}

	printLines();
	currentX=minX;
	currentY=minY;
	moveCursToTemp(currentX,currentY);
	printStatusBar();

	eventLoop();
	finalizeCursesLib();
	free(srcPath);
	unlink(srcPath);
	rmdir(tmpEdDir);
	printf("\n");
	system("stty sane");
	printf("\033[?%dl\n", 1000);
	return 0;
}

