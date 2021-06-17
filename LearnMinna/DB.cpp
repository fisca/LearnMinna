#include "DB.h"

DB::DB()
{
	GetConfig();
}

DB::~DB()
{
	DisconnectDB();
}

void DB::GetConfig()
{
	string msg = "";
	wchar_t filePath[MAX_PATH], * cp;
	GetModuleFileNameW(NULL, filePath, MAX_PATH);

	cp = wcsrchr(filePath, '\\');
	if (cp != NULL) *cp = 0;

	lstrcatW(filePath, L"\\");
	lstrcatW(filePath, _config_file);

	if (!ifstream(filePath))
	{
		//wchar_t lm[MAX_PATH];
		//swprintf_s(lm, L"File %ws not found!", _config_file);
		//MessageBoxW(_hwnd, lm, L"Fild Not Found!", MB_ICONWARNING | MB_OK);
		//wstring ws(lm);
		//string str(ws.begin(), ws.end());

		char lg[MAX_PATH];
		sprintf_s(lg, "File %ws not found!", _config_file);
		MessageBoxA(_hwnd, lg, "File Not Found!", MB_ICONWARNING | MB_OK);
		Log::Info(lg);
		return;
	}

	GetPrivateProfileStringW(L"SQLServer", L"server", L"", _server, MAX_PATH, filePath);
	GetPrivateProfileStringW(L"SQLServer", L"uid", L"", _uid, MAX_PATH, filePath);
	GetPrivateProfileStringW(L"SQLServer", L"pwd", L"", _pwd, MAX_PATH, filePath);
	GetPrivateProfileStringW(L"SQLServer", L"database", L"", _database, MAX_PATH, filePath);

	//wstring ws(filePath);
	//string strFilePath(ws.begin(), ws.end());
	char file_path[MAX_PATH];
	//strcpy_s(file_path, strFilePath.c_str());
	sprintf_s(file_path, "%ws", filePath);

	//--- Config
	GetPrivateProfileStringA("LogPath", "log_path", "", _log_file_path, 300, file_path);
	/*GetPrivateProfileStringA("AppConfig", "schedule_time", "", _schedule_time, MAX_PATH, file_path);
	GetPrivateProfileStringA("AppConfig", "app_description", "", _app_description, 1000, file_path);
	GetPrivateProfileStringA("AppConfig", "run_server", "", _run_server, 100, file_path);
	GetPrivateProfileStringA("AppConfig", "start_night_session", "", _start_night_session, 9, file_path);*/
}

void DB::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	char       lszMessage[1000];
	char       lszState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE)
	{
		//fwprintf(stderr, L"Invalid handle!\n");
		Log::Info("Invalid handle!");
		return;
	}

	while (SQLGetDiagRecA(hType,
		hHandle,
		++iRec,
		(SQLCHAR*)lszState,
		&iError,
		(SQLCHAR*)lszMessage,
		(SQLSMALLINT)(sizeof(lszMessage) / sizeof(CHAR)),
		(SQLSMALLINT*)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated..
		if (strncmp(lszState, "01004", 5))
		{
			fprintf(stderr, "[%5.5s] %s (%d)\n", lszState, lszMessage, iError);
			char lg[1024];
			sprintf_s(lg, "[%5.5s] %s (%d)\n", lszState, lszMessage, iError);
			Log::Info(lg);
		}
	}
}

int DB::ConnectDB()
{
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlenvhandle) != SQL_SUCCESS)
	{
		Log::Info("SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlenvhandle) failed");
		return DisconnectDB();
	}
	if (SQLSetEnvAttr(sqlenvhandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) != SQL_SUCCESS)
	{
		Log::Info("SQLSetEnvAttr(sqlenvhandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) falied");
		return DisconnectDB();
	}
	if (SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, &sqlconnectionhandle) != SQL_SUCCESS)
	{
		Log::Info("SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, &sqlconnectionhandle) failed");
		return DisconnectDB();
	}

	wchar_t conf[1024];
	swprintf_s(conf, L"DRIVER={%ws};SERVER=%ws;DATABASE=%ws;UID=%ws;PWD=%ws;MARS_Connection=yes;",
		DB_DRIVER, _server, _database, _uid, _pwd);

	SQLWCHAR* conStr = (SQLWCHAR*)conf;

	SQLWCHAR retconstring[1024];
	SQLRETURN sqlRet = SQLDriverConnectW(sqlconnectionhandle, NULL, conStr, SQL_NTS, retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT);
	switch (sqlRet)
	{
	case SQL_SUCCESS_WITH_INFO:
		//Log::Info("Connection to database successful");
		break;
	case SQL_INVALID_HANDLE: case SQL_ERROR:
		Log::Info("Connection to database failed!");
		return DisconnectDB();
	default:
		Log::Info("Connection to database failed!");
		return DisconnectDB();
	}

	Log::Info("Connect to database successful");
	_con = 1;
	return 1;
}

int DB::DisconnectDB()
{
	SQLDisconnect(sqlconnectionhandle);
	SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);

	Log::Info("Disconnect and clear all sql handle complete");
	_con = 0;
	return 0;
}

SQLHSTMT DB::search_hiragana(CONST WCHAR* pwstr)
{
	WCHAR wqry[256] = L"select * from lm_kana ";
	if (pwstr != NULL && pwstr[0] != L'\0' && pwstr != L"")
	{
		lstrcat(wqry, L"where romanji like '%");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"%' or hiragana like '%");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"%' or katakana like '%");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"%' ");
	}

	SQLHSTMT hstmt;
	SQLINTEGER cbSearchStr = SQL_NTS;

	SQLRETURN sret = SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &hstmt);
	if (sret != SQL_SUCCESS)
	{
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, sret);
		Log::Info("search_hiragana SQLAllocHandle() failed");
		return NULL;
	}

	sret = SQLPrepare(hstmt, wqry, SQL_NTS);
	if (sret != SQL_SUCCESS)
	{
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, sret);
		Log::Info("search_hiragana SQLPrepare() failed");
		return NULL;
	}

	sret = SQLExecute(hstmt);
	if (sret != SQL_SUCCESS)
	{
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, sret);
		Log::Info("search_hiragana SQLExecute() failed");
		return 0;
	}

	return hstmt;
}

void DB::display_hiragana(CONST WCHAR* pwstr)
{
	HSTMT hstmt = search_hiragana(pwstr);
	while (SQLFetch(hstmt) == SQL_SUCCESS)
	{
		WCHAR wrmj[5], whrgn[5], wktkn[5], wmsg[50];

		SQLGetData(hstmt, 2, SQL_C_WCHAR, wrmj, 5, NULL);
		SQLGetData(hstmt, 3, SQL_C_WCHAR, whrgn, 5, NULL);
		SQLGetData(hstmt, 4, SQL_C_WCHAR, wktkn, 5, NULL);

		wsprintf(wmsg, L"%ws %ws %ws", wrmj, whrgn, wktkn);
		MessageBoxW(NULL, wmsg, L"test hiragana", 0);
		//cout << whrgn << " " << wktkn << endl;
	}
}

SQLHSTMT DB::search_kanji(CONST WCHAR* pwstr)
{
	WCHAR wqry[500] = L"select * from lm_kanji ";
	if (pwstr != NULL && pwstr[0] != L'\0' && pwstr != L"")
	{
		lstrcat(wqry, L"where character = N'");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"' or onyomi like '%' + N'");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"' + '%' or kunyomi like '%' + N'");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"' + '%' or meaning like '%' + N'");
		lstrcat(wqry, pwstr);
		lstrcat(wqry, L"' + '%' ");
	}

	SQLHSTMT hstmt;
	SQLINTEGER cbSearchStr = SQL_NTS;

	SQLRETURN sret = SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &hstmt);
	if (sret != SQL_SUCCESS)
	{
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, sret);
		Log::Info("search_kanji SQLAllocHandle failed");
		return NULL;
	}

	sret = SQLPrepare(hstmt, wqry, SQL_NTS);
	if (sret != SQL_SUCCESS)
	{
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, sret);
		Log::Info("search_kanji SQLPrepare failed");
		return NULL;
	}

	sret = SQLExecute(hstmt);
	if (sret != SQL_SUCCESS)
	{
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, sret);
		Log::Info("search_kanji SQLExecute failed");
		return 0;
	}

	return hstmt;
}

void DB::display_kanji(CONST WCHAR* pwstr)
{
	HSTMT hstmt = search_kanji(pwstr);
	while (SQLFetch(hstmt) == SQL_SUCCESS)
	{
		WCHAR wrmj[5], whrgn[50], wktkn[50], wmsg[150];

		SQLGetData(hstmt, 2, SQL_C_WCHAR, wrmj, 5, NULL);
		SQLGetData(hstmt, 3, SQL_C_WCHAR, whrgn, 50, NULL);
		SQLGetData(hstmt, 4, SQL_C_WCHAR, wktkn, 150, NULL);

		wsprintf(wmsg, L"%ws %ws %ws", wrmj, whrgn, wktkn);
		MessageBoxW(NULL, wmsg, L"Search Kanji", 0);
		//cout << whrgn << " " << wktkn << endl;
	}
}

