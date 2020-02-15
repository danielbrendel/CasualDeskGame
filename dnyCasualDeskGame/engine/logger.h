#pragma once

/*
	Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2020 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	License: see LICENSE.txt
*/

#include "shared.h"
#include <iomanip>
#include <ctime> 

/* Logger component */
namespace Logger {
	const int LOG_INFO = 1;
	const int LOG_DEBUG = 2;
	const int LOG_WARNING = 3;
	const int LOG_ERROR = 4;

	class CLogger {
	private:
		std::wofstream m_hFile;

		std::wstring GetCurrentTimeString(void)
		{
			//Get current time and date as string

			std::time_t t = std::time(nullptr);
			wchar_t wcsDate[MAX_PATH];
			if (std::wcsftime(wcsDate, sizeof(wcsDate), L"%d-%m-%Y %H:%M:%S", std::localtime(&t))) {
				return wcsDate;
			}

			return L"";
		}

		void WriteLine(const std::wstring& wszLine)
		{
			//Write line to stream

			std::wstring wszFullLine = std::wstring(wszLine + L"\n");

			this->m_hFile.write(wszFullLine.data(), wszFullLine.length());
		}
	public:
		CLogger(const std::wstring& wszFile)
		{
			//Open file for writing 

			this->m_hFile.open(wszFile, std::ofstream::out);
			if (this->m_hFile.is_open()) {
				this->WriteLine(DNY_CDG_PRODUCT_NAME L" v" DNY_CDG_PRODUCT_VERSION L" developed by " DNY_CDG_PRODUCT_AUTHOR L" (" DNY_CDG_PRODUCT_CONTACT L")\n");
				this->WriteLine(L"Logfile stream opened at " + this->GetCurrentTimeString() + L"\n");
			}
		}
		~CLogger()
		{
			//Close file

			if (this->m_hFile.is_open()) {
				this->m_hFile.close();
			}
		}

		inline bool IsReady(void) const
		{
			//Indicate if the log file has been opened

			return this->m_hFile.is_open();
		}

		bool Log(const int iLogLevel, const std::wstring& wszContent)
		{
			//Log to stream

			if (!this->IsReady())
				return false;

			std::wstring wszLogLevel = L"";

			switch (iLogLevel) {
			case LOG_INFO:
				wszLogLevel = L"Info";
				break;
			case LOG_DEBUG:
				wszLogLevel = L"Debug";
				break;
			case LOG_WARNING:
				wszLogLevel = L"Warning";
				break;
			case LOG_ERROR:
				wszLogLevel = L"Error";
				break;
			default:
				wszLogLevel = L"Unknown";
				break;
			}

			this->WriteLine(L"[" + this->GetCurrentTimeString() + L"][" + wszLogLevel + L"] " + wszContent);

			return true;
		}
	};
}