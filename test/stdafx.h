// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
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



// TODO: 在此处引用程序需要的其他头文件

#endif
