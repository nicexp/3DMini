#include "stdio.h"
#include <ctime>
#include <sys\timeb.h>
#include <sys\types.h>
#include <stdarg.h>
#include "3DLog.h"

char _3dlog_buffer[1024];
static FILE* logfp = NULL;

int LOG_INIT()
{
	if ((logfp = fopen("gamelog.txt", "w")) == NULL)
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
	if (logfp)
	{
		LOG_OUT("close the log");
		
		fclose(logfp);
		logfp = NULL;
		return 0;
	}

	return -1;
}

int LOG_OUT(char* fmt_str, ...)
{
	char buffer[1024];

	va_list arglist;

	if (!fmt_str || !logfp)
		return -1;

	va_start(arglist, fmt_str);
	vsprintf(buffer, fmt_str, arglist);
	va_end(arglist);

	fprintf(logfp, buffer);
	fprintf(logfp, "\n");

	fflush(logfp);

	return 0;
}