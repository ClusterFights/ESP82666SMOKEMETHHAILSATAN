#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define DEFAULT_DIR "/mnt/iso/"

int main ( int argc, char *argv[] )
{	
	DIR *dp;
	struct dirent *ep;
	struct stat sb;
	
	dp = opendir (argc > 1 ? argv[1] : DEFAULT_DIR);
	
	if (dp != NULL)
	{
		while (ep = readdir (dp))
			puts (ep->d_name);
		/* if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode))
		   {
			...it is a directory...
		   } */
		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory. Is...it...real?");
		return EXIT_FAILURE;

	return 0;
}
