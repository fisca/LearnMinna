#pragma once

#if defined(__INTEL_COMPILER)
#define DEPRECATED [[deprecated]]
#endif

#include "framework.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include <conio.h>
#include <windows.h>
#include <wchar.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "Log.h"

using namespace std;

#define DB_DRIVER L"SQL Server Native Client 11.0"
const int APP_NAME_LEN = 50, APP_VERSION_LEN = 50, SCHEDULE_TIME_LEN = 20,
APP_DESCRIPTION_LEN = 1000, RUN_SERVER_LEN = 100, LOG_FILE_PATH_LEN = 300;

class DB
{
	HWND _hwnd;
	wchar_t _server[MAX_PATH], _uid[MAX_PATH], _pwd[MAX_PATH], _database[MAX_PATH], _account_table[MAX_PATH];
	wchar_t _config_file[MAX_PATH] = L"LearnMinna.ini";
	char _schedule_time[MAX_PATH], _app_description[1000], _run_server[100], _log_file_path[300], _start_night_session[9];

	SQLHANDLE sqlenvhandle;
	SQLHANDLE sqlconnectionhandle;

	void GetConfig();
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

public:
	int _con = 0;
	DB();
	~DB();
	int ConnectDB();
	int DisconnectDB();

	SQLHSTMT search_hiragana(CONST WCHAR* PWCHAR);
	void display_hiragana(CONST WCHAR* PWCHAR);
};

