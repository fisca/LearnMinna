// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// reference additional headers your program requires here
//#include <winsock2.h>
//#include <ws2tcpip.h>
//
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <wchar.h>
#include <time.h>

using namespace std;

#define APP_NAME "LearnMinna"
#define APP_NAME_SIZ 50
#define APP_VERSION "1.1.0.1"
#define CONFIG_FILE APP_NAME ## ".ini"
