#pragma once

/*
	Casual Desktop Game (dnyCasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2020 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	License: see LICENSE.txt
*/

#include "shared.h"
#include "renderer.h"
#include "sound.h"
#include "entity.h"
#include "browser.h"
#include <steam_api.h>

/* Menu environment */
namespace Menu {
	enum MouseKey_e { MKEY_LEFT, MKEY_MID, MKEY_RIGHT };
	enum WorkshopService_e { WORKSHOP_STEAM, WORKSHOP_OWN };

	int SettingMenuBgImageWidth = 800;
	int SettingMenuBgImageHeight = 500;
	size_t SettingMenuPreviewItemDisplayCount = 8;
	size_t SettingMenuPreviewItemLineCount = 2;
	int SettingMenuCloseX = 765;
	int SettingMenuCloseY = 14;
	int SettingMenuCloseW = 26;
	int SettingMenuCloseH = 26;
	int SettingMenuNavX = 12;
	int SettingMenuNavY = 57;
	int SettingMenuNavW = 167;
	int SettingMenuNavH = 447;
	int SettingMenuBodyX = 200;
	int SettingMenuBodyY = 71;
	int SettingMenuBodyW = 567;
	int SettingMenuBodyH = 424;
	int SettingMenuScrollUpX = 770;
	int SettingMenuScrollUpY = 57;
	int SettingMenuScrollDownX = 770;
	int SettingMenuScrollDownY = 471;
	int SettingMenuScrollWH = 21;
	int SettingMenuScrollCubeWH = 20;
	int SettingMenuPreviewItemResW = 195;
	int SettingMenuPreviewItemResH = 90;
	int SettingMenuPreviewItemGapHor = 15;
	int SettingMenuPreviewItemGapVert = 1;
	int SettingMenuScrollbarLength = 394;
	DxRenderer::d3dfont_s* DefaultFontHandle = nullptr;
	Entity::game_keys_s sGameKeys;
	bool bShallConfirmOnExit = true;
	WorkshopService_e workshopService;
	bool bEnableScreenshotUpload = false;

	void SetGameKeys(const Entity::game_keys_s& keys) { sGameKeys = keys; }
	void SetServices(WorkshopService_e service, bool enableScreenshotUpload) { Menu::workshopService = service; Menu::bEnableScreenshotUpload = enableScreenshotUpload; }

	bool SetupMenu(const std::wstring& wszInputFile)
	{
		//Query menu settings from input file

		std::wifstream hFile;
		hFile.open(wszInputFile, std::wifstream::in);
		if (!hFile.is_open())
			return false;

		wchar_t wszCurLine[MAX_PATH];

		while (!hFile.eof()) {
			hFile.getline(wszCurLine, sizeof(wszCurLine), '\n');

			std::vector<std::wstring> vItems = Utils::SplitW(wszCurLine, L" ");
			if (vItems.size() == 2) {
				if (vItems[0] == L"Width") {
					SettingMenuBgImageWidth = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"Height") {
					SettingMenuBgImageHeight = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"ImageDisplayCount") {
					SettingMenuPreviewItemDisplayCount = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"ImageLineCount") {
					SettingMenuPreviewItemLineCount = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemCloseX") {
					SettingMenuCloseX = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemCloseY") {
					SettingMenuCloseY = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemCloseW") {
					SettingMenuCloseW = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemCloseH") {
					SettingMenuCloseH = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemNavX") {
					SettingMenuNavX = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemNavY") {
					SettingMenuNavY = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemNavW") {
					SettingMenuNavW = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemNavH") {
					SettingMenuNavH = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemBodyX") {
					SettingMenuBodyX = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemBodyY") {
					SettingMenuBodyY = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemBodyW") {
					SettingMenuBodyW = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemBodyH") {
					SettingMenuBodyH = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemResW") {
					SettingMenuPreviewItemResW = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemResH") {
					SettingMenuPreviewItemResH = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemGapHor") {
					SettingMenuPreviewItemGapHor = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"BodyItemGapVert") {
					SettingMenuPreviewItemGapVert = _wtoi(vItems[1].c_str());
				} else if (vItems[0] == L"ScrollBarLength") {
					SettingMenuScrollbarLength = _wtoi(vItems[1].c_str());
				}
			}
		}

		hFile.close();

		return true;
	}

	static const wchar_t DefaultFontName[] = { L"Arial" };
	static const int DefaultFontSizeW = 7;
	static const int DefaultFontSizeH = 15;

	class CMenu {
	#define NAV_TEXT_SPACING 10
	#define NAV_ITEM_YPOS(item) ((int)item * (DefaultFontSizeH * 2))
	#define NAV_TEXT_AREA_HEIGHT (DefaultFontSizeH * 2)
	private:
		class IMenuItemContainerBase { //Menu item container base interface
		public:
			struct resultinfo_s {
				std::wstring wszText;
				Entity::Color sColor;
				Entity::CTimer tmrTimer;
			};
		private:
			std::wstring m_wszText;
			Entity::Vector m_vecCursorPos;
			std::vector<resultinfo_s> m_vInfos;
		protected:
			virtual void PushText(const std::wstring& wszInfo, const Entity::Color& rColor)
			{
				//Add temporary text to list

				resultinfo_s sInfo;
				sInfo.wszText = wszInfo;
				sInfo.sColor = rColor;
				sInfo.tmrTimer.SetDelay(2500);
				sInfo.tmrTimer.Reset();
				sInfo.tmrTimer.SetActive(true);
				this->m_vInfos.push_back(sInfo);
			}
			virtual void PushInfo(const std::wstring& wszInfo)
			{
				//Push info

				return this->PushText(wszInfo, Entity::Color(177, 240, 167, 150));
			}
			virtual void PushError(const std::wstring& wszInfo)
			{
				//Push error

				return this->PushText(wszInfo, Entity::Color(150, 0, 0, 150));
			}
			virtual void ProcessResultInfos(void)
			{
				//Process result info entries

				for (size_t i = 0; i < this->m_vInfos.size(); i++) {
					this->m_vInfos[i].tmrTimer.Update();
					if (this->m_vInfos[i].tmrTimer.Elapsed()) {
						this->m_vInfos.erase(this->m_vInfos.begin() + i);
					}
				}
			}

			//Internal getters
			virtual const Entity::Vector& GetCursorPos(void) const { return this->m_vecCursorPos; }
			virtual size_t GetResultInfoStringCount(void) const { return this->m_vInfos.size(); }
			virtual const resultinfo_s& GetResultInfoData(const size_t uiId) const { static resultinfo_s sEmpty; if (uiId < this->m_vInfos.size()) return this->m_vInfos[uiId]; return sEmpty; }
		public:
			IMenuItemContainerBase() {}
			IMenuItemContainerBase(const std::wstring& wszText) : m_wszText(wszText) { }
			virtual ~IMenuItemContainerBase() {}

			//Handlers
			virtual void OnShow(void) = 0; //Called if item is shown
			virtual void OnProcess(void) { this->ProcessResultInfos(); } //Called for processing stuff
			virtual void OnDraw(const Entity::Vector& vBodyStart) = 0; //Called for drawing
			virtual void OnDrawAdditions(const Entity::Vector& vBodyStart) {} //Useful for additional drawings
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey) = 0; //Called when clicking inside the subitem range
			virtual void OnKeyPressed(int vKey) {} //Whenever a key is pressed
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection) = 0; //Called when selecting an item

			//Getters
			virtual const std::wstring& GetText(void) const { return this->m_wszText; }
			virtual bool IsValid(void) const = 0;
			virtual bool CustomDrawing(void) const = 0;
			virtual size_t GetScrollIndex(void) const = 0;
			virtual Entity::Vector GetSubItemPreviewRes(void) const = 0;
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) = 0;
			virtual size_t GetSubItemCount(void) const = 0;

			//Setters
			virtual void SetCursorPos(const Entity::Vector& vPos) { this->m_vecCursorPos = vPos; }

			//Scrolling
			virtual void ScrollDown(void) = 0;
			virtual void ScrollUp(void) = 0;
		};
		class IMenuItemContextMenu { //Context menu handler
		protected:
			static const size_t InvalidItemId = (size_t)-1;
		private:
			static const int DistBetweenText = 3;
			static const int TextSideSpacing = 10;
			struct contextmenuitem_s {
				size_t uiItemId;
				std::wstring wszText;
				bool bSelectable;
			};
			std::vector<contextmenuitem_s> m_vItems;
			bool m_bVisible;
			Entity::Vector m_vecShowPos;
			Entity::Vector m_vecFontDims;
			Entity::Color m_sBgColor;
			Entity::Color m_sTextColor;
			Entity::Color m_sSelColor;
			DxRenderer::CDxRenderer* m_pRenderer;

			void GetContextMenuDims(Entity::Vector& rvOut)
			{
				//Calculate context menu item resolution

				//Calculate width

				//Find longest item text
				size_t uiMostStringLength = 0;
				for (size_t i = 0; i < this->m_vItems.size(); i++) {
					if (this->m_vItems[i].wszText.length() > uiMostStringLength) {
						uiMostStringLength = this->m_vItems[i].wszText.length();
					}
				}

				//Calculate width
				rvOut[0] = ((int)uiMostStringLength * this->m_vecFontDims[0]) + TextSideSpacing;

				//Calculate height
				rvOut[1] = (int)this->m_vItems.size() * (this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText);
			}

			size_t FindItemByCoords(const Entity::Vector& vecCoords)
			{
				//Find item by coordinates

				//Get menu dimensions
				Entity::Vector vecMenuDims;
				this->GetContextMenuDims(vecMenuDims);

				for (size_t i = 0; i < this->m_vItems.size(); i++) { //Search in list
					Entity::Vector vecCurrentItemPos;

					//Ignore unselectable items
					if (!this->m_vItems[i].bSelectable) continue;

					//Calculate position of current item
					vecCurrentItemPos[0] = this->m_vecShowPos[0];
					vecCurrentItemPos[1] = this->m_vecShowPos[1] + ((int)i * (this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText));

					//Check if coords are located inside current item range
					if ((vecCoords[0] >= vecCurrentItemPos[0]) && (vecCoords[0] <= this->m_vecShowPos[0] + vecMenuDims[0]) && (vecCoords[1] >= vecCurrentItemPos[1]) && (vecCoords[1] <= vecCurrentItemPos[1] + this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText))
						return i;
				}

				return IMenuItemContextMenu::InvalidItemId;
			}

			size_t FindItemByIdent(const size_t uiIdentifier)
			{
				//Find item by ident

				for (size_t i = 0; i < this->m_vItems.size(); i++) {
					if (!this->m_vItems[i].bSelectable) continue;

					if (this->m_vItems[i].uiItemId == uiIdentifier)
						return i;
				}

				return IMenuItemContextMenu::InvalidItemId;
			}
		protected:
			virtual bool AddContextMenuItem(const std::wstring& wszText, const size_t uiIdentifier)
			{
				//Add item to context menu
				
				//Only unselectable items can have a non-specifying identifier
				if ((uiIdentifier == IMenuItemContextMenu::InvalidItemId) && (wszText.length() > 0))
					return false;
				
				//Check if ident is already in use
				if (this->FindItemByIdent(uiIdentifier) != IMenuItemContextMenu::InvalidItemId)
					return false;
				
				//Setup data struct
				contextmenuitem_s sItem;
				sItem.wszText = wszText;
				sItem.uiItemId = uiIdentifier;
				sItem.bSelectable = wszText.length() > 0;

				//Add to list
				this->m_vItems.push_back(sItem);
				
				return true;
			}

			virtual bool AddContextMenuItemDelimiter(void)
			{
				//Add delimiter item

				return this->AddContextMenuItem(L"", IMenuItemContextMenu::InvalidItemId);
			}

			virtual bool RemoveContextMenuItem(const size_t uiIdentifier)
			{
				//Remove item

				//Find item
				size_t uiListId = this->FindItemByIdent(uiIdentifier);
				if (uiListId == IMenuItemContextMenu::InvalidItemId)
					return false;

				//Remove item from list
				this->m_vItems.erase(this->m_vItems.begin() + uiListId);

				return true;
			}

			virtual bool UdateContextMenuItem(const size_t uiIdentifier, const std::wstring& wszNewText)
			{
				//Update item

				//Find item
				size_t uiListId = this->FindItemByIdent(uiIdentifier);
				if (uiListId == IMenuItemContextMenu::InvalidItemId)
					return false;

				//Update item string
				this->m_vItems[uiListId].wszText = wszNewText;

				return true;
			}

			virtual void ClearContextMenuItems(void)
			{
				//Clear item list

				this->m_vItems.clear();
			}

			virtual void ShowContextMenu(const Entity::Vector& vPos)
			{
				//Show context menu at given position

				this->m_vecShowPos = vPos;
				this->m_bVisible = true;
			}

			virtual void Click(const Entity::Vector& vMousePos)
			{
				//Handle click event
				
				//Hide menu
				this->m_bVisible = false;

				//Check if click is outside of range and close menu incase
				Entity::Vector vecRange;
				this->GetContextMenuDims(vecRange);
				if ((vMousePos[0] < this->m_vecShowPos[0]) || (vMousePos[0] > this->m_vecShowPos[0] + vecRange[0]) || (vMousePos[1] < this->m_vecShowPos[1]) || (vMousePos[1] > this->m_vecShowPos[1] + vecRange[1])) {
					return;
				}

				//Check if an item has been clicked
				size_t uiMenuItem = this->FindItemByCoords(vMousePos);
				if (uiMenuItem != IMenuItemContextMenu::InvalidItemId) {
					//Inform event method
					this->HandleContextMenuItem(this->m_vItems[uiMenuItem].uiItemId);
				}
			}

			virtual void Draw(const Entity::Vector& vCurCursorPos)
			{
				//Draw context menu if visible

				if (!this->m_bVisible)
					return;

				//Get menu dims
				Entity::Vector vecDims;
				this->GetContextMenuDims(vecDims);
				
				//Draw background
				this->m_pRenderer->DrawFilledBox(this->m_vecShowPos[0], this->m_vecShowPos[1], vecDims[0], vecDims[1], this->m_sBgColor.r, this->m_sBgColor.g, this->m_sBgColor.b, this->m_sBgColor.a);

				//Draw selected item if any
				size_t uiCurSelItem = this->FindItemByCoords(vCurCursorPos);
				if (uiCurSelItem != IMenuItemContextMenu::InvalidItemId) {
					this->m_pRenderer->DrawFilledBox(this->m_vecShowPos[0], this->m_vecShowPos[1] + ((int)uiCurSelItem * (this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText)), vecDims[0], this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText, this->m_sSelColor.r, this->m_sSelColor.g, this->m_sSelColor.b, this->m_sSelColor.a);
				}

				//Draw items
				for (size_t i = 0; i < this->m_vItems.size(); i++) {
					//Draw text or delimiter line
					if (this->m_vItems[i].bSelectable) {
						this->m_pRenderer->DrawString(DefaultFontHandle, this->m_vItems[i].wszText, this->m_vecShowPos[0], this->m_vecShowPos[1] + ((int)i * (this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText)), this->m_sTextColor.r, this->m_sTextColor.g, this->m_sTextColor.b, this->m_sTextColor.a);
					} else {
						this->m_pRenderer->DrawFilledBox(this->m_vecShowPos[0], this->m_vecShowPos[1] + ((this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText) / 2) + ((int)i * (this->m_vecFontDims[1] + IMenuItemContextMenu::DistBetweenText)), vecDims[0], 2, 0, 0, 0, 150);
					}
				}
			}

			//Called when a context menu item has been selected
			virtual void HandleContextMenuItem(const size_t uiIdentifier) = 0;

			//Getter
			virtual bool IsContextMenuVisible(void) const { return this->m_bVisible; }

			//Interface
			virtual void SetDrawingInterface(DxRenderer::CDxRenderer* pRenderer) { this->m_pRenderer = pRenderer; }
			virtual void SetFontDims(const Entity::Vector& dims) { this->m_vecFontDims = dims; }
			virtual void SetColors(const Entity::Color& bg, const Entity::Color& sel, const Entity::Color& txt) { this->m_sBgColor = bg; this->m_sSelColor = sel; this->m_sTextColor = txt; }
		public:
			IMenuItemContextMenu() : m_bVisible(false), m_pRenderer(nullptr) {}
			IMenuItemContextMenu(DxRenderer::CDxRenderer* pRenderer, const Entity::Vector& dims, const Entity::Color& bg, const Entity::Color& sel, const Entity::Color& txt) { this->SetDrawingInterface(pRenderer); this->SetFontDims(dims); this->SetColors(bg, sel, txt); }
			~IMenuItemContextMenu() { this->m_vItems.clear(); }
		};
		template <typename TSubItemType> class IMenuItemContainer : public IMenuItemContainerBase { //Base menu item container manager
		private:
			std::vector<TSubItemType*> m_vSubItems;
			size_t m_uiScrollIndex;
		protected:
			virtual size_t SubItemCount(void) const { return this->m_vSubItems.size(); } //Return amount of subitems
			virtual TSubItemType* GetSubItem(const size_t uiItemId) { if (uiItemId < this->m_vSubItems.size()) return this->m_vSubItems[uiItemId]; return nullptr; } //Return subitem instance pointer

			virtual bool AddSubItem(TSubItemType* pSubItemType)
			{
				//Add new subitem

				if (!pSubItemType)
					return false;

				this->m_vSubItems.push_back(pSubItemType);

				return true;
			}

			virtual bool RemoveSubItem(const size_t uiItemId)
			{
				//Remove subitem

				if (uiItemId >= this->m_vSubItems.size())
					return false;

				delete this->m_vSubItems[uiItemId];
				this->m_vSubItems.erase(this->m_vSubItems.begin() + uiItemId);

				return true;
			}

			void Release(void)
			{
				//Release resources

				for (size_t i = 0; i < this->m_vSubItems.size(); i++) {
					delete this->m_vSubItems[i];
				}

				this->m_vSubItems.clear();
			}
		public:
			IMenuItemContainer() : IMenuItemContainerBase(), m_uiScrollIndex(0) {}
			IMenuItemContainer(const std::wstring& wszText) : IMenuItemContainerBase(wszText), m_uiScrollIndex(0) { }
			virtual ~IMenuItemContainer() { this->Release(); }

			//Items
			virtual size_t GetSubItemCount(void) const { return this->m_vSubItems.size(); }

			//Scrolling
			virtual void ScrollDown(void) { if (this->SubItemCount() > SettingMenuPreviewItemDisplayCount) { if (this->m_uiScrollIndex < this->SubItemCount() - SettingMenuPreviewItemDisplayCount) this->m_uiScrollIndex++; } }
			virtual void ScrollUp(void) { if (this->m_uiScrollIndex > 0) this->m_uiScrollIndex--; }
			virtual size_t GetScrollIndex(void) const { return this->m_uiScrollIndex; }
			virtual void SetScrollIndex(const size_t uiIndex) { this->m_uiScrollIndex = uiIndex; }
		};

		struct SubItemCatNews { byte ucNone; };
		class CMenuCatNews : public IMenuItemContainer<SubItemCatNews> { //Handler for news
		private:
			DxRenderer::CDxRenderer* m_pRenderer;
			DxRenderer::HD3DSPRITE m_hSprNewsImage;
		public:
			CMenuCatNews() : IMenuItemContainer<SubItemCatNews>(), m_pRenderer(nullptr), m_hSprNewsImage(GFX_INVALID_SPRITE_ID) {}
			CMenuCatNews(const std::wstring& wszText, DxRenderer::CDxRenderer* pRenderer) : IMenuItemContainer<SubItemCatNews>(wszText), m_pRenderer(pRenderer), m_hSprNewsImage(GFX_INVALID_SPRITE_ID){}

			virtual void OnShow(void)
			{
				//Download and load news image

				Browser::CBrowser* pBrowser = new Browser::CBrowser();
				if (pBrowser) {
					if (Browser::SettingHostURL.length()) {
						if (!pBrowser->DownloadResource(L"http://" + Browser::SettingHostURL + L"/cdg/news.png", L"cache\\news.png")) {
							Entity::APIFuncs::Print2("Failed to obtain news content: " + std::to_string(GetLastError()), Console::ConColor(150, 150, 0));
						}

						this->m_hSprNewsImage = this->m_pRenderer->LoadSprite(L"cache\\news.png", 1, SettingMenuBodyW, SettingMenuBodyH, 1, true);
					}

					delete pBrowser;
				}
			}
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection)
			{
				bToggleAfterSelection = false;
			}
			virtual void OnDraw(const Entity::Vector& vBodyStart)
			{
				//Draw news image

				if (this->m_hSprNewsImage != GFX_INVALID_SPRITE_ID) {
					this->m_pRenderer->DrawSprite(this->m_hSprNewsImage, vBodyStart[0] - 20, vBodyStart[1] - 20, 0, 0.0f, -1, -1, 0.0f, 0.0f, false, 0, 0, 0, 0);
				} else {
					this->m_pRenderer->DrawString(DefaultFontHandle, L"Welcome to " DNY_CDG_PRODUCT_TEXT " v" DNY_CDG_PRODUCT_VERSION, vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
				}
			}
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey) {}

			virtual bool CustomDrawing(void) const { return true; }
			virtual bool IsValid(void) const { return this->m_pRenderer != nullptr; }
			virtual Entity::Vector GetSubItemPreviewRes(void) const { return Entity::Vector(SettingMenuPreviewItemResW, SettingMenuPreviewItemResH); }
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) { return nullptr; }
		};

		struct SubItemCatTools { DxRenderer::HD3DSPRITE hPreviewSprite; size_t hTool; std::wstring wszName; std::wstring wszCategory; SubItemCatTools(DxRenderer::HD3DSPRITE spr, size_t ht, const std::wstring& nm, const std::wstring& ct) : hPreviewSprite(spr), hTool(ht), wszName(nm), wszCategory(ct) {} };
		class CMenuDropDown {
		public:
			struct DropDownItem_s {
				DropDownItem_s() : uiItemId((size_t)-1) {}
				std::wstring wszText;
				size_t uiItemId;
			};
		private:
			static const int DropDownBorderSize = 1;
			static const int DropDownDistToBorder = 2;
			static const int DropDownButtonCubeSize = DefaultFontSizeH;
			DxRenderer::CDxRenderer* m_pRenderer;
			bool m_bActive;
			Entity::Vector m_vecPosition;
			int m_iWidth;
			std::vector<DropDownItem_s> m_vItems;
			size_t m_uiSelection;
		public:
			CMenuDropDown() : m_bActive(false), m_uiSelection((size_t)-1) {}
			CMenuDropDown(DxRenderer::CDxRenderer* pRenderer, const Entity::Vector& vecPos, int iWidth) : m_bActive(false), m_pRenderer(pRenderer), m_vecPosition(vecPos), m_iWidth(iWidth), m_uiSelection((size_t)-1) {}
			~CMenuDropDown() { this->m_vItems.clear(); }

			bool AddItem(const std::wstring& wszText, const size_t uiItemId)
			{
				//Add menu item

				if (!wszText.length())
					return false;

				DropDownItem_s sItemData;
				sItemData.uiItemId = uiItemId;
				sItemData.wszText = wszText;

				this->m_vItems.push_back(sItemData);

				return true;
			}

			//Actions
			
			void Draw(void)
			{
				//Draw component

				//Draw window
				this->m_pRenderer->DrawBox(this->m_vecPosition[0], this->m_vecPosition[1], this->m_iWidth, DefaultFontSizeH + ((DropDownBorderSize + DropDownDistToBorder) * 2), DropDownBorderSize, 67, 67, 70, 150);
				this->m_pRenderer->DrawFilledBox(this->m_vecPosition[0] + DropDownBorderSize, this->m_vecPosition[1] + DropDownBorderSize, this->m_iWidth - DropDownBorderSize, DefaultFontSizeH + DropDownDistToBorder * 2 + DropDownBorderSize, 51, 51, 55, 150);
				this->m_pRenderer->DrawFilledBox(this->m_vecPosition[0] + (this->m_iWidth - DropDownButtonCubeSize - DropDownBorderSize - DropDownDistToBorder), this->m_vecPosition[1] + DropDownBorderSize + DropDownDistToBorder, DropDownButtonCubeSize, DropDownButtonCubeSize, 32, 32, 35, 150);
				
				//Draw selection text
				if (this->m_uiSelection != (size_t)-1)
					this->m_pRenderer->DrawString(DefaultFontHandle, this->m_vItems[this->m_uiSelection].wszText, this->m_vecPosition[0] + DropDownBorderSize + DropDownDistToBorder, this->m_vecPosition[1] + DropDownBorderSize + DropDownDistToBorder, 255, 255, 255, 150);
				else
					this->m_pRenderer->DrawString(DefaultFontHandle, L"Select...", this->m_vecPosition[0] + DropDownBorderSize + DropDownDistToBorder, this->m_vecPosition[1] + DropDownBorderSize + DropDownDistToBorder, 255, 255, 255, 150);

				//Draw item content if active
				if (this->m_bActive) {
					//Draw background
					this->m_pRenderer->DrawFilledBox(this->m_vecPosition[0] + DropDownBorderSize, this->m_vecPosition[1] + + DefaultFontSizeH + DropDownBorderSize * 2 + DropDownDistToBorder * 2, this->m_iWidth, (int)this->m_vItems.size() * DefaultFontSizeH, 27, 27, 28, 150);

					//Draw items
					for (size_t i = 0; i < this->m_vItems.size(); i++) {
						this->m_pRenderer->DrawString(DefaultFontHandle, this->m_vItems[i].wszText, this->m_vecPosition[0] + DropDownBorderSize + DropDownDistToBorder, this->m_vecPosition[1] + DefaultFontSizeH + DropDownBorderSize * 2 + DropDownDistToBorder * 2 + ((int)i * DefaultFontSizeH), 255, 255, 255, 150);
					}
				}
			}
			void Draw(const Entity::Vector& vDrawPos)
			{
				//Draw on new position

				this->m_vecPosition = vDrawPos;
				this->Draw();
			}
			void OnClick(const Entity::Vector& vCursorPos)
			{
				//Handle click event

				//Handle toggle event
				if ((vCursorPos[0] > this->m_vecPosition[0]) && (vCursorPos[0] < this->m_vecPosition[0] + this->m_iWidth) && (vCursorPos[1] > this->m_vecPosition[1]) && (vCursorPos[1] < this->m_vecPosition[1] + DefaultFontSizeH)) {
					this->Toggle();
					return;
				}

				//Handle item click events
				if (this->m_bActive) {
					this->m_uiSelection = (size_t)-1;

					for (size_t i = 0; i < this->m_vItems.size(); i++) {
						if ((vCursorPos[0] > this->m_vecPosition[0]) && (vCursorPos[0] < this->m_vecPosition[0] + this->m_iWidth) && (vCursorPos[1] > this->m_vecPosition[1] + DefaultFontSizeH + DropDownBorderSize * 2 + DropDownDistToBorder * 2 + ((int)i * DefaultFontSizeH)) && (vCursorPos[1] < this->m_vecPosition[1] + (DefaultFontSizeH + DropDownBorderSize * 2 + DropDownDistToBorder * 2) + ((int)i * DefaultFontSizeH) + DefaultFontSizeH)) {
							this->m_uiSelection = i;
							this->Toggle();
							return;
						}
					}
				}
			}
			void Toggle(void) { this->m_bActive = !this->m_bActive; }
			void Clear(void) { this->m_vItems.clear(); }

			//Getters
			inline const size_t Selection(void) const { return this->m_uiSelection; }
			inline const DropDownItem_s& Item(const size_t uiId) const { static DropDownItem_s sEmpty; if (uiId < this->Count()) return this->m_vItems[uiId]; return sEmpty; }
			inline const size_t Count(void) const { return this->m_vItems.size(); }
			inline const bool IsActive(void) const { return this->m_bActive; }
			bool ItemExists(const std::wstring& wszItem)
			{
				//Check whether item does already exist

				for (size_t i = 0; i < this->m_vItems.size(); i++) {
					if (this->m_vItems[i].wszText == wszItem)
						return true;
				}

				return false;
			}
		};
		class CMenuCatTools : public IMenuItemContainer<SubItemCatTools>, IMenuItemContextMenu { //Handler for tools
		private:
			static const size_t ContextMenuItemSelect = 0;
			static const size_t ContextMenuItemRemove = 1;

			Entity::CToolMgr* m_pToolMgr;
			DxRenderer::CDxRenderer* m_pRenderer;
			size_t m_uiContextMenuItem;
			CMenuDropDown* m_pDropDownMenu;
			size_t m_uiLastSelection;
			Entity::Vector m_vecBodyStart;

			void LoadAllItems(void)
			{
				//Load all items

				this->Release();

				for (size_t i = 0; i < this->m_pToolMgr->Count(); i++) {
					DxRenderer::HD3DSPRITE hSprite = this->m_pToolMgr->GetPreviewImage(i);
					if (hSprite != GFX_INVALID_SPRITE_ID) {
						const Entity::CToolMgr::gt_list_item_s& rToolInfo = this->m_pToolMgr->GetBaseInfo(i);
						this->AddSubItem(new SubItemCatTools(hSprite, i, Utils::ConvertToWideString(rToolInfo.szScript), Utils::ConvertToWideString(rToolInfo.sInfo.szCategory)));
					}
				}

				this->SetScrollIndex(0);
			}

			void LoadItemsOfCategory(const std::wstring& wszCategory)
			{
				//Load items of category

				this->Release();

				for (size_t i = 0; i < this->m_pToolMgr->Count(); i++) {
					DxRenderer::HD3DSPRITE hSprite = this->m_pToolMgr->GetPreviewImage(i);
					if (hSprite != GFX_INVALID_SPRITE_ID) {
						const Entity::CToolMgr::gt_list_item_s& rToolInfo = this->m_pToolMgr->GetBaseInfo(i);

						if (rToolInfo.sInfo.szCategory == Utils::ConvertToAnsiString(wszCategory)) {
							this->AddSubItem(new SubItemCatTools(hSprite, i, Utils::ConvertToWideString(rToolInfo.szScript), Utils::ConvertToWideString(rToolInfo.sInfo.szCategory)));
						}
					}
				}

				this->SetScrollIndex(0);
			}

			Entity::CToolMgr::HTOOL GetToolIdFromCatItem(const size_t uiSelectedItem)
			{
				//Get HTOOL from menu item from category

				SubItemCatTools* pSubItem = this->GetSubItem(uiSelectedItem);
				if (!pSubItem)
					return -1;

				return pSubItem->hTool;
			}
		public:
			CMenuCatTools() : IMenuItemContainer<SubItemCatTools>(), m_pToolMgr(nullptr), m_pRenderer(nullptr), m_pDropDownMenu(nullptr) {}
			CMenuCatTools(const std::wstring& wszText, Entity::CToolMgr* pToolMgr, DxRenderer::CDxRenderer* pRenderer) : IMenuItemContainer<SubItemCatTools>(wszText), m_pToolMgr(pToolMgr), m_pRenderer(pRenderer)
			{
				IMenuItemContextMenu::SetDrawingInterface(pRenderer);
				IMenuItemContextMenu::SetFontDims(Entity::Vector(DefaultFontSizeW, DefaultFontSizeH));
				IMenuItemContextMenu::SetColors(Entity::Color(0, 162, 232, 150), Entity::Color(231, 231, 232, 150), Entity::Color(0, 0, 0, 150));
				IMenuItemContextMenu::AddContextMenuItem(L"Select", CMenuCatTools::ContextMenuItemSelect);
				IMenuItemContextMenu::AddContextMenuItem(L"Remove", CMenuCatTools::ContextMenuItemRemove);

				this->m_pDropDownMenu = new CMenuDropDown(this->m_pRenderer, Entity::Vector(0, 0), 134);
				this->m_uiLastSelection = (size_t)-1;
			}
			~CMenuCatTools()
			{
				if (this->m_pDropDownMenu) {
					delete this->m_pDropDownMenu;
					this->m_pDropDownMenu = nullptr;
				}
			}

			virtual void OnShow(void)
			{
				//Re-initialize tool data

				this->Release();

				this->m_pDropDownMenu->Clear();

				this->m_pDropDownMenu->AddItem(L"All", (size_t)-1);

				for (size_t i = 0; i < this->m_pToolMgr->Count(); i++) {
					DxRenderer::HD3DSPRITE hSprite = this->m_pToolMgr->GetPreviewImage(i);
					if (hSprite != GFX_INVALID_SPRITE_ID) {
						const Entity::CToolMgr::gt_list_item_s& rToolInfo = this->m_pToolMgr->GetBaseInfo(i);

						if (!this->m_pDropDownMenu->ItemExists(Utils::ConvertToWideString(rToolInfo.sInfo.szCategory))) {
							this->m_pDropDownMenu->AddItem(Utils::ConvertToWideString(rToolInfo.sInfo.szCategory), i);
						}
					}
				}

				if (this->m_pDropDownMenu->Selection() == (size_t)-1) {
					this->LoadAllItems();
				}
			}
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection)
			{
				//Select tool or delete tool content from disk

				bToggleAfterSelection = false;

				if (IMenuItemContextMenu::IsContextMenuVisible())
					return;

				if (eMouseKey == MKEY_LEFT) {
					size_t uiToolId = this->GetToolIdFromCatItem(uiItemId);
					this->m_pToolMgr->Select(uiToolId);
					if (uiToolId != -1) {
						bToggleAfterSelection = true;
					}
				} else if (eMouseKey == MKEY_RIGHT) {
					this->m_uiContextMenuItem = uiItemId;
					IMenuItemContextMenu::ShowContextMenu(this->GetCursorPos());
				}
			}
			virtual void HandleContextMenuItem(const size_t uiIdentifier)
			{
				//Handle clicked items

				switch (uiIdentifier) {
				case CMenuCatTools::ContextMenuItemSelect: {
					this->m_pToolMgr->Select(this->GetToolIdFromCatItem(this->m_uiContextMenuItem));
					break;
				}
				case CMenuCatTools::ContextMenuItemRemove: {
					SubItemCatTools* pSubItem = this->GetSubItem(this->m_uiContextMenuItem);
					if (pSubItem) {
						if (Utils::RemoveEntireDirectory(L"tools\\" + pSubItem->wszName)) {
							this->PushInfo(L"Tool " + pSubItem->wszName + L" has been removed!");
							this->OnShow();
						}
					}
					break;
				}
				default:
					break;
				}
			}
			virtual void OnDraw(const Entity::Vector& vBodyStart) {}
			virtual void OnDrawAdditions(const Entity::Vector& vBodyStart)
			{
				//Draw info if no items are loaded and also all result info strings

				this->m_vecBodyStart = vBodyStart;

				IMenuItemContextMenu::Draw(this->GetCursorPos());

				if (this->m_pDropDownMenu) {
					this->m_pDropDownMenu->Draw(Entity::Vector(vBodyStart[0] - 180, vBodyStart[1] + 200));
				}

				if (!this->GetSubItemCount()) {
					this->m_pRenderer->DrawString(DefaultFontHandle, L"There are no tools loaded.", vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
				}

				for (size_t i = 0; i < this->GetResultInfoStringCount(); i++) {
					#define TOOLS_INFO_BOX_SPACING 10
					const resultinfo_s& rInfo = this->GetResultInfoData(i);
					this->m_pRenderer->DrawFilledBox(vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)rInfo.wszText.length() / 2 * DefaultFontSizeW)) - TOOLS_INFO_BOX_SPACING / 2, vBodyStart[1] + SettingMenuBgImageHeight - 150 - TOOLS_INFO_BOX_SPACING / 2, (int)rInfo.wszText.length() * DefaultFontSizeW + TOOLS_INFO_BOX_SPACING, DefaultFontSizeH + TOOLS_INFO_BOX_SPACING, rInfo.sColor.r, rInfo.sColor.g, rInfo.sColor.b, rInfo.sColor.a);
					this->m_pRenderer->DrawString(DefaultFontHandle, rInfo.wszText, vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)rInfo.wszText.length() / 2 * DefaultFontSizeW)), vBodyStart[1] + SettingMenuBgImageHeight - 150, 0, 0, 0, 150);
				}
			}
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey)
			{
				if (IMenuItemContextMenu::IsContextMenuVisible()) {
					if (eMouseKey == MKEY_LEFT)
						IMenuItemContextMenu::Click(vCursorPos);

					return;
				}

				this->m_pDropDownMenu->OnClick(vCursorPos);
				if (this->m_pDropDownMenu->Selection() != this->m_uiLastSelection) {
					this->m_uiLastSelection = this->m_pDropDownMenu->Selection();

					if (this->m_uiLastSelection == 0)
						this->LoadAllItems();
					else
						this->LoadItemsOfCategory(this->m_pDropDownMenu->Item(this->m_pDropDownMenu->Selection()).wszText);
				}
			}
			
			virtual bool CustomDrawing(void) const { return false; }
			virtual bool IsValid(void) const { return this->m_pToolMgr != nullptr; }
			virtual Entity::Vector GetSubItemPreviewRes(void) const { return Entity::Vector(SettingMenuPreviewItemResW, SettingMenuPreviewItemResH); }
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) { SubItemCatTools* pSubItem = this->GetSubItem(uiItemId); if (pSubItem) return pSubItem->hPreviewSprite; return nullptr; }
		};

		struct SubItemCatBrowse { DxRenderer::HD3DSPRITE hPreviewSprite; std::wstring wszName; SubItemCatBrowse() {} SubItemCatBrowse(DxRenderer::HD3DSPRITE spr, const std::wstring& sz) : hPreviewSprite(spr), wszName(sz) {} };
		class CMenuCatBrowser : public IMenuItemContainer<SubItemCatBrowse>, IMenuItemContextMenu { //Handler for browsing online tools
		private:
			static const int ContextMenuItemDownload = 0;

			Entity::CToolMgr* m_pToolMgr;
			DxRenderer::CDxRenderer* m_pRenderer;
			DxSound::CDxSound* m_pSound;
			Browser::CBrowser* m_pBrowser;
			bool m_bFetchingList;
			size_t m_uiFetchingItemId;
			bool m_bDownloadingItem;
			SubItemCatBrowse m_sCurSubItem;
			size_t m_uiJobListItemId;
			bool m_bNetworkError;
			size_t m_uiSelectedItem;

			void InitDownload(const size_t uiItemId)
			{
				if ((this->m_bFetchingList) || (this->m_bDownloadingItem) || (!Browser::SettingHostURL.length()))
					return;

				Entity::APIFuncs::Print("> Starting download of selected tool...");

				this->m_pBrowser = new Browser::CBrowser();
				if (!this->m_pBrowser) {
					Entity::APIFuncs::Print2("> Failed to instantiate Browser object", Console::ConColor(150, 0, 0));
					return;
				}

				const SubItemCatBrowse* pSubItem = this->GetSubItem(uiItemId);
				if (!pSubItem) {
					Entity::APIFuncs::Print2("> Failed to get subitem data from item " + std::to_string(uiItemId), Console::ConColor(150, 0, 0));
					return;
				}

				Entity::APIFuncs::Print("> Fetching job list for \"" + Utils::ConvertToAnsiString(pSubItem->wszName) + "\"...");
				this->m_sCurSubItem = *pSubItem;

				if (!this->m_pBrowser->FetchJobList(L"http://" + Browser::SettingHostURL + L"/cdg/" + pSubItem->wszName + L"/list.txt")) {
					Entity::APIFuncs::Print2("> Failed to fetch file list from \"" + Utils::ConvertToAnsiString(Browser::SettingHostURL) + "/cdg/" + Utils::ConvertToAnsiString(pSubItem->wszName) + "/list.txt\"", Console::ConColor(150, 0, 0));
					return;
				}

				if (Utils::DirExists(L"tools\\" + pSubItem->wszName)) {
					if (!Utils::RemoveEntireDirectory(L"tools\\" + pSubItem->wszName)) {
						Entity::APIFuncs::Print2("> Failed to remove previous installation, new installation will most likely fail", Console::ConColor(150, 150, 0));
					}
				}

				if (!CreateDirectory((L"tools\\" + pSubItem->wszName).c_str(), nullptr)) {
					Entity::APIFuncs::Print2("> Failed to create tool base directory", Console::ConColor(150, 0, 0));
					return;
				}

				this->m_bDownloadingItem = true;
				this->m_uiJobListItemId = 0;
			}
		public:
			CMenuCatBrowser() : IMenuItemContainer<SubItemCatBrowse>(), m_pToolMgr(nullptr), m_pRenderer(nullptr), m_pSound(nullptr), m_bFetchingList(false), m_pBrowser(nullptr), m_bDownloadingItem(false), m_bNetworkError(false) {}
			CMenuCatBrowser(const std::wstring& wszText, Entity::CToolMgr* pToolMgr, DxRenderer::CDxRenderer* pRenderer, DxSound::CDxSound* pSound) : IMenuItemContainer<SubItemCatBrowse>(wszText), m_pToolMgr(pToolMgr), m_pRenderer(pRenderer), m_pSound(pSound), m_bFetchingList(false), m_pBrowser(nullptr), m_bDownloadingItem(false), m_bNetworkError(false)
			{
				IMenuItemContextMenu::SetDrawingInterface(pRenderer);
				IMenuItemContextMenu::SetFontDims(Entity::Vector(DefaultFontSizeW, DefaultFontSizeH));
				IMenuItemContextMenu::SetColors(Entity::Color(0, 162, 232, 150), Entity::Color(231, 231, 232, 150), Entity::Color(0, 0, 0, 150));
				IMenuItemContextMenu::AddContextMenuItem(L"Download", CMenuCatBrowser::ContextMenuItemDownload);
			}
			~CMenuCatBrowser()
			{
				if (this->m_pBrowser) {
					delete this->m_pBrowser;
				}
			}

			virtual void OnShow(void)
			{
				//Fetch tool link list from host

				if (Menu::workshopService == WORKSHOP_STEAM) {
					SteamFriends()->ActivateGameOverlayToWebPage("https://steamcommunity.com/app/1001860/workshop/");
					return;
				}

				this->m_bNetworkError = true;

				this->Release();

				this->m_pBrowser = new Browser::CBrowser();
				if (!this->m_pBrowser) {
					Entity::APIFuncs::Print2("Failed to instantiate Browser object", Console::ConColor(150, 0, 0));
					return;
				}

				if (!Browser::SettingHostURL.length()) {
					Entity::APIFuncs::Print2("Host URL is empty, nothing to do", Console::ConColor(150, 150, 0));
					return;
				}

				if (!this->m_pBrowser->FetchToolList(L"http://" + Browser::SettingHostURL + L"/cdg/list.txt")) {
					Entity::APIFuncs::Print2("Failed to fetch tool link list from \"" + Utils::ConvertToAnsiString(Browser::SettingHostURL) + "\"", Console::ConColor(150, 0, 0));
					return;
				}
				
				this->m_bNetworkError = false;
				this->m_bFetchingList = true;
				this->m_bDownloadingItem = false;
				this->m_uiFetchingItemId = 0;
			}
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection)
			{
				//Download selected tool

				bToggleAfterSelection = false;

				if (IMenuItemContextMenu::IsContextMenuVisible())
					return;

				if (eMouseKey == MKEY_RIGHT) {
					this->m_uiSelectedItem = uiItemId;
					IMenuItemContextMenu::ShowContextMenu(this->GetCursorPos());
				}
			}
			virtual void HandleContextMenuItem(const size_t uiIdentifier)
			{
				//Handle clicked items

				switch (uiIdentifier) {
				case CMenuCatBrowser::ContextMenuItemDownload: {
					this->InitDownload(this->m_uiSelectedItem);
					break;
				}
				default:
					break;
				}
			}
			virtual void OnProcess(void)
			{
				//Download current pointed item (preview data or workshop item data)

				//Perform base processings
				IMenuItemContainerBase::OnProcess();

				if (!this->m_pBrowser)
					return;

				if (this->m_bFetchingList) { //When fetching preview data
					if (this->m_uiFetchingItemId < this->m_pBrowser->GetToolLinkCount()) {
						//Get item data info pointer
						const Browser::CBrowser::toollink_s* pToolLink = this->m_pBrowser->GetToolLinkItem(this->m_uiFetchingItemId);
						if (pToolLink) {
							if ((this->m_pToolMgr->FindTool(pToolLink->wszName) != SI_INVALID_ID) && (pToolLink->wszVersion == this->m_pToolMgr->GetToolVersion(pToolLink->wszName))) {
								Entity::APIFuncs::Print("Tool " + Utils::ConvertToAnsiString(pToolLink->wszName) + " is already installed, skipping...");
							} else {
								Entity::APIFuncs::Print("Downloading preview info: \"" + Utils::ConvertToAnsiString(pToolLink->wszImageLink) + "\" to \"" + Utils::ConvertToAnsiString(L"cache\\" + pToolLink->wszName + L".png") + "\"");
								//Download preview image
								if (this->m_pBrowser->DownloadResource(pToolLink->wszImageLink, L"cache\\" + pToolLink->wszName + L".png")) {
									DxRenderer::HD3DSPRITE hSprite = this->m_pRenderer->LoadSprite(L"cache\\" + pToolLink->wszName + L".png", 1, SettingMenuPreviewItemResW, SettingMenuPreviewItemResH, 1, false);
									if (hSprite != GFX_INVALID_SPRITE_ID) {
										if (!this->AddSubItem(new SubItemCatBrowse(hSprite, pToolLink->wszName))) {
											Entity::APIFuncs::Print2("Failed to add preview image", Console::ConColor(150, 0, 0));
										}
									}
								}
							}
						}

						this->m_uiFetchingItemId++; //Goto next item
					
						//Check if end is reached
						if (this->m_uiFetchingItemId >= this->m_pBrowser->GetToolLinkCount()) {
							//Clear resources and values
							this->m_bFetchingList = false;
							delete this->m_pBrowser;
							this->m_pBrowser = nullptr;
						}
					}
				} else if (this->m_bDownloadingItem) { //When downloading workshop item data
					if (this->m_uiJobListItemId < this->m_pBrowser->GetJobListCount()) {
						//Get item data info pointer
						const Browser::CBrowser::dljob_s* pJobItem = this->m_pBrowser->GetJobListItem(this->m_uiJobListItemId);
						if (pJobItem) {
							Entity::APIFuncs::Print("> Processing job: \"" + Utils::ConvertToAnsiString(pJobItem->wszAction) + "\" on \"" + Utils::ConvertToAnsiString(pJobItem->wszItem) + "\"");
							if (pJobItem->wszAction == L"Create") { //Create directory
								if (!CreateDirectory((L"tools\\" + this->m_sCurSubItem.wszName + L"\\" + pJobItem->wszItem).c_str(), nullptr)) {
									Entity::APIFuncs::Print2("> Failed to create directory: \"" + Utils::ConvertToAnsiString(pJobItem->wszItem) + "\"", Console::ConColor(150, 0, 0));
								}
							} else if (pJobItem->wszAction == L"Download") { //Download item
								if (!this->m_pBrowser->DownloadResource(L"http://" + Browser::SettingHostURL + L"/cdg/" + this->m_sCurSubItem.wszName + L"/" + pJobItem->wszItem, L"tools\\" + this->m_sCurSubItem.wszName + L"\\" + pJobItem->wszItem)) {
									Entity::APIFuncs::Print2("> Failed to download item: \"" + Utils::ConvertToAnsiString(pJobItem->wszItem) + "\"", Console::ConColor(150, 0, 0));
								}
							} else {
								Entity::APIFuncs::Print2("> Unknown job: " + Utils::ConvertToAnsiString(pJobItem->wszAction), Console::ConColor(150, 150, 0));
							}
						}

						this->m_uiJobListItemId++; //Goto next item

						//Check if end is reached
						if (this->m_uiJobListItemId >= this->m_pBrowser->GetJobListCount()) {
							//Load downloaded tool
							if (this->m_pToolMgr->LoadToolFromName(this->m_sCurSubItem.wszName, sGameKeys) != Entity::CToolMgr::InvalidToolHandle) {
								Entity::APIFuncs::Print2("Tool \"" + Utils::ConvertToAnsiString(this->m_sCurSubItem.wszName) + "\": Ok", Console::ConColor(0, 150, 0));
							} else {
								Entity::APIFuncs::Print2("Tool \"" + Utils::ConvertToAnsiString(this->m_sCurSubItem.wszName) + "\": Fail", Console::ConColor(150, 0, 0));
							}
							//Clear resources and values
							this->m_bDownloadingItem = false;
							delete this->m_pBrowser;
							this->m_pBrowser = nullptr;
							//Print info
							Entity::APIFuncs::Print("> Download finished!");
							this->PushInfo(L"Download finished!");
							//Play indicator sound
							DxSound::HDXSOUND hInfoSound = this->m_pSound->QuerySound(L"res\\dlsucc.wav");
							if (hInfoSound) this->m_pSound->Play(hInfoSound, 10, 0);
							//Reload list
							this->OnShow();
						}
					}
				}
			}
			virtual void OnDraw(const Entity::Vector& vBodyStart) {}
			virtual void OnDrawAdditions(const Entity::Vector& vBodyStart)
			{
				//Draw additional info

				if (Menu::workshopService == WorkshopService_e::WORKSHOP_STEAM) {
					this->m_pRenderer->DrawString(DefaultFontHandle, L"Use the Steam Workshop for getting new tools.", vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
					return;
				}

				IMenuItemContextMenu::Draw(this->GetCursorPos());

				if (this->m_bFetchingList) {
					#define WSHOP_INFO_BOX_SPACING 10
					#define WSHOP_INFO_TEXT L"Fetching Workshop items..."
					this->m_pRenderer->DrawFilledBox(vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)wcslen(WSHOP_INFO_TEXT) / 2 * DefaultFontSizeW)) - WSHOP_INFO_BOX_SPACING / 2, vBodyStart[1] + SettingMenuBgImageHeight - 150 - WSHOP_INFO_BOX_SPACING / 2, (int)wcslen(WSHOP_INFO_TEXT) * DefaultFontSizeW + WSHOP_INFO_BOX_SPACING, DefaultFontSizeH + WSHOP_INFO_BOX_SPACING, 177, 240, 167, 150);
					this->m_pRenderer->DrawString(DefaultFontHandle, WSHOP_INFO_TEXT, vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)wcslen(WSHOP_INFO_TEXT) / 2 * DefaultFontSizeW)), vBodyStart[1] + SettingMenuBgImageHeight - 150, 0, 0, 0, 150);
				} else if (this->m_bDownloadingItem) {
					#define WSHOP_INFO_BOX_SPACING 10
					std::wstring wszCurrentInfoText = L"Downloading selected workshop item " + this->m_sCurSubItem.wszName + L" (" + std::to_wstring(this->m_uiJobListItemId) + L"/" + std::to_wstring(this->m_pBrowser->GetJobListCount()) + L")...";
					this->m_pRenderer->DrawFilledBox(vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)wszCurrentInfoText.length() / 2 * DefaultFontSizeW)) - WSHOP_INFO_BOX_SPACING / 2, vBodyStart[1] + SettingMenuBgImageHeight - 150 - WSHOP_INFO_BOX_SPACING / 2, (int)wszCurrentInfoText.length() * DefaultFontSizeW + WSHOP_INFO_BOX_SPACING, DefaultFontSizeH + WSHOP_INFO_BOX_SPACING, 177, 240, 167, 150);
					this->m_pRenderer->DrawString(DefaultFontHandle, wszCurrentInfoText, vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)wszCurrentInfoText.length() / 2 * DefaultFontSizeW)), vBodyStart[1] + SettingMenuBgImageHeight - 150, 0, 0, 0, 150);
				} else if (this->m_bNetworkError) {
					this->m_pRenderer->DrawString(DefaultFontHandle, L"Network service unavailable. Please try again later.", vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
				} else {
					if (!this->GetSubItemCount()) {
						this->m_pRenderer->DrawString(DefaultFontHandle, L"There are no new tools available.", vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
					}
				}

				for (size_t i = 0; i < this->GetResultInfoStringCount(); i++) {
					#define WSHOP_INFO_BOX_SPACING 10
					const resultinfo_s& rData = this->GetResultInfoData(i);
					this->m_pRenderer->DrawFilledBox(vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)rData.wszText.length() / 2 * DefaultFontSizeW)) - WSHOP_INFO_BOX_SPACING / 2, vBodyStart[1] + SettingMenuBgImageHeight - 150 - WSHOP_INFO_BOX_SPACING / 2, (int)rData.wszText.length() * DefaultFontSizeW + WSHOP_INFO_BOX_SPACING, DefaultFontSizeH + WSHOP_INFO_BOX_SPACING, rData.sColor.r, rData.sColor.g, rData.sColor.b, rData.sColor.a);
					this->m_pRenderer->DrawString(DefaultFontHandle, rData.wszText, vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)rData.wszText.length() / 2 * DefaultFontSizeW)), vBodyStart[1] + SettingMenuBgImageHeight - 150, 0, 0, 0, 150);
				}
			}
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey)
			{
				if (IMenuItemContextMenu::IsContextMenuVisible()) {
					if (eMouseKey == MKEY_LEFT)
						IMenuItemContextMenu::Click(vCursorPos);
				}
			}

			virtual bool CustomDrawing(void) const { return false; }
			virtual bool IsValid(void) const { return true; }
			virtual Entity::Vector GetSubItemPreviewRes(void) const { return Entity::Vector(SettingMenuPreviewItemResW, SettingMenuPreviewItemResH); }
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) { SubItemCatBrowse* pSubItem = this->GetSubItem(uiItemId); if (pSubItem) return pSubItem->hPreviewSprite; return nullptr; }
		};

		struct SubItemCatScreens { DxRenderer::HD3DSPRITE hPreviewSprite; std::wstring wszFile; SubItemCatScreens(DxRenderer::HD3DSPRITE spr, const std::wstring& sz) : hPreviewSprite(spr), wszFile(sz) {} };
		class CMenuCatScreenshots : public IMenuItemContainer<SubItemCatScreens>, IMenuItemContextMenu { //Handler for screenshots
		private:
			static const int ContextMenuItemCopy = 0;
			static const int ContextMenuItemUpload = 1;
			static const int ContextMenuItemDelete = 2;

			DxRenderer::CDxRenderer* m_pRenderer;
			Entity::CTimer m_oUploadTimer;
			std::wstring m_wszCurrentScreenshot;

			static inline DWORD InMinutes(const DWORD dwMinutes) { return dwMinutes * (1000 * 60); }

			std::vector<std::wstring> ListImages(const std::wstring& wszBaseDir)
			{
				static const wchar_t* wszSpriteExt[] = {
					{ L"png" },{ L"bmp" },{ L"jpg" },{ L"jpeg" }
				};

				return Utils::ListFilesByExt(wszBaseDir, wszSpriteExt, _countof(wszSpriteExt));
			}

			bool CopyScreenshotToClipboard(const std::wstring& wszFile)
			{
				//Copy screenshot content to clipboard

				bool bResult = false;

				if (OpenClipboard(this->m_pRenderer->GetOwnerWindow())) { //Open clipboard
					EmptyClipboard(); //Clear clipboard content

					//Load Bitmap file from disk
					HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, wszFile.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					if (hBitmap) {
						//Copy Bitmap to clipboard
						if (SetClipboardData(CF_BITMAP, hBitmap)) {
							bResult = true;
						}
					}

					//Close clipboard
					CloseClipboard();
				}

				return bResult;
			}

			inline bool MayUpload(void) { this->m_oUploadTimer.Update(); return this->m_oUploadTimer.Elapsed(); }
		public:
			CMenuCatScreenshots() : IMenuItemContainer<SubItemCatScreens>(), m_pRenderer(nullptr) {}
			CMenuCatScreenshots(const std::wstring& wszText, DxRenderer::CDxRenderer* pRenderer) : IMenuItemContainer<SubItemCatScreens>(wszText), m_pRenderer(pRenderer)
			{
				this->m_oUploadTimer.SetDelay(CMenuCatScreenshots::InMinutes(5));
				this->m_oUploadTimer.SetActive(true);
				this->m_oUploadTimer.Reset();
				IMenuItemContextMenu::SetDrawingInterface(pRenderer);
				IMenuItemContextMenu::SetFontDims(Entity::Vector(DefaultFontSizeW, DefaultFontSizeH));
				IMenuItemContextMenu::SetColors(Entity::Color(0, 162, 232, 150), Entity::Color(231, 231, 232, 150), Entity::Color(0, 0, 0, 150));
				IMenuItemContextMenu::AddContextMenuItem(L"Copy", CMenuCatScreenshots::ContextMenuItemCopy);
				if (Menu::bEnableScreenshotUpload) {
					IMenuItemContextMenu::AddContextMenuItem(L"Upload", CMenuCatScreenshots::ContextMenuItemUpload);
				}
				IMenuItemContextMenu::AddContextMenuItem(L"Remove", CMenuCatScreenshots::ContextMenuItemDelete);
			}

			virtual void OnShow(void)
			{
				//Reload all thumbnails

				this->Release();

				std::vector<std::wstring> vScreenshots = this->ListImages(L"screenshots");

				for (size_t i = 0; i < vScreenshots.size(); i++) {
					DxRenderer::HD3DSPRITE hSprite = this->m_pRenderer->LoadSprite(L"screenshots\\" + vScreenshots[i], 1, SettingMenuPreviewItemResW, SettingMenuPreviewItemResH, 1, true);
					if (hSprite != GFX_INVALID_SPRITE_ID) {
						this->AddSubItem(new SubItemCatScreens(hSprite, L"screenshots\\" + vScreenshots[i]));
					}
				}
			}
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection)
			{
				//Copy screenshot content of item to clipboard and upload to server if in permission time or delete selected screenshot from disk

				bToggleAfterSelection = false;

				if (IMenuItemContextMenu::IsContextMenuVisible())
					return;

				SubItemCatScreens* pSubItem = this->GetSubItem(uiItemId);
				if (!pSubItem)
					return;

				if (eMouseKey == MKEY_RIGHT) {
					this->m_wszCurrentScreenshot = pSubItem->wszFile;
					IMenuItemContextMenu::ShowContextMenu(this->GetCursorPos());
				}
			}
			virtual void HandleContextMenuItem(const size_t uiIdentifier)
			{
				//Handle clicked items

				switch (uiIdentifier) {
				case CMenuCatScreenshots::ContextMenuItemCopy: {
					if (this->CopyScreenshotToClipboard(this->m_wszCurrentScreenshot))
						this->PushInfo(L"Screenshot copied to clipboard!");

					break;
				}
				case CMenuCatScreenshots::ContextMenuItemUpload: {
					//if (this->MayUpload()) {
						if (!Browser::SettingHostURL.length()) {
							this->PushError(L"Image upload failed!");
							return;
						}
						std::wstring wszFileName = Utils::ExtractFileName(this->m_wszCurrentScreenshot);
						Browser::CImageUploader oImageUploader(Utils::ConvertToAnsiString(Browser::SettingHostURL), "/cdg/uploadScreenshot", Utils::ConvertToAnsiString(wszFileName), Utils::ConvertToAnsiString(this->m_wszCurrentScreenshot), std::to_string(SteamUser()->GetSteamID().ConvertToUint64()), "imgfile");
						bool bResult = oImageUploader.Upload();
						if (bResult) {
							this->PushInfo(L"Image upload succeeded!");
						} else {
							this->PushError(L"Image upload failed!");
						}
					//}
					break;
				}
				case CMenuCatScreenshots::ContextMenuItemDelete: {
					if (DeleteFile(this->m_wszCurrentScreenshot.c_str())) {
						this->PushInfo(L"Screenshot " + this->m_wszCurrentScreenshot + L" has been removed!");
						this->OnShow();
					}
					break;
				}
				default:
					break;
				}
			}
			virtual void OnProcess(void)
			{
				//Process result info entries

				IMenuItemContainerBase::OnProcess();
			}
			virtual void OnDraw(const Entity::Vector& vBodyStart) {}
			virtual void OnDrawAdditions(const Entity::Vector& vBodyStart)
			{
				//Draw result info data and screenshot content information

				IMenuItemContextMenu::Draw(this->GetCursorPos());

				for (size_t i = 0; i < this->GetResultInfoStringCount(); i++) {
					#define SSHOT_INFO_BOX_SPACING 10
					const resultinfo_s& rData = this->GetResultInfoData(i);
					this->m_pRenderer->DrawFilledBox(vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)rData.wszText.length() / 2 * DefaultFontSizeW)) - SSHOT_INFO_BOX_SPACING / 2, vBodyStart[1] + SettingMenuBgImageHeight - 150 - SSHOT_INFO_BOX_SPACING / 2, (int)rData.wszText.length() * DefaultFontSizeW + SSHOT_INFO_BOX_SPACING, DefaultFontSizeH + SSHOT_INFO_BOX_SPACING, rData.sColor.r, rData.sColor.g, rData.sColor.b, rData.sColor.a);
					this->m_pRenderer->DrawString(DefaultFontHandle, rData.wszText, vBodyStart[0] + (SettingMenuBodyW / 2 - ((int)rData.wszText.length() / 2 * DefaultFontSizeW)), vBodyStart[1] + SettingMenuBgImageHeight - 150, 0, 0, 0, 150);
				}

				if (!this->GetSubItemCount()) {
					this->m_pRenderer->DrawString(DefaultFontHandle, L"The screenshot directory is empty.", vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
				}
			}
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey)
			{
				if (IMenuItemContextMenu::IsContextMenuVisible()) {
					if (eMouseKey == MKEY_LEFT)
						IMenuItemContextMenu::Click(vCursorPos);
				}
			}

			virtual bool CustomDrawing(void) const { return false; }
			virtual bool IsValid(void) const { return true; }
			virtual Entity::Vector GetSubItemPreviewRes(void) const { return Entity::Vector(SettingMenuPreviewItemResW, SettingMenuPreviewItemResH); }
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) { SubItemCatScreens* pSubItem = this->GetSubItem(uiItemId); if (pSubItem) return pSubItem->hPreviewSprite; return nullptr; }
		};

		struct SubItemCatSettings { byte ucNone; };
		class CMenuCatSettings : public IMenuItemContainer<SubItemCatSettings> { //Handler for settings
		private:
			DxRenderer::CDxRenderer* m_pRenderer;
			DxSound::CDxSound* m_pSound;
			Entity::Vector m_vecBodyStart;
			std::vector<std::vector<std::wstring>>* m_pToolBindings;
			size_t m_uiSelectedToolBinding;
			CMenuDropDown* m_pMenuDropDown;
			size_t m_uiLastSelection;
			Entity::CToolMgr* m_pToolMgr;
		public:
			CMenuCatSettings() : IMenuItemContainer<SubItemCatSettings>(), m_pRenderer(nullptr), m_pSound(nullptr), m_uiSelectedToolBinding(-1), m_pToolMgr(nullptr), m_uiLastSelection(-1), m_pToolBindings(nullptr) {}
			CMenuCatSettings(const std::wstring& wszText, DxRenderer::CDxRenderer* pRenderer, DxSound::CDxSound* pSound, Entity::CToolMgr* pToolMgr, std::vector<std::vector<std::wstring>>* pToolBindings) : IMenuItemContainer<SubItemCatSettings>(wszText), m_pRenderer(pRenderer), m_pSound(pSound), m_pToolMgr(pToolMgr), m_pToolBindings(pToolBindings), m_uiSelectedToolBinding(-1), m_uiLastSelection(-1)
			{
				//Instantiate resources
				this->m_pMenuDropDown = new CMenuDropDown(pRenderer, Entity::Vector(0, 0), 135);
			}
			~CMenuCatSettings()
			{
				//Release resources

				if (this->m_pMenuDropDown) {
					delete this->m_pMenuDropDown;
				}
			}

			virtual void OnShow(void)
			{
				//Initialize data on show event

				//Load tool list into dropdown menu
				this->m_pMenuDropDown->Clear();
				this->m_pMenuDropDown->AddItem(L"#0", 0);
				for (size_t i = 0; i < this->m_pToolMgr->Count(); i++) {
					const Entity::CToolMgr::gt_list_item_s& rInfo = this->m_pToolMgr->GetBaseInfo(i);
					this->m_pMenuDropDown->AddItem(Utils::ConvertToWideString(rInfo.szToolScript), i + 1);
				}
			}
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection)
			{
				bToggleAfterSelection = false;
			}
			virtual void OnDraw(const Entity::Vector& vBodyStart)
			{
				//Draw settings information

				//Store body start position
				this->m_vecBodyStart = vBodyStart;

				//Draw volume
				this->m_pRenderer->DrawString(DefaultFontHandle, L"Volume: << " + std::to_wstring(this->m_pSound->GetGlobalVolume()) + L" >>", vBodyStart[0] + 1, vBodyStart[1] + 10, 200, 200, 200, 255);
				
				//Draw tool bindings
				for (size_t i = 0; i < this->m_pToolBindings->size(); i++) {
					if ((*this->m_pToolBindings)[i].size() == 2) {
						int r = 200, g = 200, b = 200;
						if (this->m_uiSelectedToolBinding == i) { r = 0; g = 100; b = 0; }
						this->m_pRenderer->DrawString(DefaultFontHandle, (*this->m_pToolBindings)[i][0] + L": " + (*this->m_pToolBindings)[i][1], vBodyStart[0] + 1, vBodyStart[1] + 50 + ((int)i * (DefaultFontSizeH + 5)), r, g, b, 255);
					}
				}

				//Draw drop down menu if tool binding is selected
				if (this->m_uiSelectedToolBinding != -1) {
					this->m_pMenuDropDown->Draw(Entity::Vector(vBodyStart[0] + 200, vBodyStart[1] + 50));
				}

				//Draw exit confirmation indicator
				std::wstring wszConfirmOnExit = ((Menu::bShallConfirmOnExit) ? L"Yes" : L"No");
				this->m_pRenderer->DrawString(DefaultFontHandle, L"Confirm on exit: " + wszConfirmOnExit + L" (click to toggle)", vBodyStart[0] + 1, vBodyStart[1] + 300, 200, 200, 200, 255);
			}
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey)
			{
				//Handle click events

				//Decrement volume
				if ((vCursorPos[0] >= this->m_vecBodyStart[0] + 1 + 44) && (vCursorPos[1] >= this->m_vecBodyStart[1] + 10) && (vCursorPos[0] <= this->m_vecBodyStart[0] + 1 + 44 + 17) && (vCursorPos[1] <= this->m_vecBodyStart[1] + 10 + 15)) {
					long lVolume = this->m_pSound->GetGlobalVolume();
					lVolume--;
					if (lVolume < 0)
						lVolume = 0;
					this->m_pSound->SetGlobalVolume(lVolume);

					return;
				}

				//Increment volume
				if ((vCursorPos[0] >= this->m_vecBodyStart[0] + 1 + 81) && (vCursorPos[1] >= this->m_vecBodyStart[1] + 10) && (vCursorPos[0] <= this->m_vecBodyStart[0] + 1 + 81 + 17) && (vCursorPos[1] <= this->m_vecBodyStart[1] + 10 + 15)) {
					long lVolume = this->m_pSound->GetGlobalVolume();
					lVolume++;
					if (lVolume > 10)
						lVolume = 10;
					this->m_pSound->SetGlobalVolume(lVolume);

					return;
				}

				//Handle tool key binding selection
				for (size_t i = 0; i < (*this->m_pToolBindings).size(); i++) {
					if ((*this->m_pToolBindings)[i].size() == 2) {
						if ((vCursorPos[0] >= this->m_vecBodyStart[0] + 1) && (vCursorPos[1] >= this->m_vecBodyStart[1] + 50 + ((int)i * (DefaultFontSizeH + 5))) && (vCursorPos[0] <= this->m_vecBodyStart[0] + 1 + (((*this->m_pToolBindings)[i][0].length() + (*this->m_pToolBindings)[i][1].length() + 2) * DefaultFontSizeW)) && (vCursorPos[1] <= this->m_vecBodyStart[1] + 50 + DefaultFontSizeH + ((int)i * (DefaultFontSizeH + 5)))) {
							this->m_uiSelectedToolBinding = i;
							break;
						}
					}
				}

				//Inform drop down menu
				if (this->m_uiSelectedToolBinding != -1) {
					this->m_pMenuDropDown->OnClick(vCursorPos);
					if (this->m_pMenuDropDown->Selection() != this->m_uiLastSelection) {
						(*this->m_pToolBindings)[this->m_uiSelectedToolBinding][1] = this->m_pMenuDropDown->Item(this->m_pMenuDropDown->Selection()).wszText;
						this->m_uiLastSelection = this->m_pMenuDropDown->Selection();
						this->m_uiSelectedToolBinding = -1;
					}
				}

				//Toggle exit confirmation indicator
				if ((vCursorPos[0] >= this->m_vecBodyStart[0] + 1 + 110) && (vCursorPos[1] >= this->m_vecBodyStart[1] + 300) && (vCursorPos[0] <= this->m_vecBodyStart[0] + 1 + 110 + 17) && (vCursorPos[1] <= this->m_vecBodyStart[1] + 300 + 15)) {
					Menu::bShallConfirmOnExit = !Menu::bShallConfirmOnExit;

					return;
				}
			}
			virtual void OnKeyPressed(int vKey)
			{
				//Handle key press

				//Store tool binding and unselect
				if (vKey == 32 /* Enter */) {
					if (this->m_uiSelectedToolBinding != -1) {
						(*this->m_pToolBindings)[this->m_uiSelectedToolBinding][1] = this->m_pMenuDropDown->Item(this->m_pMenuDropDown->Selection()).wszText;

						this->m_uiSelectedToolBinding = -1;
					}
				}
			}

			virtual bool CustomDrawing(void) const { return true; }
			virtual bool IsValid(void) const { return this->m_pRenderer != nullptr; }
			virtual Entity::Vector GetSubItemPreviewRes(void) const { return Entity::Vector(SettingMenuPreviewItemResW, SettingMenuPreviewItemResH); }
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) { return nullptr; }
		};

		struct SubItemCatBackgrounds { std::wstring wszName; DxRenderer::HD3DSPRITE hPreview; D3DXIMAGE_INFO sInfo;  SubItemCatBackgrounds(const std::wstring& wsz, DxRenderer::HD3DSPRITE prv, const D3DXIMAGE_INFO& info) : wszName(wsz), hPreview(prv), sInfo(info) {} };
		class CMenuCatBackgrounds : public IMenuItemContainer<SubItemCatBackgrounds>, IMenuItemContextMenu { //Handler for backgrounds
		public:
			static const size_t ContextMenuItemSelect = 0;
		private:
			DxRenderer::CDxRenderer* m_pRenderer;
			Entity::Vector m_vecBodyStart;
			size_t m_uiContextMenuItem;
		public:
			CMenuCatBackgrounds() : IMenuItemContainer<SubItemCatBackgrounds>(), m_pRenderer(nullptr), m_uiContextMenuItem(-1) {}
			CMenuCatBackgrounds(const std::wstring& wszText, DxRenderer::CDxRenderer* pRenderer) : IMenuItemContainer<SubItemCatBackgrounds>(wszText), m_pRenderer(pRenderer), m_uiContextMenuItem(-1)
			{
				//Construct context menu and load all backgrounds

				IMenuItemContextMenu::SetDrawingInterface(pRenderer);
				IMenuItemContextMenu::SetFontDims(Entity::Vector(DefaultFontSizeW, DefaultFontSizeH));
				IMenuItemContextMenu::SetColors(Entity::Color(0, 162, 232, 150), Entity::Color(231, 231, 232, 150), Entity::Color(0, 0, 0, 150));
				IMenuItemContextMenu::AddContextMenuItem(L"Select", CMenuCatBackgrounds::ContextMenuItemSelect);

				WIN32_FIND_DATAW sFindData = { 0 };
				HANDLE hFindData = FindFirstFileW(L"backgrounds\\*.*", &sFindData);
				if (hFindData != INVALID_HANDLE_VALUE) {
					while (FindNextFile(hFindData, &sFindData)) {
						if (sFindData.cFileName[0] == '.') {
							continue;
						}

						D3DXIMAGE_INFO sImageInfo;
						if (pRenderer->GetSpriteInfo(std::wstring(L"backgrounds\\" + std::wstring(sFindData.cFileName)), sImageInfo)) {
							DxRenderer::HD3DSPRITE hPreview = pRenderer->LoadSprite(std::wstring(L"backgrounds\\" + std::wstring(sFindData.cFileName)), 1, SettingMenuPreviewItemResW, SettingMenuPreviewItemResH, 1, true);
							if (hPreview != GFX_INVALID_SPRITE_ID) {
								this->AddSubItem(new SubItemCatBackgrounds(sFindData.cFileName, hPreview, sImageInfo));
							}
						}
					}

					CloseHandle(hFindData);
				}
			}
			~CMenuCatBackgrounds()
			{
				//Release resources

				for (size_t i = 0; i < this->GetSubItemCount(); i++) {
					SubItemCatBackgrounds* pItem = this->GetSubItem(i);
					if (pItem != nullptr) {
						this->m_pRenderer->FreeSprite(pItem->hPreview);
					}
				}
			}

			virtual void OnShow(void)
			{
				//Initialize data on show event
			}
			virtual void HandleContextMenuItem(const size_t uiIdentifier)
			{
				//Handle clicked items

				switch (uiIdentifier) {
				case CMenuCatBackgrounds::ContextMenuItemSelect: {
					SubItemCatBackgrounds* pItem = this->GetSubItem(this->m_uiContextMenuItem);
					if (pItem != nullptr) {
						this->m_pRenderer->SetBackgroundPicture(L"backgrounds\\" + pItem->wszName);
					}
					break;
				}
				default:
					break;
				}
			}
			virtual void OnSelect(const size_t uiItemId, MouseKey_e eMouseKey, bool& bToggleAfterSelection)
			{
				//Perform selection

				bToggleAfterSelection = false;

				if (IMenuItemContextMenu::IsContextMenuVisible())
					return;

				if (eMouseKey == MKEY_RIGHT) {
					this->m_uiContextMenuItem = uiItemId;
					IMenuItemContextMenu::ShowContextMenu(this->GetCursorPos());
				}
			}
			virtual void OnDraw(const Entity::Vector& vBodyStart)
			{
				//Draw settings information
			}
			virtual void OnDrawAdditions(const Entity::Vector& vBodyStart)
			{
				//Draw additional stuff

				IMenuItemContextMenu::Draw(this->GetCursorPos());

				if (!this->GetSubItemCount()) {
					this->m_pRenderer->DrawString(DefaultFontHandle, L"There are no backgrounds in the associated folder.", vBodyStart[0], vBodyStart[1], 200, 200, 200, 150);
				}
			}
			virtual void OnClick(const Entity::Vector& vCursorPos, MouseKey_e eMouseKey)
			{
				//Handle click events

				if (IMenuItemContextMenu::IsContextMenuVisible()) {
					if (eMouseKey == MKEY_LEFT)
						IMenuItemContextMenu::Click(vCursorPos);

					return;
				}
			}
			virtual void OnKeyPressed(int vKey)
			{
				//Handle key press
			}

			virtual bool CustomDrawing(void) const { return false; }
			virtual bool IsValid(void) const { return this->m_pRenderer != nullptr; }
			virtual Entity::Vector GetSubItemPreviewRes(void) const { return Entity::Vector(SettingMenuPreviewItemResW, SettingMenuPreviewItemResH); }
			virtual DxRenderer::HD3DSPRITE GetSubItemPreviewImage(const size_t uiItemId) { SubItemCatBackgrounds* pSubItem = this->GetSubItem(uiItemId); if (pSubItem) return pSubItem->hPreview; return nullptr; }
		};

		bool m_bReady;
		bool m_bVisible;
		DxRenderer::HD3DSPRITE m_hMenuSprite;
		DxRenderer::HD3DSPRITE m_hCursor;
		DxRenderer::CDxRenderer* m_pRenderer;
		DxSound::CDxSound* m_pSound;
		Entity::Vector m_vScreenPos;
		Entity::Vector m_vCursorPos;
		DxSound::HDXSOUND m_hSndMenuShow;
		DxSound::HDXSOUND m_hSndSelItem;
		std::vector<IMenuItemContainerBase*> m_vMenuItems;
		size_t m_uiSelectedItem;
		DxRenderer::d3dfont_s* m_pDefaultFont;

		bool AddMenuItem(IMenuItemContainerBase* pItemHandler)
		{
			//Add new menu item handler object

			if (!pItemHandler)
				return false;

			if (!pItemHandler->IsValid())
				return false;

			this->m_vMenuItems.push_back(pItemHandler);

			return true;
		}

		void Release(bool bFreeMenuItems = true)
		{
			//Release resources

			if (!this->m_bReady)
				return;

			this->m_pRenderer->FreeSprite(this->m_hMenuSprite);
			this->m_pRenderer->FreeSprite(this->m_hCursor);

			if (bFreeMenuItems) {
				for (size_t i = 0; i < this->m_vMenuItems.size(); i++) {
					if (this->m_vMenuItems[i])
						delete this->m_vMenuItems[i];
				}
			}

			this->m_vMenuItems.clear();

			this->m_pDefaultFont = nullptr;
			this->m_bReady = false;
		}
	public:
		CMenu() : m_bReady(false), m_bVisible(false), m_pRenderer(nullptr), m_pSound(nullptr), m_uiSelectedItem(std::wstring::npos), m_pDefaultFont(nullptr) {}
		CMenu(DxRenderer::CDxRenderer* pRenderer, DxSound::CDxSound* pSound, Entity::CToolMgr* pToolMgr, const Entity::Vector& vScreenRes, std::vector<std::vector<std::wstring>>* pToolBindings) : CMenu() { this->Initialize(pRenderer, pSound, pToolMgr, vScreenRes, pToolBindings); }
		~CMenu() { this->Release(); }

		bool Initialize(DxRenderer::CDxRenderer* pRenderer, DxSound::CDxSound* pSound, Entity::CToolMgr* pToolMgr, const Entity::Vector& vScreenRes, std::vector<std::vector<std::wstring>>* pToolBindings)
		{
			//Initialize menu

			if ((!pRenderer) || (!pToolMgr) || (!pSound))
				return false;

			//Load default font
			this->m_pDefaultFont = pRenderer->LoadFont(DefaultFontName, DefaultFontSizeW, DefaultFontSizeH);
			if (!this->m_pDefaultFont)
				return false;

			//Load menu sprite
			this->m_hMenuSprite = pRenderer->LoadSprite(L"res\\menubg.png", 1, SettingMenuBgImageWidth, SettingMenuBgImageHeight, 1, false);
			if (this->m_hMenuSprite == GFX_INVALID_SPRITE_ID)
				return false;

			//Load menu cursor
			this->m_hCursor = pRenderer->LoadSprite(L"res\\menucursor.png", 1, 16, 16, 1, false);
			if (this->m_hCursor == GFX_INVALID_SPRITE_ID) {
				pRenderer->FreeSprite(this->m_hMenuSprite);
				return false;
			}

			//Load menu sounds
			this->m_hSndMenuShow = pSound->QuerySound(L"res\\menu_show.wav");
			this->m_hSndSelItem = pSound->QuerySound(L"res\\menu_select.wav");

			//Calculate center position
			this->m_vScreenPos[0] = vScreenRes[0] / 2 - SettingMenuBgImageWidth / 2;
			this->m_vScreenPos[1] = vScreenRes[1] / 2 - SettingMenuBgImageHeight / 2;

			//Store data
			this->m_pRenderer = pRenderer;
			this->m_pSound = pSound;
			DefaultFontHandle = this->m_pDefaultFont;

			//Instantiate item container objects
			#define ADD_MENU_ITEM(constr) if (!this->AddMenuItem(new constr)) { pRenderer->FreeSprite(this->m_hMenuSprite); return false; }
			ADD_MENU_ITEM(CMenuCatNews(L"News", pRenderer));
			ADD_MENU_ITEM(CMenuCatTools(L"Tools", pToolMgr, pRenderer));
			ADD_MENU_ITEM(CMenuCatBrowser(L"Workshop", pToolMgr, pRenderer, pSound));
			ADD_MENU_ITEM(CMenuCatScreenshots(L"Screenshots", pRenderer));
			ADD_MENU_ITEM(CMenuCatSettings(L"Settings", pRenderer, pSound, pToolMgr, pToolBindings));
			ADD_MENU_ITEM(CMenuCatBackgrounds(L"Backgrounds", pRenderer));

			return this->m_bReady = true;
		}

		void Draw(void)
		{
			//Draw menu

			if ((!this->m_bReady) || (!this->m_bVisible))
				return;

			//Draw background image
			this->m_pRenderer->DrawSprite(this->m_hMenuSprite, this->m_vScreenPos[0], this->m_vScreenPos[1], 0, 0.0f);
			
			//Draw navigation items
			for (size_t i = 0; i < this->m_vMenuItems.size(); i++) {
				//Draw selection box if mouse hovers over current iterated item
				if ((this->m_vCursorPos[0] >= this->m_vScreenPos[0] + SettingMenuNavX) && (this->m_vCursorPos[0] <= this->m_vScreenPos[0] + SettingMenuNavX + SettingMenuNavW) && (this->m_vCursorPos[1] >= this->m_vScreenPos[1] + SettingMenuNavY + NAV_TEXT_SPACING + (NAV_ITEM_YPOS(i) - NAV_TEXT_AREA_HEIGHT / 4)) && (this->m_vCursorPos[1] <= this->m_vScreenPos[1] + SettingMenuNavY + NAV_TEXT_SPACING + (NAV_ITEM_YPOS(i) - NAV_TEXT_AREA_HEIGHT / 4) + NAV_TEXT_AREA_HEIGHT)) {
					this->m_pRenderer->DrawFilledBox(this->m_vScreenPos[0] + SettingMenuNavX - 2, this->m_vScreenPos[1] + SettingMenuNavY + NAV_TEXT_SPACING + NAV_ITEM_YPOS(i) - (NAV_TEXT_AREA_HEIGHT / 4), 153, NAV_TEXT_AREA_HEIGHT, 0, 122, 204, 150);
				}

				//Setup current iterated item text color
				Entity::Color sTextColor(200, 200, 200, 150);
				if (i == this->m_uiSelectedItem) {
					sTextColor.r = 0;
					sTextColor.g = 162;
					sTextColor.b = 232;
				}

				//Draw item container text
				this->m_pRenderer->DrawString(this->m_pDefaultFont, this->m_vMenuItems[i]->GetText(), this->m_vScreenPos[0] + SettingMenuNavX + NAV_TEXT_SPACING, this->m_vScreenPos[1] + SettingMenuNavY + NAV_TEXT_SPACING + NAV_ITEM_YPOS(i), sTextColor.GetG(), sTextColor.GetB(), sTextColor.GetB(), sTextColor.GetA());
			}

			//Handle item container drawing
			if ((this->m_uiSelectedItem != std::wstring::npos) && (this->m_uiSelectedItem < this->m_vMenuItems.size())) {
				if (this->m_vMenuItems[this->m_uiSelectedItem]->CustomDrawing()) {
					//Let item manager perform its custom drawing
					this->m_vMenuItems[this->m_uiSelectedItem]->OnDraw(Entity::Vector(this->m_vScreenPos[0] + SettingMenuBodyX, this->m_vScreenPos[1] + SettingMenuBodyY));
				} else {
					//Perform default drawing
					for (size_t i = this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex(); i < SettingMenuPreviewItemDisplayCount + this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex(); i++) { //Loop through current entry range
						DxRenderer::HD3DSPRITE hPreviewImage = this->m_vMenuItems[this->m_uiSelectedItem]->GetSubItemPreviewImage(i); //Obtain preview image sprite handle
						if (hPreviewImage != GFX_INVALID_SPRITE_ID) {
							Entity::Vector vDrawingPos;
							//Calculate absolute screen coordinates according to item display ID
							vDrawingPos[0] = (int)(((i - this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex()) % SettingMenuPreviewItemLineCount) * ((size_t)SettingMenuPreviewItemResW + SettingMenuPreviewItemGapHor)) + this->m_vScreenPos[0] + SettingMenuBodyX;
							vDrawingPos[1] = (int)(((i - this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex()) / SettingMenuPreviewItemLineCount) * ((size_t)SettingMenuPreviewItemResH + SettingMenuPreviewItemGapVert)) + this->m_vScreenPos[1] + SettingMenuBodyY;
							//Draw selection box
							if ((this->m_vCursorPos[0] >= vDrawingPos[0]) && (this->m_vCursorPos[0] <= vDrawingPos[0] + SettingMenuPreviewItemResW) && (this->m_vCursorPos[1] >= vDrawingPos[1]) && (this->m_vCursorPos[1] <= vDrawingPos[1] + SettingMenuPreviewItemResH)) {
								this->m_pRenderer->DrawBox(vDrawingPos[0], vDrawingPos[1], SettingMenuPreviewItemResW, SettingMenuPreviewItemResH, 1, 0, 122, 204, 150);
							}
							//Draw sprite
							this->m_pRenderer->DrawSprite(hPreviewImage, vDrawingPos[0], vDrawingPos[1], 0, 0.0f, -1, -1, 0.0f, 0.0f, false, 0, 0, 0, 0);
						}
					}
				}

				//Let item container draw additional stuff
				this->m_vMenuItems[this->m_uiSelectedItem]->OnDrawAdditions(Entity::Vector(this->m_vScreenPos[0] + SettingMenuBodyX, this->m_vScreenPos[1] + SettingMenuBodyY));
			
				//Draw scroll position indicator
				if (this->m_vMenuItems[this->m_uiSelectedItem]->GetSubItemCount() > SettingMenuPreviewItemDisplayCount) {
					//Calculate percent of current item start to item amount
					double dblPercentItems = ((double)this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex() * 100) / ((double)this->m_vMenuItems[this->m_uiSelectedItem]->GetSubItemCount() - SettingMenuPreviewItemDisplayCount);
					//Calculate pixel screen pos according to percent
					int iPixelValue = ((int)dblPercentItems * (SettingMenuScrollbarLength - SettingMenuScrollCubeWH)) / 100;
					//Draw indicator
					this->m_pRenderer->DrawFilledBox(this->m_vScreenPos[0] + SettingMenuBgImageWidth - 32, this->m_vScreenPos[1] + 75 + iPixelValue, SettingMenuScrollCubeWH, SettingMenuScrollCubeWH, 100, 100, 100, 155);
				}
			}

			//Draw menu cursor
			this->m_pRenderer->DrawSprite(this->m_hCursor, this->m_vCursorPos[0], this->m_vCursorPos[1], 0, 0.0f);
		}

		void OnClick(const Entity::Vector& vPos, MouseKey_e eMouseKey)
		{
			//Handle click event
			
			if ((!this->m_bReady) || (!this->m_bVisible))
				return;
			
			//Handle close area click event
			if ((vPos[0] >= this->m_vScreenPos[0] + SettingMenuCloseX) && (vPos[0] <= this->m_vScreenPos[0] + SettingMenuCloseX + SettingMenuCloseW) && (vPos[1] >= this->m_vScreenPos[1] + SettingMenuCloseY) && (vPos[1] <= this->m_vScreenPos[1] + SettingMenuCloseY + SettingMenuCloseH)) {
				this->Toggle();
				return;
			}

			//Handle navigation area click event
			if ((vPos[0] >= this->m_vScreenPos[0] + SettingMenuNavX) && (vPos[0] <= this->m_vScreenPos[0] + SettingMenuNavX + SettingMenuNavW) && (vPos[1] >= this->m_vScreenPos[1] + SettingMenuNavY) && (vPos[1] <= this->m_vScreenPos[1] + SettingMenuNavY + SettingMenuNavH)) {
				for (size_t i = 0; i < this->m_vMenuItems.size(); i++) {
					if ((vPos[0] >= this->m_vScreenPos[0] + SettingMenuNavX) && (vPos[0] <= this->m_vScreenPos[0] + SettingMenuNavX + SettingMenuNavW) && (vPos[1] >= this->m_vScreenPos[1] + SettingMenuNavY + NAV_TEXT_SPACING + (NAV_ITEM_YPOS(i) - NAV_TEXT_AREA_HEIGHT / 4)) && (vPos[1] <= this->m_vScreenPos[1] + SettingMenuNavY + NAV_TEXT_SPACING + (NAV_ITEM_YPOS(i) - NAV_TEXT_AREA_HEIGHT / 4) + NAV_TEXT_AREA_HEIGHT)) {
						this->m_uiSelectedItem = i;
						this->m_vMenuItems[this->m_uiSelectedItem]->OnShow();
						this->m_pSound->Play(this->m_hSndSelItem, 10, 0);
						return;
					}
				}
			}

			//Handle scroll buttons click event
			if ((vPos[0] >= this->m_vScreenPos[0] + SettingMenuScrollUpX) && (vPos[0] <= this->m_vScreenPos[0] + SettingMenuScrollUpX + SettingMenuScrollWH) && (vPos[1] >= this->m_vScreenPos[1] + SettingMenuScrollUpY) && (vPos[1] <= this->m_vScreenPos[1] + SettingMenuScrollUpY + SettingMenuScrollWH)) {
				this->ScrollUp();
			}
			if ((vPos[0] >= this->m_vScreenPos[0] + SettingMenuScrollDownX) && (vPos[0] <= this->m_vScreenPos[0] + SettingMenuScrollDownX + SettingMenuScrollWH) && (vPos[1] >= this->m_vScreenPos[1] + SettingMenuScrollDownY) && (vPos[1] <= this->m_vScreenPos[1] + SettingMenuScrollDownY + SettingMenuScrollWH)) {
				this->ScrollDown();
			}

			//Inform item handler
			this->m_vMenuItems[this->m_uiSelectedItem]->OnClick(vPos, eMouseKey);

			//Handle body area click event
			if ((vPos[0] >= this->m_vScreenPos[0] + SettingMenuBodyX) && (vPos[0] <= this->m_vScreenPos[0] + SettingMenuBodyX + SettingMenuBodyW) && (vPos[1] >= this->m_vScreenPos[1] + SettingMenuBodyY) && (vPos[1] <= this->m_vScreenPos[1] + SettingMenuBodyY + SettingMenuBodyH)) {
				//Inform selected item handler
				if ((this->m_uiSelectedItem != std::wstring::npos) && (this->m_uiSelectedItem < this->m_vMenuItems.size())) {
					//Inform item handler
					//this->m_vMenuItems[this->m_uiSelectedItem]->OnClick(vPos, eMouseKey);

					//Perform custom selection (if custom drawing is disabled)
					if (!this->m_vMenuItems[this->m_uiSelectedItem]->CustomDrawing()) {
						for (size_t i = this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex(); i < SettingMenuPreviewItemDisplayCount + this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex(); i++) {
							Entity::Vector vScreenPos;
							vScreenPos[0] = (int)(((i - this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex()) % SettingMenuPreviewItemLineCount) * ((size_t)SettingMenuPreviewItemResW + SettingMenuPreviewItemGapHor)) + this->m_vScreenPos[0] + SettingMenuBodyX;
							vScreenPos[1] = (int)(((i - this->m_vMenuItems[this->m_uiSelectedItem]->GetScrollIndex()) / SettingMenuPreviewItemLineCount) * ((size_t)SettingMenuPreviewItemResH + SettingMenuPreviewItemGapVert)) + this->m_vScreenPos[1] + SettingMenuBodyY;
							if ((vPos[0] >= vScreenPos[0]) && (vPos[0] <= vScreenPos[0] + SettingMenuPreviewItemResW) && (vPos[1] >= vScreenPos[1]) && (vPos[1] <= vScreenPos[1] + SettingMenuPreviewItemResH)) {
								bool bToggleAfterSelection = false;
								this->m_vMenuItems[this->m_uiSelectedItem]->OnSelect(i, eMouseKey, bToggleAfterSelection);

								this->m_pSound->Play(this->m_hSndSelItem, 10, 0);

								if (bToggleAfterSelection) {
									this->Toggle();
								}

								return;
							}
						}
					}
				}
			}
		}

		void OnKeyPressed(int vKey)
		{
			//Called when a key is pressed

			if (this->m_uiSelectedItem < this->m_vMenuItems.size()) {
				this->m_vMenuItems[this->m_uiSelectedItem]->OnKeyPressed(vKey);
			}
		}
		
		//Events/Actions
		void RefreshCurrent(void) { if (this->m_uiSelectedItem < this->m_vMenuItems.size()) this->m_vMenuItems[this->m_uiSelectedItem]->OnShow(); }
		void ScrollDown(void) { if (this->m_uiSelectedItem < this->m_vMenuItems.size()) this->m_vMenuItems[this->m_uiSelectedItem]->ScrollDown(); }
		void ScrollUp(void) { if (this->m_uiSelectedItem < this->m_vMenuItems.size()) this->m_vMenuItems[this->m_uiSelectedItem]->ScrollUp(); }
		void Process(void) { if ((this->m_uiSelectedItem != std::wstring::npos) && (this->m_uiSelectedItem < this->m_vMenuItems.size())) this->m_vMenuItems[this->m_uiSelectedItem]->OnProcess(); }
		inline void Toggle(void) { this->m_bVisible = !this->m_bVisible; if (this->m_bVisible) this->m_pSound->Play(this->m_hSndMenuShow, 10, 0); }
		inline void UpdateCursorPos(const Entity::Vector& vPos) { this->m_vCursorPos = vPos; if (this->m_uiSelectedItem < this->m_vMenuItems.size()) this->m_vMenuItems[this->m_uiSelectedItem]->SetCursorPos(vPos); }
		inline void SetCategory(const size_t uiCategory) { this->m_uiSelectedItem = uiCategory; this->m_vMenuItems[this->m_uiSelectedItem]->OnShow(); }

		//Getters
		inline bool IsReady(void) { return this->m_bReady; }
		inline bool IsVisible(void) const { return this->m_bVisible; }
	};

	class CExitMenu { //Game exit menu
	public:
		typedef void(*TpfnOnConfirmPrompt)(void);
	private:
		static const int ExitMenuWidth = 380;
		static const int ExitMenuHeight = 110;
		DxRenderer::CDxRenderer* m_pRenderer;
		bool m_bVisible;
		Entity::Vector m_vDrawPos;
		std::wstring m_wszMenuKeyName;
		TpfnOnConfirmPrompt m_pfnConfirmPrompt;
		bool m_bFlagValue;
	public:
		CExitMenu() : m_pRenderer(nullptr), m_bVisible(false), m_wszMenuKeyName(L""), m_pfnConfirmPrompt(nullptr) {}
		CExitMenu(DxRenderer::CDxRenderer* pRenderer, TpfnOnConfirmPrompt pEventFunction) 
			: m_pRenderer(pRenderer),
			m_bVisible(false),
			m_wszMenuKeyName(L""),
			m_pfnConfirmPrompt(pEventFunction)
		{
			//Init center drawing position
			this->m_vDrawPos[0] = pRenderer->GetWindowWidth() / 2 - CExitMenu::ExitMenuWidth / 2;
			this->m_vDrawPos[1] = pRenderer->GetWindowHeight() / 2 - CExitMenu::ExitMenuHeight / 2;

			//Get name of menu key
			WCHAR wszName[MAX_PATH];
			UINT uiScanCode = MapVirtualKeyW(Menu::sGameKeys.vkMenu, MAPVK_VK_TO_VSC);
			int iCopyResult = GetKeyNameTextW((uiScanCode << 16), wszName, sizeof(wszName));
			if (iCopyResult != 0) {
				this->m_wszMenuKeyName = wszName;
			}

			//Set flag value
			this->m_bFlagValue = Menu::bShallConfirmOnExit;
		}
		~CExitMenu() {}

		void Draw(void)
		{
			//Draw the menu

			if (!this->m_bVisible)
				return;

			//Draw form
			this->m_pRenderer->DrawBox(this->m_vDrawPos[0] - 1, this->m_vDrawPos[1] - 1, CExitMenu::ExitMenuWidth + 1, ExitMenuHeight + 1, 1, 0, 122, 204, 150);
			this->m_pRenderer->DrawFilledBox(this->m_vDrawPos[0], this->m_vDrawPos[1], CExitMenu::ExitMenuWidth, CExitMenu::ExitMenuHeight, 37, 37, 38, 150);

			//Draw confirmation message
			this->m_pRenderer->DrawString(Menu::DefaultFontHandle, L"Do you really want to exit?", this->m_vDrawPos[0] + 6, this->m_vDrawPos[1] + 6, 200, 200, 200, 150);
			//Draw info message
			this->m_pRenderer->DrawString(Menu::DefaultFontHandle, L"(Press " + this->m_wszMenuKeyName + L" to open the menu)", this->m_vDrawPos[0] + 6, this->m_vDrawPos[1] + 25, 200, 200, 200, 150);

			//Draw checkbox content
			this->m_pRenderer->DrawBox(this->m_vDrawPos[0] + 20, this->m_vDrawPos[1] + 53, 10, 10, 1, 0, 122, 204, 150);
			this->m_pRenderer->DrawString(Menu::DefaultFontHandle, L"Do not ask again", this->m_vDrawPos[0] + 20 + 15, this->m_vDrawPos[1] + 50, 200, 200, 200, 150);
			if (!this->m_bFlagValue) {
				this->m_pRenderer->DrawFilledBox(this->m_vDrawPos[0] + 22, this->m_vDrawPos[1] + 55, 7, 7, 0, 122, 204, 150);
			}

			//Draw buttons
			this->m_pRenderer->DrawString(Menu::DefaultFontHandle, L"Yes", this->m_vDrawPos[0] + 14, this->m_vDrawPos[1] + 80, 200, 200, 200, 150);
			this->m_pRenderer->DrawString(Menu::DefaultFontHandle, L"No", this->m_vDrawPos[0] + 335, this->m_vDrawPos[1] + 80, 200, 200, 200, 150);
		}

		void OnClick(const Entity::Vector& vPos, MouseKey_e eMouseKey)
		{
			//Handle click events

			if ((vPos[0] >= this->m_vDrawPos[0] + 14) && (vPos[0] <= this->m_vDrawPos[0] + 14 + 20) && (vPos[1] >= this->m_vDrawPos[1] + 80) && (vPos[1] <= this->m_vDrawPos[1] + 80 + 20)) {
				Menu::bShallConfirmOnExit = this->m_bFlagValue;
				this->m_pfnConfirmPrompt();
			}
			else if ((vPos[0] >= this->m_vDrawPos[0] + 335) && (vPos[0] <= this->m_vDrawPos[0] + 335 + 20) && (vPos[1] >= this->m_vDrawPos[1] + 80) && (vPos[1] <= this->m_vDrawPos[1] + 80 + 20)) {
				this->Hide();
			}
			else if ((vPos[0] >= this->m_vDrawPos[0] + 20) && (vPos[0] <= this->m_vDrawPos[0] + 20 + 100) && (vPos[1] >= this->m_vDrawPos[1] + 53) && (vPos[1] <= this->m_vDrawPos[1] + 53 + 10)) {
				this->m_bFlagValue = !this->m_bFlagValue;
			}
		}

		void Show(void)
		{
			//Show the menu

			this->m_bVisible = true;
		}

		void Hide(void)
		{
			//Hide the menu

			this->m_bVisible = false;
		}

		void Toggle(void)
		{
			//Toggle the menu

			this->m_bVisible = !this->m_bVisible;
		}

		inline bool IsVisible(void)
		{
			//Indicate if visible or not

			return this->m_bVisible;
		}
	};
}