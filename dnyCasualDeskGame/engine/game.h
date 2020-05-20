#pragma once

/*
	Casual Desktop Game (dnyCasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2020 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include "scriptint.h"
#include "window.h"
#include "entity.h"
#include "console.h"
#include "menu.h"
#include "browser.h"
#include "workshop.h"
#include "logger.h"
#include <steam_api.h>

/* Game management component */
namespace Game {
	const std::wstring DesktopScreenshotFileName = L"res\\gamebg.png";

	bool bGameReady = false;
	RECT sWindowRect;
	DxWindow::CDxWindow* pDxWindow = nullptr;
	DxRenderer::CDxRenderer* pDxRenderer = nullptr;
	DxSound::CDxSound* pDxSound = nullptr;
	std::wstring wszBaseDirectory;
	Scripting::CScriptInt* pScriptingInt = nullptr;
	Entity::CToolMgr* pToolManager = nullptr;
	Console::CConsole* pConsole = nullptr;
	Menu::CMenu* pGameMenu = nullptr;
	Entity::Vector vCursorPos;
	Entity::game_keys_s sGameKeys;
	Workshop::CSteamDownload* pSteamDownloader = nullptr;
	std::vector<std::vector<std::wstring>> vToolBindings;
	Menu::CExitMenu* pExitMenu = nullptr;
	DxRenderer::HD3DSPRITE hExitMenuCursor = GFX_INVALID_SPRITE_ID;
	Logger::CLogger* pLogger = nullptr;
	Menu::WorkshopService_e eServiceType;
	bool bEnableScreenshotUpload;

	void AS_MessageCallback(const asSMessageInfo *msg, void *param);
	std::wstring GetToolFromBinding(const std::wstring& wszKey);
	void Release(void);
	void OnConfirmExitWindow(void);

	inline void FatalErrorMsg(const std::wstring& wszErrorMsg)
	{
		MessageBox(((pDxWindow) ? pDxWindow->GetHandle() : NULL), wszErrorMsg.c_str(), DNY_CDG_PRODUCT_NAME, MB_ICONERROR);
		bGameReady = false;
	}

	std::wstring GetBasePath(void)
	{
		//Get base path of program

		wchar_t wszFullName[MAX_PATH];
		
		//Get full program name
		if (!GetModuleFileName(0, wszFullName, sizeof(wszFullName)))
			return L"";

		//Remove file name
		for (size_t i = wcslen(wszFullName); i > 0; i--) {
			if (wszFullName[i] == '\\')
				break;

			wszFullName[i] = 0;
		}

		return wszFullName;
	}

	bool StoreBackgroundScreenshot(const std::wstring& wszOutputFileName)
	{
		//Create screenshot from Desktop and save to disk

		//Create temporary DX renderer object to Desktop window
		DxRenderer::CDxRenderer* pObject = new DxRenderer::CDxRenderer(GetDesktopWindow(), TRUE, sWindowRect.right, sWindowRect.bottom, 0, 0, 0, 0);
		if (!pObject)
			return false;

		//Save screenshot
		bool bResult = pObject->StoreScreenshotToDisk(wszBaseDirectory + wszOutputFileName);

		//Release object
		delete pObject;

		return bResult;
	}

	bool ShallConfirmOnExit(const std::wstring& wszInputFile)
	{
		//Load the indicator from the input file

		std::wifstream hFile;
		std::wstring wLine = L"1";
		hFile.open(wszInputFile, std::wifstream::in);
		if (hFile.is_open()) {
			std::getline(hFile, wLine);
			hFile.close();
		}

		int value = std::stoi(wLine);
		return (bool)value;
	}

	bool LoadServicesSettings(const std::wstring& wszInputFile)
	{
		//Load services configuration

		std::wifstream hFile;
		std::wstring wLine = L"1";
		hFile.open(wszInputFile, std::wifstream::in);
		if (hFile.is_open()) {
			while (!hFile.eof()) {
				std::getline(hFile, wLine);

				if (wLine.find(L"workshop") == 0) {
					std::wstring wszValue = wLine.substr(wLine.find(L" ") + 1);
					if (wszValue == L"steam") {
						Game::eServiceType = Menu::WorkshopService_e::WORKSHOP_STEAM;
					}
					else if (wszValue == L"own") {
						Game::eServiceType = Menu::WorkshopService_e::WORKSHOP_OWN;
					}
					else {
						Game::eServiceType = Menu::WorkshopService_e::WORKSHOP_OWN;
					}
				}
				else if (wLine.find(L"screenshot_upload") == 0) {
					std::wstring wszValue = wLine.substr(wLine.find(L" ") + 1);
					Game::bEnableScreenshotUpload = (bool)_wtoi(wszValue.c_str());
				}
			}

			hFile.close();

			return true;
		}

		return false;
	}

	bool StoreExitConfirmationIndicator(const std::wstring& wszOutputFile)
	{
		//Store indicator value to file

		if (Utils::FileExists(wszOutputFile)) {
			DeleteFileW(wszOutputFile.c_str());
		}

		std::wofstream hFile;
		hFile.open(wszOutputFile, std::wofstream::out);
		if (hFile.is_open()) {
			hFile << ((Menu::bShallConfirmOnExit) ? L"1" : L"0") << std::endl;
			hFile.close();

			return true;
		}

		return false;
	}

	//Window event handler component
	struct CWindowEvents : public DxWindow::CDxWindow::IWindowEvents {
		virtual void OnCreated(HWND hWnd)
		{
		}

		virtual LRESULT OnKeyEvent(int vKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
		{
			//Handle key input

			if (pToolManager) pToolManager->OnKeyEvent(vKey, bDown);
			
			if (vKey == sGameKeys.vkMenu) {
				if (bDown) {
					pGameMenu->Toggle();

					if ((pGameMenu->IsVisible()) && (pExitMenu->IsVisible())) {
						pExitMenu->Toggle();
					}
				}
			}
			else if (vKey == sGameKeys.vkScrollUp) {
				if (pGameMenu->IsVisible()) { if (bDown) pGameMenu->ScrollUp(); }
			}
			else if (vKey == sGameKeys.vkScrollDown) {
				if (pGameMenu->IsVisible()) { if (bDown) pGameMenu->ScrollDown(); }
			}
			else if (vKey == sGameKeys.vkConsole) {
				if (bDown) pConsole->Toggle();
			}
			else if (vKey == sGameKeys.vkTakeScreen) {
				if (bDown) {
					std::time_t curTime = std::time(nullptr);
					//std::tm* lcltime = std::localtime(&curTime);
					std::wstring wszTargetFileName = L"screenshots\\screenshot_" + std::to_wstring(curTime) + L".bmp";
					if (!pDxRenderer->StoreScreenshotToDisk(wszTargetFileName, D3DXIFF_BMP))
						Entity::APIFuncs::Print2("Failed to store screenshot: \"" + Utils::ConvertToAnsiString(wszTargetFileName) + "\"", Console::ConColor(250, 0, 0));
					else
						pGameMenu->RefreshCurrent();
				}
			}
			else if (vKey == sGameKeys.vkKey1) {
				std::wstring wszToolName = GetToolFromBinding(L"key1");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey2) {
				std::wstring wszToolName = GetToolFromBinding(L"key2");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey3) {
				std::wstring wszToolName = GetToolFromBinding(L"key3");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey4) {
				std::wstring wszToolName = GetToolFromBinding(L"key4");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey5) {
				std::wstring wszToolName = GetToolFromBinding(L"key5");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey6) {
				std::wstring wszToolName = GetToolFromBinding(L"key6");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey7) {
				std::wstring wszToolName = GetToolFromBinding(L"key7");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey8) {
				std::wstring wszToolName = GetToolFromBinding(L"key8");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey9) {
				std::wstring wszToolName = GetToolFromBinding(L"key9");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkKey0) {
				std::wstring wszToolName = GetToolFromBinding(L"key0");
				pToolManager->Select(wszToolName);
			}
			else if (vKey == sGameKeys.vkExit) {
				if (Menu::bShallConfirmOnExit) {
					if (bDown) {
						pExitMenu->Toggle();
						if ((pExitMenu->IsVisible()) && (pGameMenu->IsVisible())) {
							pGameMenu->Toggle();
						}
					}
				}
				else {
					Release();
					bGameReady = false;
				}
			}

			if ((bDown) && (pGameMenu)) {
				pGameMenu->OnKeyPressed(vKey);
			}
			return 0;
		}

		virtual LRESULT OnMouseEvent(int x, int y, int iMouseKey, bool bDown, bool bCtrlHeld, bool bShiftHeld, bool bAltHeld)
		{
			//Handle mouse input
			
			if (!iMouseKey)
				vCursorPos = Entity::Vector(x, y);

			if (pToolManager) pToolManager->OnMouseEvent(x, y, iMouseKey, bDown, bCtrlHeld, sGameKeys);
			if ((pGameMenu) && (pGameMenu->IsVisible())) pGameMenu->UpdateCursorPos(vCursorPos);

			if (iMouseKey == sGameKeys.vkTrigger) {
				if (pGameMenu->IsVisible()) {
					if (!bDown) pGameMenu->OnClick(vCursorPos, Menu::MKEY_LEFT);
				} else if (pExitMenu->IsVisible()) {
					if (!bDown) pExitMenu->OnClick(vCursorPos, Menu::MKEY_LEFT);
				} else {
					pToolManager->Trigger(bDown);
				}
			} else if (iMouseKey == sGameKeys.vkClean) {
				if (pGameMenu->IsVisible()) {
					if (!bDown) pGameMenu->OnClick(vCursorPos, Menu::MKEY_RIGHT);
				} else {
					if (!bDown) Entity::DoUserCleaning();
				}
			}

			return 0;
		}

		virtual void OnMouseWheel(short wDistance, bool bForward)
		{
			//Handle mouse wheel event

			if (pConsole->IsVisible()) {
				if (bForward) {
					pConsole->ScrollUp();
				} else {
					pConsole->ScrollDown();
				}

				return;
			}

			if (pGameMenu->IsVisible()) {
				if (bForward) {
					pGameMenu->ScrollUp();
				} else {
					pGameMenu->ScrollDown();
				}
			}
		}

		virtual void OnDraw(void)
		{
			//Render objects

			if (!bGameReady)
				return;

			if (!pDxRenderer->DrawBegin()) {
				pConsole->AddLine(L"CDxRenderer::DrawBegin() failed", Console::ConColor(150, 0, 0));
				//bGameReady = false;
			}

			if (pToolManager) {
				pToolManager->Draw(!pGameMenu->IsVisible());

				pToolManager->DrawOnTop((!pGameMenu->IsVisible()) && (!pExitMenu->IsVisible()));

				const std::vector<Entity::CScriptedEntity*> vSelEntityList = pToolManager->GetSelectionEntities();
				for (size_t i = 0; i < vSelEntityList.size(); i++) {
					if ((vSelEntityList[i] != nullptr) && (pToolManager->IsSelectionEntityValid(i))) {
						Entity::Vector vecPos = vSelEntityList[i]->GetPosition();
						Entity::Vector vecSize = vSelEntityList[i]->GetSelectionSize();

						pDxRenderer->DrawBox(vecPos[0] - 2, vecPos[1] - 2, vecSize[0] + 1, vecSize[1] + 1, 1, 200, 200, 200, 150);
					}
				}
			}

			if ((pGameMenu) && (pGameMenu->IsVisible())) {
				pGameMenu->Draw();
			}

			if ((pExitMenu) && (pExitMenu->IsVisible())) {
				pExitMenu->Draw();
				pDxRenderer->DrawSprite(hExitMenuCursor, vCursorPos[0], vCursorPos[1], 0, 0.0f);
			}

			if ((pConsole) && (pConsole->IsVisible())) {
				pConsole->Draw();
			}

			if (!pDxRenderer->DrawEnd()) {
				pConsole->AddLine(L"CDxRenderer::DrawEnd() failed", Console::ConColor(150, 0, 0));
				//bGameReady = false;
			}
		}

		virtual void OnRelease(void)
		{
			//Indicate program shutdown
			bGameReady = false;
		}
	} oDxWindowEvents;

	bool LoadKeys(const std::wstring& wszSrcFile)
	{
		//Load key values from file

		std::wifstream hFile;
		hFile.open(wszSrcFile, std::wifstream::in);
		if (!hFile.is_open())
			return false;

		wchar_t wszCurLine[MAX_PATH];

		while (!hFile.eof()) {
			hFile.getline(wszCurLine, sizeof(wszCurLine), '\n');

			std::vector<std::wstring> vItems = Utils::SplitW(wszCurLine, L" ");
			if (vItems.size() == 2) {
				if (vItems[0] == L"trigger") {
					sGameKeys.vkTrigger = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"clean") {
					sGameKeys.vkClean = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"menu") {
					sGameKeys.vkMenu = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"scrollup") {
					sGameKeys.vkScrollUp = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"scrolldown") {
					sGameKeys.vkScrollDown = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"teamselect") {
					sGameKeys.vkTeamSelect = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"console") {
					sGameKeys.vkConsole = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"screenshot") {
					sGameKeys.vkTakeScreen = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key1") {
					sGameKeys.vkKey1 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key2") {
					sGameKeys.vkKey2 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key3") {
					sGameKeys.vkKey3 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key4") {
					sGameKeys.vkKey4 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key5") {
					sGameKeys.vkKey5 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key6") {
					sGameKeys.vkKey6 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key7") {
					sGameKeys.vkKey7 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key8") {
					sGameKeys.vkKey8 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key9") {
					sGameKeys.vkKey9 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"key0") {
					sGameKeys.vkKey0 = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"exit") {
					sGameKeys.vkExit = _wtoi(vItems[1].c_str());
				}
			}
		}

		hFile.close();

		return true;
	}

	bool LoadToolBindings(const std::wstring& wszInput)
	{
		//Load bindings for tools

		std::wifstream hFile;
		hFile.open(wszInput, std::wifstream::in);
		if (hFile.is_open()) {
			wchar_t wszInput[MAX_PATH * 2];
			while (!hFile.eof()) {
				hFile.getline(wszInput, sizeof(wszInput), '\n');

				vToolBindings.push_back(Utils::SplitW(wszInput, L" "));
			}

			return true;
		}

		return false;
	}

	bool StoreToolBindings(void)
	{
		//Store tool bindings
		
		if (Utils::FileExists(wszBaseDirectory + L"res\\toolbindings.txt"))
			DeleteFile((wszBaseDirectory + L"res\\toolbindings.txt").c_str());

		std::wofstream hFile;
		hFile.open(wszBaseDirectory + L"res\\toolbindings.txt", std::wofstream::out);
		if (hFile.is_open()) {
			for (size_t i = 0; i < vToolBindings.size(); i++) {
				if (vToolBindings[i].size() == 2) {
					hFile << vToolBindings[i][0] + L" " + vToolBindings[i][1] << std::endl;
				}
			}

			return true;
		}

		return false;
	}

	std::wstring GetToolFromBinding(const std::wstring& wszKey)
	{
		//Get tool name from binding

		for (size_t i = 0; i < vToolBindings.size(); i++) {
			if (vToolBindings[i].size() == 2) {
				if (vToolBindings[i][0] == wszKey)
					return vToolBindings[i][1];
			}
		}

		return L"";
	}

	int GetToolBindingVirtualKeyCode(const std::wstring& wszKey)
	{
		//Get virtual key code of tool binding

		if (wszKey == L"key1") {
			return sGameKeys.vkKey1;
		} else if (wszKey == L"key2") {
			return sGameKeys.vkKey2;
		} else if (wszKey == L"key3") {
			return sGameKeys.vkKey3;
		} else if (wszKey == L"key4") {
			return sGameKeys.vkKey4;
		} else if (wszKey == L"key5") {
			return sGameKeys.vkKey5;
		} else if (wszKey == L"key6") {
			return sGameKeys.vkKey6;
		} else if (wszKey == L"key7") {
			return sGameKeys.vkKey7;
		} else if (wszKey == L"key8") {
			return sGameKeys.vkKey8;
		} else if (wszKey == L"key9") {
			return sGameKeys.vkKey9;
		} else if (wszKey == L"key0") {
			return sGameKeys.vkKey0;
		} else { return 0; }
	}

	bool LoadAllTools(void)
	{
		//Load all tools

		WIN32_FIND_DATA sFindData = { 0 };
		
		//Start file search
		HANDLE hFileSearch = FindFirstFile(L"tools\\*.*", &sFindData);
		if (hFileSearch == INVALID_HANDLE_VALUE)
			return false;

		//Iterate all files
		while (FindNextFile(hFileSearch, &sFindData)) {
			if (sFindData.cFileName[0] == '.')
				continue;

			//Check if file is a directory
			if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
				//Attempt to load tool from it
				if (pToolManager->LoadToolFromName(sFindData.cFileName, sGameKeys) != Entity::CToolMgr::InvalidToolHandle) {
					pConsole->AddLine(L"Tool \"" + std::wstring(sFindData.cFileName) + L"\": Ok", Console::ConColor(0, 150, 0));
				} else {
					pConsole->AddLine(L"Tool \"" + std::wstring(sFindData.cFileName) + L"\": Failure", Console::ConColor(150, 0, 0));
				}
			}
		}

		//Cleanup
		return FindClose(hFileSearch) == TRUE;
	}

	void OnHandleWorkshopItem(const std::wstring& wszItem)
	{
		//Fired for handling Workshop items

		Workshop::workshop_item_info_s sInfo;

		if (Workshop::LoadWorkshopInfoData(wszItem, sInfo)) {
			if (pToolManager->LoadToolFromPath(wszItem + L"\\" + sInfo.wszToolName + L".as", sGameKeys) != Entity::CToolMgr::InvalidToolHandle) {
				pConsole->AddLine(L"Tool \"" + sInfo.wszToolName + L"\": Ok", Console::ConColor(0, 150, 0));
			} else {
				pConsole->AddLine(L"Tool \"" + sInfo.wszToolName + L"\": Failure", Console::ConColor(150, 0, 0));
			}
		}
	}

	bool Initialize(const std::wstring& wszArgs)
	{
		//Initialize game
		
		if (bGameReady)
			return true;

		//Instantiate logger
		pLogger = new Logger::CLogger(L"logs");
		if (!pLogger)
			return false;

		pLogger->Log(Logger::LOG_INFO, L"Initializing game...");

		//Link with Steam

		if (SteamAPI_RestartAppIfNecessary(DNY_CDG_STEAM_APPID)) {
			pLogger->Log(Logger::LOG_INFO, L"Restarting App");
			return true;
		}

		if (!SteamAPI_Init()) {
			pLogger->Log(Logger::LOG_ERROR, L"SteamAPI_Init() returned false");
			return false;
		}
		
		pLogger->Log(Logger::LOG_INFO, L"Connected with Steam");

		Sleep(2000);

		//Get base path
		
		wszBaseDirectory = GetBasePath();
		if (!wszBaseDirectory.length()) {
			pLogger->Log(Logger::LOG_ERROR, L"Could not retrieve base path: " + std::to_wstring(GetLastError()));
			return false;
		}

		Entity::SetBasePath(wszBaseDirectory);

		pLogger->Log(Logger::LOG_INFO, L"Base path: " + wszBaseDirectory);

		//Get desktop resolution
		if (!GetWindowRect(GetDesktopWindow(), &sWindowRect)) {
			pLogger->Log(Logger::LOG_ERROR, L"GetWindowRect() failed: " + std::to_wstring(GetLastError()));
			return false;
		}

		if (!wszArgs.length()) {
			//Create screenshot from Desktop
			if (!StoreBackgroundScreenshot(DesktopScreenshotFileName)) {
				pLogger->Log(Logger::LOG_ERROR, L"StoreBackgroundScreenshot() failed: " + std::to_wstring(GetLastError()));
				return false;
			}
		} else {
			//Copy arg file to destination
			if (!CopyFile(wszArgs.c_str(), (wszBaseDirectory + DesktopScreenshotFileName).c_str(), FALSE)) {
				pLogger->Log(Logger::LOG_ERROR, L"CopyFile() failed: " + std::to_wstring(GetLastError()));
				return false;
			}
		}

		pLogger->Log(Logger::LOG_INFO, L"Stored background screenshot");

		// Initialize media components

		pDxWindow = new DxWindow::CDxWindow(DNY_CDG_PRODUCT_NAME, sWindowRect.right, sWindowRect.bottom, &oDxWindowEvents);
		if (!pDxWindow) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate DxWindow::CDxWindow: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized window component");

		pDxRenderer = new DxRenderer::CDxRenderer(pDxWindow->GetHandle(), false, sWindowRect.right, sWindowRect.bottom, 255, 255, 255, 0);
		if (!pDxRenderer) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate DxRenderer::CDxRenderer: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized renderer");

		pDxSound = new DxSound::CDxSound(pDxWindow->GetHandle());
		if (!pDxSound) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate DxSound::CDxSound: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized sound");

		//Load global volume
		if (!pDxSound->LoadGlobalVolume(wszBaseDirectory + L"res\\volume.txt")) {
			pLogger->Log(Logger::LOG_WARNING, L"LoadGlobalVolume() failed: " + std::to_wstring(GetLastError()));
		}

		//Instantiate console
		pConsole = new Console::CConsole(pDxRenderer, sWindowRect.right, sWindowRect.bottom / 2, CON_DEFAULT_MAXHISTORY, Console::ConColor(100, 100, 100));
		if (!pConsole) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate Console::CConsole: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized console");

		#define CON_INFO_WRAPPER_LINE L"=============================================================="
		pConsole->AddLine(CON_INFO_WRAPPER_LINE);
		pConsole->AddLine(L"* " DNY_CDG_PRODUCT_TEXT " (" DNY_CDG_PRODUCT_NAME ") v" DNY_CDG_PRODUCT_VERSION);
		pConsole->AddLine("* Developed by " DNY_CDG_PRODUCT_AUTHOR " (" DNY_CDG_PRODUCT_CONTACT ")");
		pConsole->AddLine("* Licenced under " DNY_CDG_PRODUCT_LICENCE);
		pConsole->AddLine(CON_INFO_WRAPPER_LINE);

		//Initialize scripting
		pScriptingInt = new Scripting::CScriptInt(Utils::ConvertToAnsiString(wszBaseDirectory + L"tools\\"), &AS_MessageCallback);
		if (!pScriptingInt) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate Scripting::CScriptInt: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized scripting interface");

		//Initialize entity environment
		pToolManager = Entity::Initialize(pDxRenderer, pDxSound, pScriptingInt, pConsole);
		if (!pToolManager) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to initialize entity environment: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized entity environment");

		//Load tool bindings
		if (!LoadToolBindings(wszBaseDirectory + L"res\\toolbindings.txt")) {
			pLogger->Log(Logger::LOG_ERROR, L"LoadToolBindings() failed: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Loaded all tool bindings");

		//Load service configuration
		if (!LoadServicesSettings(L"res\\services.txt")) {
			Menu::SetServices(Menu::WorkshopService_e::WORKSHOP_STEAM, false);

			pConsole->AddLine(L"Failed to load services settings from disk, using default values", Console::ConColor(150, 150, 0));
			pLogger->Log(Logger::LOG_WARNING, L"Menu::LoadServicesSettings() failed: " + std::to_wstring(GetLastError()));
		}
		else {
			Menu::SetServices(Game::eServiceType, Game::bEnableScreenshotUpload);
		}

		//Instantiate menu
		pGameMenu = new Menu::CMenu(pDxRenderer, pDxSound, pToolManager, Entity::Vector(pDxRenderer->GetWindowWidth(), pDxRenderer->GetWindowHeight()), &vToolBindings);
		if (!pGameMenu) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate Menu:::CMenu: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized game menu");
		
		//Load key config
		if (!LoadKeys(L"res\\keys.txt")) {
			pLogger->Log(Logger::LOG_ERROR, L"LoadKeys() failed: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Loaded keys");

		//Query host info
		if (!Browser::InputHostInfo(L"res\\hostinfo.txt")) {
			pConsole->AddLine(L"Failed to query host info", Console::ConColor(150, 150, 0));
			pLogger->Log(Logger::LOG_WARNING, L"Browser::InputHostInfo() failed: " + std::to_wstring(GetLastError()));
		}

		pLogger->Log(Logger::LOG_INFO, L"Using host: " + Browser::SettingHostURL);
		
		//Query menu data
		if (!Menu::SetupMenu(L"res\\menudef.txt")) {
			pConsole->AddLine(L"Failed to query menu data from disk, using default values", Console::ConColor(150, 150, 0));
			pLogger->Log(Logger::LOG_WARNING, L"Menu::SetupMenu() failed: " + std::to_wstring(GetLastError()));
		}

		//Set exit confirmation indicator
		Menu::bShallConfirmOnExit = ShallConfirmOnExit(L"res\\exitconfirm.txt");

		//Pass gamekeys data
		Menu::SetGameKeys(sGameKeys);

		//Instantiate exit menu
		pExitMenu = new Menu::CExitMenu(pDxRenderer, &OnConfirmExitWindow);
		if (!pExitMenu) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate Menu:::CExitMenu: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized exit confirmation menu");

		//Load exit menu cursor
		hExitMenuCursor = pDxRenderer->LoadSprite(L"res\\menucursor.png", 1, 16, 16, 1, false);
		if (hExitMenuCursor == GFX_INVALID_SPRITE_ID) {
			pLogger->Log(Logger::LOG_ERROR, L"DxRenderer::CDxRenderer::LoadSprite() returned GFX_INVALID_SPRITE_ID for menu cursor: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Loaded menu cursor");

		//Instantiate Steam Workshop downloader object
		pSteamDownloader = new Workshop::CSteamDownload(&OnHandleWorkshopItem);
		if (!pSteamDownloader) {
			pLogger->Log(Logger::LOG_ERROR, L"Failed to instantiate Workshop::CSteamDownload: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Initialized Steam Workshop interface");

		//Process subscribed Workshop items
		if (!pSteamDownloader->ProcessSubscribedItems()) {
			pLogger->Log(Logger::LOG_WARNING, L"Workshop::CSteamDownload::ProcessSubscribedItems() failed: " + std::to_wstring(GetLastError()));
		} else {
			pLogger->Log(Logger::LOG_INFO, L"Retrieved Workshop items");
		}

		//Load all tools
		if (!LoadAllTools()) {
			pLogger->Log(Logger::LOG_ERROR, L"LoadAllTools() failed: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Loaded all tools");

		//Set background image
		if (!pDxRenderer->SetBackgroundPicture(wszBaseDirectory + DesktopScreenshotFileName)) {
			pLogger->Log(Logger::LOG_ERROR, L"DxRenderer::CDxRenderer::SetBackgroundPicture() failed: " + std::to_wstring(GetLastError()));
			return false;
		}

		pLogger->Log(Logger::LOG_INFO, L"Successfully set background image");
		
		pGameMenu->SetCategory(0);
		pGameMenu->Toggle();

		pLogger->Log(Logger::LOG_INFO, L"All set up and running. Have fun playing!");

		//Return set indicator
		return bGameReady = true;
	}

	void Process(void)
	{
		//Process game

		while (bGameReady) {
			if (pDxWindow) pDxWindow->Process();
			if (pToolManager) pToolManager->Process();
			if (pGameMenu) pGameMenu->Process();

			SteamAPI_RunCallbacks();

			Sleep(1);
		}
	}

	void Release(void)
	{
		//Release resources

		if (!bGameReady)
			return;

		//Log information
		pLogger->Log(Logger::LOG_INFO, L"Shutting down");

		//Store global volume
		pDxSound->StoreGlobalVolume(wszBaseDirectory + L"res\\volume.txt");

		//Store exit confirmation value
		StoreExitConfirmationIndicator(wszBaseDirectory + L"res\\exitconfirm.txt");

		//Store tool bindings
		StoreToolBindings();

		//Release objects
		#define _delete(p) if (p) { delete p; p = nullptr; }
		_delete(pDxRenderer);
		_delete(pDxSound);
		_delete(pDxWindow);
		_delete(pScriptingInt);
		_delete(pToolManager);
		_delete(pConsole);
		_delete(pGameMenu);
		_delete(pSteamDownloader);
		_delete(pExitMenu);
		_delete(pLogger);

		//Delete screenshot
		if (Utils::FileExists(wszBaseDirectory + DesktopScreenshotFileName))
			DeleteFile((wszBaseDirectory + DesktopScreenshotFileName).c_str());

		//Unlink with Steam
		SteamAPI_Shutdown();

		//Clear indicator
		bGameReady = false;
	}

	void OnConfirmExitWindow(void)
	{
		//Called when clicking the "Yes"-button

		Release();
	}

	void AS_MessageCallback(const asSMessageInfo *msg, void *param)
	{
		//Callback function for AngelScript message outputs

		Console::ConColor sColor(200, 200, 200);

		std::string szMsgType("Info");
		if (msg->type == asMSGTYPE_WARNING) {
			szMsgType = "Warning";
			sColor = Console::ConColor(150, 150, 0);
		} else if (msg->type == asMSGTYPE_ERROR) {
			szMsgType = "Error";
			sColor = Console::ConColor(250, 0, 0);
		}

		std::string szErrMsg = "[AngelScript][" + szMsgType + "] " + std::string(msg->section) + " (" + std::to_string(msg->row) + ":" + std::to_string(msg->col) + "): " + msg->message + "\n";

		Entity::APIFuncs::Print2(szErrMsg.c_str(), sColor);
	}

	void HandleToolUpload(const std::wstring& wszArgs)
	{
		//Handle tool upload process

		std::wstring wszItem = wszArgs.substr(wszArgs.find(L"-workshop_item_update") + wcslen(L"-workshop_item_update") + 1);
		
		if (MessageBox(0, (L"Attempting to upload tool: " + wszItem + L"\n\nNote: This might take a while. You will be informed when the process has finished.\n\nDo you want to continue?").c_str(), DNY_CDG_PRODUCT_NAME, MB_ICONINFORMATION | MB_YESNO) == IDNO)
			return;

		SteamAPI_Init();

		Workshop::CSteamUpload* pWorkShop = new Workshop::CSteamUpload();

		pWorkShop->InitWorkshopUpdate(wszItem);

		while (!pWorkShop->IsFinished()) {
			SteamAPI_RunCallbacks();
			Sleep(1);
		}

		(pWorkShop->GetResult()) ? MessageBox(0, L"Your Workshop item has been stored!", DNY_CDG_PRODUCT_NAME, MB_ICONINFORMATION) : MessageBox(0, L"Failed to store Workshop item", DNY_CDG_PRODUCT_NAME, MB_ICONEXCLAMATION);

		delete pWorkShop;

		SteamAPI_Shutdown();
	}
}