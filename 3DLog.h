#ifndef _3DLOG_H
#define _3DLOG_H

//#define _LOG_ALL

static char _3dlog_buffer[1024];

#ifdef _LOG_ALL
#define DEBUG_LOG(fmt_str, ...) {sprintf(_3dlog_buffer, "File:%s,Function:%s,Line:%d, %s",__FILE__,__FUNCTION__, __LINE__, fmt_str);LOG_OUT(_3dlog_buffer, __VA_ARGS__);}
#else
#define DEBUG_LOG(fmt_str, ...) {sprintf(_3dlog_buffer, "Function:%s,Line:%d, %s",__FUNCTION__, __LINE__, fmt_str);LOG_OUT(_3dlog_buffer, __VA_ARGS__);}
#endif



int LOG_INIT();
int LOG_CLOSE();
int LOG_OUT(char* fmt_str, ...);

#endif