/*-----------------
* Map Algebra
*
*-----------------*/

#include <stdio.h>
#include <stdlib.h>
#include "winbox.h"
#include "utils.h"


typedef struct Test
{
	int* tab;
	POINT p;

}Test;


void setter(Test* t)
{
	t->tab = (int*)malloc(10 * sizeof(int));
	t->tab[0] = 10;
	t->p.x = 5;
	t->p.y = 5;

	printf("%d %d\n", t->tab[0], t->p.x);

}
char *tempDir;
char *temps[6] = { "TEMP", "temp", "TMP", "tmp", "TEMPDIR" , "tempdir"};
char* appPath;

void getDirs(char* app)
{		
	for (int i = 0; i < 6; i++)
	{
		tempDir = getenv(temps[i]);		
		if (tempDir)
		{
			break;
		}
	}
	if (!tempDir)
	{
		tempDir = (char*)malloc(1);
		tempDir[0] = '\\';
	}
	printf("Temp directory: %s\n", tempDir);

	appPath = (char*)malloc(MAX_PATH);
#ifndef __WIN32
	readlink("/proc/self/exe", appPath, MAX_PATH);	
#else
	//wchar_t appTmp[MAX_PATH];
	GetModuleFileNameA(NULL, appPath , MAX_PATH);
	//wcstombs(appPath, appTmp, MAX_PATH);
	
#endif
	getPathWithoutFinalSlash(appPath, appPath);
	printf("App directory: %s from %s\n", appPath,app);


}
int main(int argc, char** argv)
{ 
	char** _args;
	int i = 0, k = 0;
	const char* s;

	printf("\nMap Algebra Tool v%d.%d\n\nUsage: mapalgebra file1 [file2 file3 ...]\n\n",
		MAJORVERSION, MINORVERSION);

	printf("FreeImage version: %s\n", FreeImage_GetVersion());
	getDirs(argv[0]);

	if (argc > 1)
		showGlutWindow(argc, argv);		    
	else
	{ 
		const char* files = tinyfd_openFileDialog("Open an image...", "", 0, NULL, "", 1);
		if (files)
		{
			s = files;
			for (i = 0; s[i]; s[i] == '|' ? i++ : *s++);
			_args = (char**)calloc(i + 1, sizeof(char*));

			if (i > 0)
			{
				k = 1;
				s = strtok((char*)files, "|");
				while (s != NULL)
				{
					printf("%s\n", s);
					_args[k] = (char*)calloc(strlen(s) + 1, sizeof(char));
					strcpy(_args[k], s);
					k++;
					s = strtok(NULL, "|");
				} 
				showGlutWindow(i + 2, _args);
			}
			else
			{
				_args = (char**)calloc(2, sizeof(char*));
				_args[1] = (char*)calloc(strlen(files) + 1, sizeof(char));
				strcpy(_args[1], files);
				showGlutWindow(2, _args);
			}
		}
    }
    return 0;
}
