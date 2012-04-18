#ifndef __LOGTHREAD_H__
#define __LOGTHREAD_H__
#include "Header.h"
#include "Loggerhandle.h"
class Loggerhandle;
class LogThread
{
public:
	LogThread( Loggerhandle * Lgi, U32 id);
	~LogThread(void);

	void operator ()();

public:
	Loggerhandle*  m_LoggerhandleMgr;
	U32 m_threadId;
};

#endif //__LOGTHREAD_H__