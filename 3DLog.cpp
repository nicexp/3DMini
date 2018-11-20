#include "stdio.h"
#include <ctime>
#include <sys\timeb.h>
#include <sys\types.h>
#include <stdarg.h>
#include "3DLog.h"

static FILE* fp = NULL;

int LOG_INIT()
{
	if ((fp = fopen("gamelog.txt", "w")) == NULL)
		return -1;

	struct _timeb timebuffer;
	char *timeline;
	char timestring[280];

	_ftime(&timebuffer);
	timeline = ctime(&(timebuffer.time));
	LOG_OUT("open log file");
	sprintf(timestring, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);
	LOG_OUT(timestring);

	return 0;
}

int LOG_CLOSE()
{
	if (fp)
	{
		LOG_OUT("close the log");
		
		fclose(fp);
		fp = NULL;
		return 0;
	}

	return -1;
}

int LOG_OUT(char* fmt_str, ...)
{
	char buffer[1024];

	va_list arglist;

	if (!fmt_str || !fp)
		return -1;

	va_start(arglist, fmt_str);
	vsprintf(buffer, fmt_str, arglist);
	va_end(arglist);

	fprintf(fp, buffer);
	fprintf(fp, "\n");

	fflush(fp);

	return 0;
}