
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