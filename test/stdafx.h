// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>  
#include <fstream>
#include <sstream>
using namespace std;

#ifdef WIN32		//WIN32

#include <tchar.h>
#include <Winsock2.h>
#include <windows.h>
#include <process.h>
#include <Winbase.h>
#include <time.h>

#else				//UNIX

#include <pthread.h>
#include <string.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#endif
