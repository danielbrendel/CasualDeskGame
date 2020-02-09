#include "game.h"

/*
	Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2020 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel

	License: see LICENSE.txt
*/

namespace Main {
	inline void Initialize(const std::wstring& wszArgs)
	{
		if (wszArgs.find(L"-workshop_item_update") != -1) {
			Game::HandleToolUpload(wszArgs);

			return;
		} 

		if (!Game::Initialize(wszArgs))
			MessageBox(0, L"Game::Initialize() failed", DNY_CDG_PRODUCT_NAME, MB_ICONERROR);
	}
	inline void Process(void) { Game::Process(); }
	inline void Release(void) { Game::Release(); }
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Main::Initialize(Utils::ConvertToWideString(lpCmdLine));
	Main::Process();
	Main::Release();

	return EXIT_SUCCESS;
}