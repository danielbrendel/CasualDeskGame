#pragma once

/*
	Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2020 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>yahoo<dot>com
	GitHub: https://github.com/danielbrendel

	License: see LICENSE.txt
*/

#include "shared.h"
#include "utils.h"
#include <WinInet.h>

/* Browser environment */
namespace Browser {
	std::wstring SettingHostURL;

	bool InputHostInfo(const std::wstring& wszInputFile)
	{
		//Read settings data from file

		std::wifstream hFile;
		hFile.open(wszInputFile, std::wifstream::in);
		if (!hFile.is_open())
			return false;

		wchar_t wszCurLine[MAX_PATH];

		while (!hFile.eof()) {
			hFile.getline(wszCurLine, sizeof(wszCurLine), '\n');

			std::vector<std::wstring> vItems = Utils::SplitW(wszCurLine, L" ");
			if (vItems.size() == 2) {
				if (vItems[0] == L"Host") {
					SettingHostURL = vItems[1];
				}
			}
		}

		hFile.close();

		return true;
	}

	class CBrowser { //Browser object
	public:
		struct toollink_s {
			std::wstring wszName;
			std::wstring wszVersion;
			std::wstring wszBaseLink;
			std::wstring wszImageLink;
		};
		struct dljob_s {
			std::wstring wszAction;
			std::wstring wszItem;
		};
	private:
		HINTERNET m_hInternet;
		std::vector<toollink_s> m_vToolLinks;
		std::vector<dljob_s> m_vJobList;

		bool IsResourceAvailable(HINTERNET hOpenedResource)
		{
			//Indicate whether resource is available or not

			DWORD dwStatusCode;
			DWORD dwBufferSize = sizeof(DWORD);
			DWORD dwIndex = 0;

			return (HttpQueryInfo(hOpenedResource, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwBufferSize, &dwIndex)) && (dwStatusCode == HTTP_STATUS_OK);
		}
	public:
		CBrowser()
		{
			//Instantiate WinInet session
			this->m_hInternet = InternetOpen(DNY_CDG_PRODUCT_NAME, INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
		}
		~CBrowser()
		{
			//Close session
			InternetCloseHandle(this->m_hInternet);
			this->m_hInternet = NULL;
		}

		bool FetchToolList(const std::wstring& wszHostURL)
		{
			//Fetch all available tools from host

			if (!wszHostURL.length())
				return false;

			bool bResult = false;

			if (this->m_hInternet) {
				HINTERNET hURL = InternetOpenUrl(this->m_hInternet, wszHostURL.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD, 0); //Open connection to resource
				if (hURL) {
					if (this->IsResourceAvailable(hURL)) { //Check if requested resource is available
						byte ucCurrentBuffer[MAX_PATH] = { 0 };
						DWORD dwBytesRead = sizeof(ucCurrentBuffer);
						std::wstring wszEntireList = L"";

						while ((InternetReadFile(hURL, ucCurrentBuffer, sizeof(ucCurrentBuffer) - 1, &dwBytesRead)) && (dwBytesRead)) { //Read current buffer as long as not end of resource is reached
							ucCurrentBuffer[dwBytesRead] = 0;
							wszEntireList += Utils::ConvertToWideString((const char*)ucCurrentBuffer);
						}

						InternetCloseHandle(hURL); //Close resource connection

						bResult = true;

						//Parse each item and create an entry
						std::vector<std::wstring> vItems = Utils::SplitW(wszEntireList, L"\n");
						for (size_t i = 0; i < vItems.size(); i++) {
							std::vector<std::wstring> vLinks = Utils::SplitW(vItems[i], L" ");
							if (vLinks.size() == 4) {
								toollink_s sToolLink;
								sToolLink.wszName = vLinks[0];
								sToolLink.wszVersion = vLinks[1];
								sToolLink.wszBaseLink = vLinks[2];
								sToolLink.wszImageLink = vLinks[3];
								this->m_vToolLinks.push_back(sToolLink);
							}
						}
					}
				}
			}

			return bResult;
		}

		bool FetchJobList(const std::wstring& wszHostURL)
		{
			//Fetch all required files of a tool from host

			if (!wszHostURL.length())
				return false;

			bool bResult = false;

			if (this->m_hInternet) {
				HINTERNET hURL = InternetOpenUrl(this->m_hInternet, wszHostURL.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD, 0); //Open connection to resource
				if (hURL) {
					if (this->IsResourceAvailable(hURL)) { //Check if requested resource is available
						byte ucCurrentBuffer[MAX_PATH] = { 0 };
						DWORD dwBytesRead = sizeof(ucCurrentBuffer);
						std::wstring wszEntireList = L"";

						while ((InternetReadFile(hURL, ucCurrentBuffer, sizeof(ucCurrentBuffer) - 1, &dwBytesRead)) && (dwBytesRead)) { //Read current buffer as long as not end of resource is reached
							ucCurrentBuffer[dwBytesRead] = 0;
							wszEntireList += Utils::ConvertToWideString((const char*)ucCurrentBuffer);
						}

						InternetCloseHandle(hURL); //Close resource connection

						bResult = true;

						//Parse each job info and add to list
						std::vector<std::wstring> vJobs = Utils::SplitW(wszEntireList, L"\n");
						for (size_t i = 0; i < vJobs.size(); i++) {
							std::vector<std::wstring> vItems = Utils::SplitW(vJobs[i], L" ");
							if (vItems.size() >= 2) {
								dljob_s sDlJob;
								sDlJob.wszAction = vItems[0];
								sDlJob.wszItem = vItems[1];
								this->m_vJobList.push_back(sDlJob);
							}
						}
					}
				}
			}

			return bResult;
		}

		bool DownloadResource(const std::wstring& wszResourceURL, const std::wstring& wszTargetFile, bool bOverwriteIfExists = true)
		{
			//Download resource from URL

			if (!wszResourceURL.length())
				return false;

			bool bResult = false;

			if (this->m_hInternet) {
				HINTERNET hURL = InternetOpenUrl(this->m_hInternet, wszResourceURL.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD, 0); //Open connection to resource
				if (hURL) {
					if (this->IsResourceAvailable(hURL)) { //Check if requested resource is available
						byte ucCurrentBuffer[2048];
						DWORD dwBytesRead = sizeof(ucCurrentBuffer);
					
						//Delete file if already exists if requested
						if (Utils::FileExists(wszTargetFile)) {
							if (bOverwriteIfExists) {
								DeleteFile(wszTargetFile.c_str());
							} else {
								InternetCloseHandle(hURL); //Close resource connection
								return false;
							}
						}

						HANDLE hFile = CreateFile(wszTargetFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
						if (hFile != INVALID_HANDLE_VALUE) {
							bResult = true; //Indicate success

							while ((InternetReadFile(hURL, ucCurrentBuffer, sizeof(ucCurrentBuffer), &dwBytesRead)) && (dwBytesRead)) { //Read current buffer as long as not end of resource is reached
								//Write current buffer to file
								DWORD dwBytesWritten = 0;
								if ((!WriteFile(hFile, ucCurrentBuffer, dwBytesRead, &dwBytesWritten, nullptr) || (dwBytesWritten != dwBytesRead))) {
									bResult = false; //Indicate failure
									break;
								}
							}

							CloseHandle(hFile); //Close file handle
						}
					}

					InternetCloseHandle(hURL); //Close resource connection
				}
			}

			return bResult;
		}

		//Getters
		size_t GetToolLinkCount(void) { return this->m_vToolLinks.size(); }
		const toollink_s* GetToolLinkItem(const size_t uiItemId) { if (uiItemId < this->m_vToolLinks.size()) return &this->m_vToolLinks[uiItemId]; return nullptr; }
		size_t GetJobListCount(void) { return this->m_vJobList.size(); }
		const dljob_s* GetJobListItem(const size_t uiItemId) { if (uiItemId < this->m_vJobList.size()) return &this->m_vJobList[uiItemId]; return nullptr; }
	};

	class CImageUploader { //Image uploader
	#define CIU_BOUNDARY "CImageUploader"
	private:
		std::string m_szHost;
		std::string m_szResource;
		std::string m_szImageName;
		std::string m_szImageFile;
		std::string m_szSteamName;
		std::string m_szFormDataName;

		byte* LoadImageFile(size_t& uiImageSizeOut)
		{
			//Load image file into memory

			//Open stream to image file
			HANDLE hFile = CreateFileA(this->m_szImageFile.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile == INVALID_HANDLE_VALUE)
				return nullptr;

			//Obtain file size
			DWORD dwFileSize = GetFileSize(hFile, nullptr);
			if (!dwFileSize) {
				CloseHandle(hFile);
				return nullptr;
			}

			//Allocate buffer
			byte* pBuffer = new byte[dwFileSize];
			if (!pBuffer) {
				CloseHandle(hFile);
				return nullptr;
			}

			DWORD dwBytesRead;
			//Read file content into memory
			if ((!ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, nullptr)) || (dwBytesRead != dwFileSize)) {
				CloseHandle(hFile);
				delete[] pBuffer;
				return nullptr;
			}

			//Close stream to file
			CloseHandle(hFile);

			//Store file size
			uiImageSizeOut = (size_t)dwFileSize;

			return pBuffer;
		}

		std::string GetRequestHeader(void)
		{
			//Create request header

			std::string szResult = "--" CIU_BOUNDARY "\r\n"
				"Content-Type: application/octet-stream\r\n"
				"Content-Disposition: form-data; name=\"" + this->m_szFormDataName + "\"; steamname=\"" + this->m_szSteamName + "\" filename=\"" + this->m_szImageName + "\"\r\n\r\n";

			return szResult;
		}

		std::string GetRequestFooter(void)
		{
			//Create request footer

			std::string szResult = "\r\n--" CIU_BOUNDARY "\r\n\r\n";

			return szResult;
		}

		bool CanProcessUpload(void)
		{
			//Check whether upload process can be started

			return (this->m_szResource.length()) && (this->m_szFormDataName.length()) && (this->m_szImageName.length()) && (this->m_szImageFile.length());
		}

		bool SendRequest(const void* pData, const size_t uiDataLen)
		{
			//Send data to server

			if ((!pData) || (!uiDataLen))
				return false;

			//Instantiate WinInet session
			HINTERNET hInet = InternetOpenA(CIU_BOUNDARY, 0, nullptr, nullptr, 0);
			if (!hInet)
				return false;

			//Connect to resource
			HINTERNET hConnection = InternetConnectA(hInet, this->m_szHost.c_str(), INTERNET_DEFAULT_HTTP_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
			if (!hConnection) {
				InternetCloseHandle(hInet);
				return false;
			}

			//Init request
			HINTERNET hRequest = HttpOpenRequestA(hConnection, "POST", this->m_szResource.c_str(), "HTTP/1.1", nullptr, nullptr, 0, 0);
			if (!hRequest) {
				InternetCloseHandle(hConnection);
				InternetCloseHandle(hInet);
				return false;
			}

			//Add additional reqest info
			std::string szAddRequest = "Content-Type: multipart/form-data; boundary=" CIU_BOUNDARY "\r\n\r\n";
			if (!HttpAddRequestHeadersA(hRequest, szAddRequest.c_str(), (DWORD)szAddRequest.length(), HTTP_ADDREQ_FLAG_ADD)) {
				InternetCloseHandle(hRequest);
				InternetCloseHandle(hConnection);
				InternetCloseHandle(hInet);
				return false;
			}

			//Send request
			bool bResult = (HttpSendRequestA(hRequest, nullptr, 0, (LPVOID)pData, (DWORD)uiDataLen) == TRUE);

			//Close handles
			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hInet);

			return bResult;
		}
	public:
		CImageUploader() {}
		CImageUploader(const std::string& szHost, const std::string& szRes, const std::string& szImgName, const std::string& szImgFile, const std::string& szSteamName, const std::string& szFrmName) : m_szHost(szHost), m_szResource(szRes), m_szImageFile(szImgFile), m_szImageName(szImgName), m_szSteamName(szSteamName), m_szFormDataName(szFrmName) {}
		~CImageUploader() {}

		//Actions
		bool Upload(void)
		{
			//Upload file to destination

			//Check if strings are ready
			if (!this->CanProcessUpload())
				return false;

			size_t uiImageSize = 0;
			//Load image file into memory
			byte* pData = this->LoadImageFile(uiImageSize);
			if (!pData)
				return false;

			//Obtain header and footer
			std::string szHeader = this->GetRequestHeader();
			std::string szFooter = this->GetRequestFooter();

			//Create request buffer
			char* pEntireRequest = new char[uiImageSize + szHeader.length() + szFooter.length() + 1];
			if (!pEntireRequest) {
				delete[] pData;
				return false;
			}

			//Copy memory to buffer
			memcpy(pEntireRequest, szHeader.data(), szHeader.length());
			memcpy((void*)((DWORD_PTR)pEntireRequest + (DWORD_PTR)szHeader.length()), pData, uiImageSize);
			memcpy((void*)((DWORD_PTR)pEntireRequest + (DWORD_PTR)szHeader.length() + (DWORD_PTR)uiImageSize), szFooter.data(), szFooter.length());

			//Attempt to send request
			bool bResult = this->SendRequest(pEntireRequest, szHeader.length() + szFooter.length() + uiImageSize);

			//Free memory
			delete[] pData;
			delete[] pEntireRequest;

			return bResult;
		}

		//Setters
		void SetHost(const std::string& szHost) { this->m_szHost = szHost; }
		void SetResource(const std::string& szRes) { this->m_szResource = szRes; }
		void SetImageFile(const std::string& szImage) { this->m_szImageFile = szImage; }
		void SetImageName(const std::string& szName) { this->m_szImageName = szName; }
		void SetSteamName(const std::string& szName) { this->m_szSteamName = szName; }
		void SetFormDataName(const std::string& szName) { this->m_szFormDataName = szName; }

		//Getters
		const std::string& GetHost(void) const { return this->m_szHost; }
		const std::string& GetResource(void) const { return this->m_szResource; }
		const std::string& GetImageFile(void) const { return this->m_szImageFile; }
		const std::string& GetImageName(void) const { return this->m_szImageName; }
		const std::string& GetSteamName(void) const { return this->m_szSteamName; }
		const std::string& GetDataName(void) const { return this->m_szFormDataName; }
	};
}
