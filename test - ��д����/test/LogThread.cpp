#include "stdafx.h"
#include "LogThread.h"

LogThread::LogThread( Loggerhandle* Lgi, U32 id)
{
	 m_LoggerhandleMgr = Lgi;
	 m_threadId = id;
}

LogThread::~LogThread()
{

}

void LogThread::operator()()
{
	 m_LoggerhandleMgr->handleWriteLog();
}