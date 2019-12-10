#include "entity.h"
#include "console.h"

/*
	Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2019 by Daniel Brendel

	Version: 0.8
	Contact: dbrendel1988<at>yahoo<dot>com
	GitHub: https://github.com/danielbrendel

	Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

namespace Entity {
	DxRenderer::CDxRenderer* pGfxReference = nullptr;
	DxSound::CDxSound* pSndReference = nullptr;
	Scripting::CScriptInt* pScrReference = nullptr;
	CToolMgr* _pGameToolMgrInstance = nullptr;
	CScriptedEntsMgr oScriptedEntMgr;
	DxRenderer::d3dfont_s* pDefaultFont = nullptr;
	Console::CConsole* pConReference = nullptr;
	CConVarManager oConVarMgr;
	std::wstring wszBasePath;

	void SetBasePath(const std::wstring& path) { wszBasePath = path; }

	namespace APIFuncs {
		void Print(const std::string& in)
		{
			pConReference->AddLine(Utils::ConvertToWideString(in));
		}

		void Print2(const std::string& in, const Console::ConColor& clr)
		{
			pConReference->AddLine(Utils::ConvertToWideString(in), clr);
		}

		void SetCursorOffset(const Vector& vOffset)
		{
			_pGameToolMgrInstance->SetCursorOffset(vOffset);
		}

		void SetCursorRotation(float fRotation)
		{
			_pGameToolMgrInstance->SetCursorRotation(fRotation);
		}

		void FatalError(const std::string& in)
		{
			MessageBox(GetForegroundWindow(), Utils::ConvertToWideString(in).c_str(), DNY_CDG_PRODUCT_NAME L" :: Fatal error in tool script", MB_ICONERROR);
			exit(EXIT_FAILURE);
		}

		DxRenderer::d3dfont_s* LoadFont(const std::string& szFontName, byte ucFontSizeW, byte ucFontSizeH)
		{
			return pGfxReference->LoadFont(Utils::ConvertToWideString(szFontName), ucFontSizeW, ucFontSizeH);
		}

		bool GetSpriteInfo(const std::string& szFile, SpriteInfo& out)
		{
			SpriteInfo sInfo;
			D3DXIMAGE_INFO sImageInfo;

			if (!pGfxReference->GetSpriteInfo(Utils::ConvertToWideString(szFile), sImageInfo))
				return false;

			sInfo.res = Vector((int)sImageInfo.Width, (int)sImageInfo.Height);
			sInfo.depth = sImageInfo.Depth;
			sInfo.format = (int)sImageInfo.Format;

			return true;
		}

		DxRenderer::HD3DSPRITE LoadSprite(const std::string& szTexture, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize)
		{
			return pGfxReference->LoadSprite(Utils::ConvertToWideString(szTexture), iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize);
		}

		bool FreeSprite(DxRenderer::HD3DSPRITE hSprite)
		{
			return pGfxReference->FreeSprite(hSprite);
		}

		bool DrawBox(const Vector& pos, const Vector& size, int iThickness, const Color& color)
		{
			return pGfxReference->DrawBox(pos[0], pos[1], size[0], size[1], iThickness, color.r, color.g, color.b, color.a);
		}

		bool DrawFilledBox(const Vector& pos, const Vector& size, const Color& color)
		{
			return pGfxReference->DrawFilledBox(pos[0], pos[1], size[0], size[1], color.r, color.g, color.b, color.a);
		}

		bool DrawLine(const Vector& start, const Vector& end, const Color& color)
		{
			return pGfxReference->DrawLine(start[0], start[1], end[0], end[1], color.r, color.g, color.b, color.a);
		}

		bool DrawSprite(const DxRenderer::HD3DSPRITE hSprite, const Vector& pos, int iFrame, float fRotation, const Vector& vRotPos, float fScale1, float fScale2, const bool bUseCustomColorMask, const Color& color)
		{
			return pGfxReference->DrawSprite(hSprite, pos[0], pos[1], iFrame, fRotation, vRotPos[0], vRotPos[1], fScale1, fScale2, bUseCustomColorMask, color.r, color.g, color.b, color.a);
		}

		bool DrawString(const DxRenderer::d3dfont_s* pFont, const std::string& szText, const Vector& pos, const Color& color)
		{
			return pGfxReference->DrawString(pFont, Utils::ConvertToWideString(szText), pos[0], pos[1], color.r, color.g, color.b, color.a);
		}

		DxRenderer::d3dfont_s* GetDefaultFont()
		{
			return pDefaultFont;
		}

		DxSound::HDXSOUND QuerySound(const std::string& szSoundFile)
		{
			return pSndReference->QuerySound(Utils::ConvertToWideString(szSoundFile));
		}

		bool PlaySound_(DxSound::HDXSOUND hSound, long lVolume)
		{
			return pSndReference->Play(hSound, lVolume, 0);
		}

		int GetWindowCenterX(void)
		{
			return pGfxReference->GetWindowWidth() / 2;
		}

		int GetWindowCenterY(void)
		{
			return pGfxReference->GetWindowHeight() / 2;
		}

		bool SpawnScriptedEntity(asIScriptObject* ref, const Vector& vPos)
		{
			return oScriptedEntMgr.Spawn(_pGameToolMgrInstance->GetScriptHandleOfSelection(), ref, vPos);
		}

		size_t GetEntityCount()
		{
			return oScriptedEntMgr.GetEntityCount();
		}

		asIScriptObject* GetEntityHandle(size_t uiEntityId)
		{
			return oScriptedEntMgr.GetEntityHandle(uiEntityId);
		}

		asIScriptObject* EntityTrace(const Vector& vStart, const Vector& vEnd, asIScriptObject* pIgnoredEnt)
		{
			CEntityTrace oEntityTrace(vStart, vEnd, pIgnoredEnt);
			return ((oEntityTrace.IsEmpty()) ? nullptr : oEntityTrace.EntityObject(0));
		}

		bool Ent_IsValid(asIScriptObject* pEntity)
		{
			return oScriptedEntMgr.IsValidEntity(pEntity);
		}

		size_t Ent_GetId(asIScriptObject* pEntity)
		{
			return oScriptedEntMgr.GetEntityId(pEntity);
		}

		bool ListFilesByExt(const std::string& szBaseDir, asIScriptFunction* pFunction, const char** pFileList, const size_t uiListLen)
		{
			WIN32_FIND_DATAA sFindData = { 0 };
			HANDLE hFileSearch;
			std::vector<std::string> vNames;

			hFileSearch = FindFirstFileA((szBaseDir + "\\*.*").c_str(), &sFindData);
			if (hFileSearch == INVALID_HANDLE_VALUE)
				return false;

			while (FindNextFileA(hFileSearch, &sFindData)) {
				if (sFindData.cFileName[0] == '.')
					continue;

				if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
					std::string szFileExt = Utils::ExtractFileExt(sFindData.cFileName);
					for (size_t i = 0; i < uiListLen; i++) {
						if (szFileExt == pFileList[i]) {
							vNames.push_back(sFindData.cFileName);
						}
					}
				}
			}

			FindClose(hFileSearch);

			for (size_t i = 0; i < vNames.size(); i++) {
				bool bResult = true;
				BEGIN_PARAMS(vArgs);
				PUSH_OBJECT(&vNames[i]);

				pScrReference->CallScriptFunction(pFunction, &vArgs, &bResult, Scripting::FA_BYTE);

				END_PARAMS(vArgs);

				if (!bResult)
					break;
			}

			pFunction->Release();

			return true;
		}

		bool ListSprites(const std::string& szBaseDir, asIScriptFunction* pFunction)
		{
			static const char* szSpriteExt[] = {
				{"png"}, {"bmp"}, {"jpg"}, {"jpeg" }
			};

			return ListFilesByExt(szBaseDir, pFunction, szSpriteExt, _countof(szSpriteExt));
		}
		
		bool ListSounds(const std::string& szBaseDir, asIScriptFunction* pFunction)
		{
			static const char* szSoundExt[] = {
				{ "wav" },{ "ogg" }
			};

			return ListFilesByExt(szBaseDir, pFunction, szSoundExt, _countof(szSoundExt));
		}

		int Random(int start, int end)
		{
			static bool bRndSeedOnce = false;
			if (!bRndSeedOnce) {
				srand((unsigned int)time(nullptr));
				bRndSeedOnce = true;
			}

			return (rand() % (end - start)) + start;
		}

		PConVar RegisterConVar(const std::string& szName, const CConVarManager::cvdatatype_e eType)
		{
			return oConVarMgr.Register(Utils::ConvertToWideString(szName), eType);
		}

		PConVar QueryConVar(const std::string& szName)
		{
			return oConVarMgr.Query(Utils::ConvertToWideString(szName));
		}

		bool FreeConVar(const std::string& szName)
		{
			return oConVarMgr.Free(Utils::ConvertToWideString(szName));
		}
	}

	CToolMgr* Initialize(DxRenderer::CDxRenderer* pGfx, DxSound::CDxSound* pSnd, Scripting::CScriptInt* pScr, Console::CConsole* pCns)
	{
		//Initialize entity scripting

		if ((!pGfx) || (!pSnd) || (!pScr))
			return nullptr;
		
		//Store references
		pGfxReference = pGfx;
		pSndReference = pSnd;
		pScrReference = pScr;
		pConReference = pCns;

		//Create default font
		pDefaultFont = pGfxReference->LoadFont(L"Verdana", 7, 15);
		if (!pDefaultFont)
			return nullptr;

		//Registration macros
		#define REG_ENUM(n, h) h = pScrReference->RegisterEnumeration(n); if (h == SI_INVALID_ID) { return nullptr; }
		#define ADD_ENUM(h, n, v) if (!pScrReference->AddEnumerationValue(h, n, v))  { return nullptr; }
		#define REG_TYPEDEF(t, n) if (!pScrReference->RegisterTypeDef(t, n))  { return nullptr; }
		#define REG_FUNCDEF(t) if (!pScrReference->RegisterFuncDef(t))  { return nullptr; }
		#define REG_STRUCT(n, s, h) h = pScrReference->RegisterStructure(n, s); if (h == SI_INVALID_ID)  { return nullptr; }
		#define ADD_STRUCT(n, o, h) if (!pScrReference->AddStructureMember(h, n, o))  { return false; }
		#define REG_CLASSV(n, s, h) h = pScrReference->RegisterClass_(n, s, asOBJ_VALUE | asOBJ_APP_CLASS); if (h == SI_INVALID_ID)  { return nullptr; }
		#define REG_CLASSR(n, s, h) h = pScrReference->RegisterClass_(n, s, asOBJ_REF); if (h == SI_INVALID_ID)  { return nullptr; }
		#define REG_CLASSRT(n, s, h) h = pScrReference->RegisterClass_(n, s, asOBJ_REF | asOBJ_TEMPLATE); if (h == SI_INVALID_ID)  { return nullptr; }
		#define REG_CLASSRNC(n, s, h) h = pScrReference->RegisterClass_(n, s, asOBJ_REF | asOBJ_NOCOUNT); if (h == SI_INVALID_ID)  { return nullptr; }
		#define ADD_CLASSM(n, o, h) if (!pScrReference->AddClassMember(h, n, o))  { return nullptr; }
		#define ADD_CLASSF(n, p, h) if (!pScrReference->AddClassMethod(h, n, p))  { return nullptr; }
		#define ADD_CLASSB(b, t, p, h) if (!pScrReference->AddClassBehaviour(h, b, t, p))  { return nullptr; }
		#define REG_FUNC(d, p, c) if (!pScrReference->RegisterFunction(d, p, c))  {return nullptr; }
		#define REG_VAR(d, p) if (!pScrReference->RegisterGlobalVariable(d, p))  { return nullptr; }
		#define REG_IF(n) if (!pScrReference->RegisterInterface(n))  { return nullptr; }
		#define REG_IFM(n, m) if (!pScrReference->RegisterInterfaceMethod(n, m))  { return nullptr; }

		//Register typedefs
		REG_TYPEDEF("uint64", "size_t");
		REG_TYPEDEF("uint16", "HostVersion");
		REG_TYPEDEF("uint64", "SpriteHandle");
		REG_TYPEDEF("uint64", "FontHandle");
		REG_TYPEDEF("uint64", "SoundHandle");
		REG_TYPEDEF("uint8", "DamageValue");

		//Register enum
		Scripting::HSIENUM hEnum;
		REG_ENUM("DamageType", hEnum);
		ADD_ENUM(hEnum, "DAMAGEABLE_NO", DAMAGEABLE_NO);
		ADD_ENUM(hEnum, "DAMAGEABLE_ALL", DAMAGEABLE_ALL);
		ADD_ENUM(hEnum, "DAMAGEABLE_NOTSQUAD", DAMAGEABLE_NOTSQUAD);
		REG_ENUM("FileSeekWay", hEnum);
		ADD_ENUM(hEnum, "SEEKW_BEGIN", CFileReader::SEEKW_BEGIN);
		ADD_ENUM(hEnum, "SEEKW_CURRENT", CFileReader::SEEKW_CURRENT);
		ADD_ENUM(hEnum, "SEEKW_END", CFileReader::SEEKW_END);

		//Register function def
		REG_FUNCDEF("bool FuncFileListing(const string& in)");
		
		//Register structs
		Scripting::HSISTRUCT hStructs;
		REG_STRUCT("ToolInfo", sizeof(CToolMgr::ToolInfo), hStructs);
		ADD_STRUCT("string szName", asOFFSET(CToolMgr::ToolInfo, szName), hStructs);
		ADD_STRUCT("string szVersion", asOFFSET(CToolMgr::ToolInfo, szVersion), hStructs);
		ADD_STRUCT("string szAuthor", asOFFSET(CToolMgr::ToolInfo, szAuthor), hStructs);
		ADD_STRUCT("string szContact", asOFFSET(CToolMgr::ToolInfo, szContact), hStructs);
		ADD_STRUCT("string szPreviewImage", asOFFSET(CToolMgr::ToolInfo, szPreviewImage), hStructs);
		ADD_STRUCT("string szCursor", asOFFSET(CToolMgr::ToolInfo, szCursor), hStructs);
		ADD_STRUCT("string szCategory", asOFFSET(CToolMgr::ToolInfo, szCategory), hStructs);
		ADD_STRUCT("int iCursorWidth", asOFFSET(CToolMgr::ToolInfo, iCursorWidth), hStructs);
		ADD_STRUCT("int iCursorHeight", asOFFSET(CToolMgr::ToolInfo, iCursorHeight), hStructs);
		ADD_STRUCT("uint32 uiTriggerDelay", asOFFSET(CToolMgr::ToolInfo, uiTriggerDelay), hStructs);
		REG_STRUCT("GameKeys", sizeof(Entity::game_keys_s), hStructs);
		ADD_STRUCT("int vkTrigger", asOFFSET(Entity::game_keys_s, vkTrigger), hStructs);
		ADD_STRUCT("int vkClean", asOFFSET(Entity::game_keys_s, vkClean), hStructs);
		ADD_STRUCT("int vkMenu", asOFFSET(Entity::game_keys_s, vkMenu), hStructs);
		ADD_STRUCT("int vkScrollUp", asOFFSET(Entity::game_keys_s, vkScrollUp), hStructs);
		ADD_STRUCT("int vkScrollDown", asOFFSET(Entity::game_keys_s, vkScrollDown), hStructs);
		ADD_STRUCT("int vkTeamSelect", asOFFSET(Entity::game_keys_s, vkTeamSelect), hStructs);
		ADD_STRUCT("int vkConsole", asOFFSET(Entity::game_keys_s, vkConsole), hStructs);
		ADD_STRUCT("int vkTakeScreen", asOFFSET(Entity::game_keys_s, vkTakeScreen), hStructs);
		ADD_STRUCT("int vkKey1", asOFFSET(Entity::game_keys_s, vkKey1), hStructs);
		ADD_STRUCT("int vkKey2", asOFFSET(Entity::game_keys_s, vkKey2), hStructs);
		ADD_STRUCT("int vkKey3", asOFFSET(Entity::game_keys_s, vkKey3), hStructs);
		ADD_STRUCT("int vkKey4", asOFFSET(Entity::game_keys_s, vkKey4), hStructs);
		ADD_STRUCT("int vkKey5", asOFFSET(Entity::game_keys_s, vkKey5), hStructs);
		ADD_STRUCT("int vkKey6", asOFFSET(Entity::game_keys_s, vkKey6), hStructs);
		ADD_STRUCT("int vkKey7", asOFFSET(Entity::game_keys_s, vkKey7), hStructs);
		ADD_STRUCT("int vkKey8", asOFFSET(Entity::game_keys_s, vkKey8), hStructs);
		ADD_STRUCT("int vkKey9", asOFFSET(Entity::game_keys_s, vkKey9), hStructs);
		ADD_STRUCT("int vkKey0", asOFFSET(Entity::game_keys_s, vkKey0), hStructs);
		ADD_STRUCT("int vkExit", asOFFSET(Entity::game_keys_s, vkExit), hStructs);

		//Register classes
		Scripting::HSICLASS hClasses;
		REG_CLASSV("Color", sizeof(Color), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(Color, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(Color, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(uint8 r, uint8 g, uint8 b, uint8 a)", asMETHOD(Color, Constr_Bytes), hClasses);
		ADD_CLASSF("uint8 R()", asMETHOD(Color, GetR), hClasses);
		ADD_CLASSF("uint8 G()", asMETHOD(Color, GetG), hClasses);
		ADD_CLASSF("uint8 B()", asMETHOD(Color, GetB), hClasses);
		ADD_CLASSF("uint8 A()", asMETHOD(Color, GetA), hClasses);
		REG_CLASSV("ConColor", sizeof(Console::ConColor), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(Console::ConColor, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(Console::ConColor, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(uint8 r, uint8 g, uint8 b)", asMETHOD(Console::ConColor, Constr_Bytes), hClasses);
		ADD_CLASSF("uint8 R()", asMETHOD(Console::ConColor, GetR), hClasses);
		ADD_CLASSF("uint8 G()", asMETHOD(Console::ConColor, GetG), hClasses);
		ADD_CLASSF("uint8 B()", asMETHOD(Console::ConColor, GetB), hClasses);
		REG_CLASSV("Vector", sizeof(Vector), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(Vector, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(Vector, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(Vector &in)", asMETHOD(Vector, Constr_Class), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(int x, int y)", asMETHOD(Vector, Constr_Ints), hClasses);
		ADD_CLASSF("int opIndex(int) const", asMETHODPR(Vector, operator[], (int) const, int), hClasses);
		ADD_CLASSF("int& opIndex(int)", asMETHODPR(Vector, operator[], (int), int&), hClasses);
		ADD_CLASSF("void opAssign(const Vector &in)", asMETHODPR(Vector, operator=, (const Vector&), void), hClasses);
		ADD_CLASSF("bool opEquals(const Vector &in)", asMETHODPR(Vector, operator==, (const Vector&), bool), hClasses);
		ADD_CLASSF("void opPostInc(int)", asMETHODPR(Vector, operator++, (int), void), hClasses);
		ADD_CLASSF("void opPostDec(int)", asMETHODPR(Vector, operator--, (int), void), hClasses);
		ADD_CLASSF("Vector opAdd(const Vector &in)", asMETHODPR(Vector, operator+, (const Vector&), Vector), hClasses);
		ADD_CLASSF("Vector opSub(const Vector &in)", asMETHODPR(Vector, operator-, (const Vector&), Vector), hClasses);
		ADD_CLASSF("Vector opMul(const Vector &in)", asMETHODPR(Vector, operator*, (const Vector&), Vector), hClasses);
		ADD_CLASSF("Vector opDiv(const Vector &in)", asMETHODPR(Vector, operator/, (const Vector&), Vector), hClasses);
		ADD_CLASSF("int GetX() const", asMETHOD(Vector, GetX), hClasses);
		ADD_CLASSF("int GetY() const", asMETHOD(Vector, GetY), hClasses);
		ADD_CLASSF("int Distance(const Vector &in)", asMETHOD(Vector, Distance), hClasses);
		ADD_CLASSF("void Zero()", asMETHOD(Vector, Zero), hClasses);
		REG_CLASSV("SpriteInfo", sizeof(SpriteInfo), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(SpriteInfo, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(SpriteInfo, Destruct), hClasses);
		ADD_CLASSF("Vector& GetResolution()", asMETHOD(SpriteInfo, GetResolution), hClasses);
		ADD_CLASSF("int GetDepth()", asMETHOD(SpriteInfo, GetDepth), hClasses);
		ADD_CLASSF("int GetFormat()", asMETHOD(SpriteInfo, GetFormat), hClasses);
		REG_CLASSV("BoundingBox", sizeof(CBoundingBox), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CBoundingBox, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CBoundingBox, Destruct), hClasses);
		ADD_CLASSF("bool Alloc()", asMETHOD(CBoundingBox, Alloc), hClasses);
		ADD_CLASSF("void AddBBoxItem(const Vector& in, const Vector& in)", asMETHOD(CBoundingBox, AddBBoxItem), hClasses);
		ADD_CLASSF("bool IsCollided(const Vector& in, const Vector& in, const BoundingBox& in)", asMETHOD(CBoundingBox, IsCollided), hClasses);
		ADD_CLASSF("bool IsInside(const Vector &in, const Vector &in)", asMETHOD(CBoundingBox, IsInside), hClasses);
		ADD_CLASSF("bool IsEmpty()", asMETHOD(CBoundingBox, IsEmpty), hClasses);
		ADD_CLASSF("void Clear()", asMETHOD(CBoundingBox, Clear), hClasses);
		REG_CLASSV("TempSprite", sizeof(CTempSprite), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CTempSprite, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string &in, uint32 dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)", asMETHOD(CTempSprite, Constr_Init), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CTempSprite, Destruct), hClasses);
		ADD_CLASSF("bool Initialize(const string &in, uint32 dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)", asMETHOD(CTempSprite, Initialize), hClasses);
		ADD_CLASSF("void Draw(const Vector &in, float fRotation)", asMETHOD(CTempSprite, Draw), hClasses);
		ADD_CLASSF("void Release()", asMETHOD(CTempSprite, Release), hClasses);
		REG_CLASSV("Model", sizeof(CModel), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CModel, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CModel, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string &in)", asMETHOD(CModel, Constr_IntputStr), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string &in, bool b)", asMETHOD(CModel, Constr_IntputStrBool), hClasses);
		ADD_CLASSF("void opAssign(const Model &in)", asMETHODPR(CModel, operator=, (const CModel&), void), hClasses);
		ADD_CLASSF("bool Initialize(const string&in szMdlFile, bool bForceCustomSize)", asMETHOD(CModel, Initialize), hClasses);
		ADD_CLASSF("bool Initialize2(const BoundingBox& in, SpriteHandle hSprite)", asMETHOD(CModel, Initialize2), hClasses);
		ADD_CLASSF("void Release()", asMETHOD(CModel, Release), hClasses);
		ADD_CLASSF("bool IsCollided(const Vector& in mypos, const Vector& in refpos, const Model& in mdl)", asMETHOD(CModel, IsCollided), hClasses);
		ADD_CLASSF("bool IsValid()", asMETHOD(CModel, IsValid), hClasses);
		ADD_CLASSF("bool Alloc()", asMETHOD(CModel, Alloc), hClasses);
		ADD_CLASSF("void SetCenter(const Vector &in)", asMETHOD(CModel, SetCenter), hClasses);
		ADD_CLASSF("SpriteHandle Handle()", asMETHOD(CModel, Sprite), hClasses);
		ADD_CLASSF("const Vector& GetCenter() const", asMETHOD(CModel, Center), hClasses);
		ADD_CLASSF("BoundingBox& GetBBox()", asMETHOD(CModel, BBox), hClasses);
		REG_CLASSV("Timer", sizeof(CTimer), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CTimer, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CTimer, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(uint32 delay)", asMETHOD(CTimer, Constr_Delay), hClasses);
		ADD_CLASSF("void Reset()", asMETHOD(CTimer, Reset), hClasses);
		ADD_CLASSF("void Update()", asMETHOD(CTimer, Update), hClasses);
		ADD_CLASSF("void SetActive(bool bStatus)", asMETHOD(CTimer, SetActive), hClasses);
		ADD_CLASSF("void SetDelay(uint32 delay)", asMETHOD(CTimer, SetDelay), hClasses);
		ADD_CLASSF("bool IsActive()", asMETHOD(CTimer, Started), hClasses);
		ADD_CLASSF("uint32 GetDelay()", asMETHOD(CTimer, Delay), hClasses);
		ADD_CLASSF("bool IsElapsed()", asMETHOD(CTimer, Elapsed), hClasses);
		REG_CLASSV("FileReader", sizeof(CFileReader), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CFileReader, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CFileReader, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string& in)", asMETHOD(CFileReader, Constr_Init), hClasses);
		ADD_CLASSF("bool Open(const string &in)", asMETHOD(CFileReader, Open), hClasses);
		ADD_CLASSF("bool IsOpen()", asMETHOD(CFileReader, IsOpen), hClasses);
		ADD_CLASSF("bool Eof()", asMETHOD(CFileReader, Eof), hClasses);
		ADD_CLASSF("void Seek(FileSeekWay from, int offset)", asMETHOD(CFileReader, Seek), hClasses);
		ADD_CLASSF("string GetLine()", asMETHOD(CFileReader, GetLine), hClasses);
		ADD_CLASSF("string GetEntireContent(bool bSkipNLChar = false)", asMETHOD(CFileReader, GetEntireContent), hClasses);
		ADD_CLASSF("void Close()", asMETHOD(CFileReader, Close), hClasses);
		REG_CLASSV("FileWriter", sizeof(CFileWriter), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f()", asMETHOD(CFileWriter, Construct), hClasses);
		ADD_CLASSB(asBEHAVE_DESTRUCT, "void f()", asMETHOD(CFileWriter, Destruct), hClasses);
		ADD_CLASSB(asBEHAVE_CONSTRUCT, "void f(const string& in)", asMETHOD(CFileWriter, Constr_Init), hClasses);
		ADD_CLASSF("bool Open(const string &in, bool bAppend = true)", asMETHOD(CFileWriter, Open), hClasses);
		ADD_CLASSF("bool IsOpen()", asMETHOD(CFileWriter, IsOpen), hClasses);
		ADD_CLASSF("bool Eof()", asMETHOD(CFileWriter, Eof), hClasses);
		ADD_CLASSF("void Seek(FileSeekWay from, int offset)", asMETHOD(CFileWriter, Seek), hClasses);
		ADD_CLASSF("void Write(const string &in)", asMETHOD(CFileWriter, Write), hClasses);
		ADD_CLASSF("void WriteLine(const string &in)", asMETHOD(CFileWriter, WriteLine), hClasses);
		ADD_CLASSF("void Close()", asMETHOD(CFileWriter, Close), hClasses);

		//Register interface
		REG_IF("IScriptedEntity");
		REG_IFM("IScriptedEntity", "void OnSpawn(const Vector& in vec)");
		REG_IFM("IScriptedEntity", "void OnRelease()");
		REG_IFM("IScriptedEntity", "void OnProcess()");
		REG_IFM("IScriptedEntity", "void OnDraw()");
		REG_IFM("IScriptedEntity", "bool DoUserCleaning()");
		REG_IFM("IScriptedEntity", "DamageType IsDamageable()");
		REG_IFM("IScriptedEntity", "void OnDamage(DamageValue dv)");
		REG_IFM("IScriptedEntity", "Model& GetModel()");
		REG_IFM("IScriptedEntity", "Vector& GetPosition()");
		REG_IFM("IScriptedEntity", "float GetRotation()");
		REG_IFM("IScriptedEntity", "DamageValue GetDamageValue()");
		REG_IFM("IScriptedEntity", "bool NeedsRemoval()");
		REG_IFM("IScriptedEntity", "string GetName()");

		//Register scripting API

		struct script_api_func_s {
			std::string szDefinition;
			void* pFunction;
		} sGameAPIFunctions[] = {
			{ "void Print(const string& in)", &APIFuncs::Print },
			{ "void PrintClr(const string& in, const ConColor &in)", &APIFuncs::Print2 },
			{ "void SetCursorOffset(const Vector& in)", &APIFuncs::SetCursorOffset },
			{ "void SetCursorRotation(float fRotation)", &APIFuncs::SetCursorRotation },
			{ "FontHandle R_LoadFont(const string& in, uint8 ucFontSizeW, uint8 ucFontSizeH)", &APIFuncs::LoadFont },
			{ "bool R_GetSpriteInfo(const string &in, SpriteInfo &out)", &APIFuncs::GetSpriteInfo },
			{ "SpriteHandle R_LoadSprite(const string& in szFile, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)", &APIFuncs::LoadSprite },
			{ "bool R_FreeSprite(SpriteHandle hSprite)", &APIFuncs::FreeSprite },
			{ "bool R_DrawBox(const Vector& in pos, const Vector&in size, int iThickness, const Color&in color)", &APIFuncs::DrawBox },
			{ "bool R_DrawFilledBox(const Vector&in pos, const Vector&in size, const Color&in color)", &APIFuncs::DrawFilledBox },
			{ "bool R_DrawLine(const Vector&in start, const Vector&in end, const Color&in color)", &APIFuncs::DrawLine },
			{ "bool R_DrawSprite(const SpriteHandle hSprite, const Vector&in pos, int iFrame, float fRotation, const Vector &in vRotPos, float fScale1, float fScale2, bool bUseCustomColorMask, const Color&in color)", &APIFuncs::DrawSprite },
			{ "bool R_DrawString(const FontHandle font, const string&in szText, const Vector&in pos, const Color&in color)", &APIFuncs::DrawString },
			{ "FontHandle R_GetDefaultFont()", &APIFuncs::GetDefaultFont },
			{ "SoundHandle S_QuerySound(const string&in szSoundFile)", &APIFuncs::QuerySound },
			{ "bool S_PlaySound(SoundHandle hSound, int32 lVolume)", &APIFuncs::PlaySound_ },
			{ "int Wnd_GetWindowCenterX()", &APIFuncs::GetWindowCenterX },
			{ "int Wnd_GetWindowCenterY()", &APIFuncs::GetWindowCenterY },
			{ "bool Ent_SpawnEntity(IScriptedEntity @obj, const Vector& in)", &APIFuncs::SpawnScriptedEntity },
			{ "size_t Ent_GetEntityCount()", &APIFuncs::GetEntityCount },
			{ "IScriptedEntity@+ Ent_GetEntityHandle(size_t uiEntityId)", &APIFuncs::GetEntityHandle },
			{ "IScriptedEntity@+ Ent_TraceLine(const Vector&in vStart, const Vector&in vEnd, IScriptedEntity@+ pIgnoredEnt)", &APIFuncs::EntityTrace },
			{ "bool Ent_IsValid(IScriptedEntity@ pEntity)", &APIFuncs::Ent_IsValid },
			{ "size_t Ent_GetId(IScriptedEntity@ pEntity)", &APIFuncs::Ent_GetId },
			{ "bool Util_ListSprites(const string& in, FuncFileListing @cb)", &APIFuncs::ListSprites },
			{ "bool Util_ListSounds(const string& in, FuncFileListing @cb)", &APIFuncs::ListSounds },
			{ "int Util_Random(int start, int end)", &APIFuncs::Random } 
		};

		for (size_t i = 0; i < _countof(sGameAPIFunctions); i++) {
			if (!pScrReference->RegisterFunction(sGameAPIFunctions[i].szDefinition, sGameAPIFunctions[i].pFunction))
				return nullptr;
		}
		
		return new CToolMgr;
	}

	void DoUserCleaning(void)
	{
		oScriptedEntMgr.OnUserClean();
	}
}