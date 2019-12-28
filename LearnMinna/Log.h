#pragma once

#include "framework.h"
#include <Shlobj.h>

using namespace std;

class Log {

	static void GetConfig(char* log_path)
	{
		//string msg = "";
		char filePath[MAX_PATH], * cp, logPath[MAX_PATH];
		GetModuleFileNameA(NULL, filePath, MAX_PATH);

		//if ((cp = strrchr(filePath, '\\')) != NULL)		
		cp = strrchr(filePath, '\\');
		if (cp != NULL) *cp = 0;

		/*if ((cp = wcsrchr(filePath, '\\')) != NULL)
		cp = wcsrchr(filePath, '\\');
		if (cp != NULL)
			*cp = 0;*/

		lstrcatA(filePath, "\\");

		lstrcpyA(logPath, filePath);
		lstrcatA(logPath, APP_NAME);
		lstrcatA(logPath, "_log");

		lstrcatA(filePath, CONFIG_FILE);


		if (!ifstream(filePath))
		{
			char lg[MAX_PATH];
			sprintf_s(lg, "File %s not found!", CONFIG_FILE);
			Log::Info(lg);
			lstrcpyA(log_path, logPath);
			return;
		}

		// Get log path from config file
		GetPrivateProfileStringA("LogPath", "log_path", "", log_path, MAX_PATH, filePath);
		if (log_path == "" || log_path == NULL || log_path[0] == '\0')
		{
			lstrcatA(log_path, logPath);
		}
	}

public:

	static void GetDate(char* buf, int buf_size)
	{
		time_t now = time(NULL);
		struct tm* ptm = new tm; // = localtime(&now);
		localtime_s(ptm, &now);
		//char buft[50] = { 0 };
		strftime(buf, buf_size, "%Y-%m-%d", ptm);
	}

	static void GetDateTime(char* buf, int buf_size)
	{
		time_t now = time(NULL);
		struct tm* ptm = new tm; // = localtime(&now);
		localtime_s(ptm, &now);
		//char buft[50] = { 0 };
		strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", ptm);
	}

	static void GetDirectoryOfModule(char* dirPath)
	{
		//char dirPath[MAX_PATH], *cp;
		char* cp;
		GetModuleFileNameA(NULL, dirPath, MAX_PATH);
		if ((cp = strrchr(dirPath, '\\')) != NULL)
			*cp = 0;
		//strcat_s(dirPath, "\\log");
	}

	static void CreateLogFile(char* filePath)
	{
		fstream fs;
		fs.open(filePath, fstream::in | fstream::out | fstream::app);
		if (!fs)
		{
			fs.open(filePath, fstream::in | fstream::out | fstream::trunc);
			//fs << "\n";
			fs.close();
		}
	}

	static void Info(const char* msg)
	{
		char filePath[MAX_PATH], buf[2048], bufd[15], buft[30];
		//GetDirectoryOfModule(filePath);
		char log_path[MAX_PATH];
		GetConfig(log_path);
		strcpy_s(filePath, log_path);
		//strcat_s(filePath, "\\log");
		//CreateDirectory(filePath, NULL);
		SHCreateDirectoryExA(NULL, filePath, NULL);
		GetDate(bufd, 15);
		strcat_s(filePath, "\\");
		strcat_s(filePath, bufd);
		strcat_s(filePath, ".log");

		GetDateTime(buft, 30);
		strcpy_s(buf, buft);
		strcat_s(buf, " - ");
		strcat_s(buf, msg);

		CreateLogFile(filePath);

		ifstream infile;
		infile.open(filePath);
		string str;
		infile.seekg(0, ios::end);
		str.reserve(infile.tellg());
		infile.seekg(0, ios::beg);
		str.assign((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
		infile.close();

		ofstream outfile;
		outfile.open(filePath);
		if (str.empty())
			outfile << buf;
		else
			outfile << str << "\n" << buf;
		outfile.close();
	}

};
