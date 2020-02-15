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
#include "scriptint.h"
#include "utils.h"
#include "console.h"

/* Entity (vectors, tools, scripted ents, models, ...) management environment */
namespace Entity {
	//Component references
	extern DxRenderer::CDxRenderer* pGfxReference;
	extern DxSound::CDxSound* pSndReference;
	extern Scripting::CScriptInt* pScrReference;
	extern class CToolMgr* _pGameToolMgrInstance;
	extern DxRenderer::d3dfont_s* pDefaultFont;
	extern Console::CConsole* pConReference;
	extern std::wstring wszBasePath;

	enum DamageType { DAMAGEABLE_NO = 0, DAMAGEABLE_ALL, DAMAGEABLE_NOTSQUAD };

	struct game_keys_s {
		int vkTrigger, vkClean, vkMenu, vkScrollUp, vkScrollDown, vkTeamSelect, vkConsole, vkTakeScreen, vkKey1, vkKey2, vkKey3, vkKey4, vkKey5, vkKey6, vkKey7, vkKey8, vkKey9, vkKey0, vkExit;
	};

	struct Color {
		Color() {}
		Color(byte cr, byte cg, byte cb, byte ca) : r(cr), g(cg), b(cb), a(ca) {}
		~Color() {}

		byte r, g, b, a;

		byte GetR(void) { return r; }
		byte GetG(void) { return g; }
		byte GetB(void) { return b; }
		byte GetA(void) { return a; }

		//AngelScript interface methods
		void Constr_Bytes(byte cr, byte cg, byte cb, byte ca) { r = cr; g = cg; b = cb; a = ca; }
		void Construct(void* pMemory) { new (pMemory) Color(); }
		void Destruct(void* pMemory) { ((Color*)pMemory)->~Color(); }
	};

	/* Timer utility class */
	class CTimer {
	private:
		bool m_bStarted;
		DWORD m_dwInitial;
		DWORD m_dwCurrent;
		DWORD m_dwDelay;
	public:
		CTimer() {}
		//CTimer() : m_bStarted(false) {}
		CTimer(DWORD dwDelay) { this->SetDelay(dwDelay); this->SetActive(true); }
		~CTimer() {}

		void Reset(void)
		{
			//Reset timer

			this->m_dwCurrent = this->m_dwInitial = GetTickCount();
		}

		void Update(void)
		{
			//Update current value

			this->m_dwCurrent = GetTickCount();
		}

		//Setters
		void SetActive(bool bStatus) { this->m_bStarted = bStatus; }
		void SetDelay(DWORD dwDelay) { this->m_dwDelay = dwDelay; }

		//Getters
		const bool Started(void) const { return this->m_bStarted; }
		const DWORD Delay(void) const { return this->m_dwDelay; }
		const bool Elapsed(void) const { return this->m_dwCurrent >= this->m_dwInitial + this->m_dwDelay; }

		//AngelScript interface methods
		void Constr_Delay(DWORD dwDelay) { this->SetDelay(dwDelay); this->SetActive(true); }
		void Construct(void* pMemory) { new (pMemory) CTimer(); }
		void Destruct(void* pMemory) { ((CTimer*)pMemory)->~CTimer(); }
	};

	/* Config variable management */
	typedef void* PConVar;
	template <typename TDataType> class CConVar {
	private:
		TDataType m_dtDataObject;
	public:
		CConVar() {}
		CConVar(const TDataType& ref) : m_dtDataObject(value) {}
		~CConVar() {}

		//Data accessors
		TDataType GetValue(void) const { return this->m_dtDataObject; }
		void SetValue(const TDataType& ref) { this->m_dtDataObject = value; }

		//AngelScript interface methods
		void Constr_Delay(TDataType value) { this->SetValue(value); }
		void Construct(void* pMemory) { new (pMemory) CConVar(); }
		void Destruct(void* pMemory) { ((CConVar*)pMemory)->~CConVar(); }
	};
	class CConVarManager {
	public:
		enum cvdatatype_e {
			CONVAR_BOOL, CONVAR_INT, CONVAR_FLOAT, CONVAR_STRING
		};
		struct convar_s {
			PConVar pObject;
			cvdatatype_e eDataType;
			std::wstring wszName;
		};
	private:
		std::vector<convar_s> m_vOjbects;

		void Release(void)
		{
			//Release resources

			for (size_t i = 0; i < this->m_vOjbects.size(); i++) {
				delete this->m_vOjbects[i].pObject;
			}

			this->m_vOjbects.clear();
		}

		size_t Find(const std::wstring& wszName)
		{
			//Find ConVar from list

			if (!wszName.length())
				return std::wstring::npos;

			//Search in list and return ID when found
			for (size_t i = 0; i < this->m_vOjbects.size(); i++) {
				if (this->m_vOjbects[i].wszName == wszName)
					return i;
			}

			return std::wstring::npos;
		}

		size_t Find(const PConVar pConVar)
		{
			//Find ConVar from list

			if (!pConVar)
				return std::wstring::npos;

			//Search in list and return ID when found
			for (size_t i = 0; i < this->m_vOjbects.size(); i++) {
				if (this->m_vOjbects[i].pObject == pConVar)
					return i;
			}

			return std::wstring::npos;
		}

		bool Free(const size_t uiId)
		{
			//Free a ConVar

			if (uiId < this->m_vOjbects.size()) {
				//Free memory and remove from list
				delete this->m_vOjbects[uiId].pObject;
				this->m_vOjbects.erase(this->m_vOjbects.begin() + uiId);
			}

			return false;
		}
	public:
		CConVarManager() {}
		~CConVarManager() { this->Release(); }

		PConVar Register(const std::wstring& wszName, const cvdatatype_e eDataType)
		{
			//Register new ConVar

			if (!wszName.length())
				return nullptr;

			//Setup data struct
			convar_s sConVar;
			sConVar.wszName = wszName;
			sConVar.eDataType = eDataType;

			//Allocate object according to type
			switch (eDataType) {
			case CONVAR_BOOL:
				sConVar.pObject = new CConVar<bool>;
				break;
			case CONVAR_INT:
				sConVar.pObject = new CConVar<int>;
				break;
			case CONVAR_FLOAT:
				sConVar.pObject = new CConVar<float>;
				break;
			case CONVAR_STRING:
				sConVar.pObject = new CConVar<std::string>;
				break;
			default:
				return nullptr;
			}

			if (!sConVar.pObject)
				return nullptr;

			//Add to list
			this->m_vOjbects.push_back(sConVar);

			return sConVar.pObject;
		}

		PConVar Query(const std::wstring& wszName)
		{
			//Query ConVar from list

			if (!wszName.length())
				return nullptr;

			//Search in list and return object when found
			for (size_t i = 0; i < this->m_vOjbects.size(); i++) {
				if (this->m_vOjbects[i].wszName == wszName)
					return this->m_vOjbects[i].pObject;
			}

			return nullptr;
		}

		bool Free(const std::wstring& wszName)
		{
			//Free a ConVar

			size_t uiId = this->Find(wszName);
			if (uiId != std::wstring::npos) {
				return this->Free(uiId);
			}

			return false;
		}

		bool Free(const PConVar pConVar)
		{
			//Free a ConVar

			size_t uiId = this->Find(pConVar);
			if (uiId != std::wstring::npos) {
				return this->Free(uiId);
			}

			return false;
		}
	};

	/* Entity screen position manager */
	struct Vector {
		//Constructors
		Vector() {}
		Vector(int x, int y) { elem[0] = x; elem[1] = y; }
		Vector(const Vector& ref) { elem[0] = ref.GetX(); elem[1] = ref.GetY(); }

		//Getters
		inline int GetX(void) const { return elem[0]; }
		inline int GetY(void) const { return elem[1]; }

		//Setters
		inline void SetX(int x) { elem[0] = x; }
		inline void SetY(int y) { elem[1] = y; }

		//Overriden operators
		inline int& operator[](int i) { return elem[i]; }
		inline int operator[](int i) const { return elem[i]; }
		inline bool operator==(const Vector& ref) { return (elem[0] == ref[0]) && (elem[1] == ref[1]); }
		inline void operator=(const Vector& ref) { elem[0] = ref.GetX(); elem[1] = ref.GetY(); }
		inline void operator++(int iIndex) { this->elem[iIndex]++; }
		inline void operator--(int iIndex) { this->elem[iIndex]--; }
		inline Vector operator+(const Vector& v) { Vector res; res[0] = this->elem[0] + v[0]; res[1] = this->elem[1] + v[1]; return res; }
		inline Vector operator-(const Vector& v) { Vector res; res[0] = this->elem[0] - v[0]; res[1] = this->elem[1] - v[1]; return res; }
		inline Vector operator*(const Vector& v) { Vector res; res[0] = this->elem[0] * v[0]; res[1] = this->elem[1] * v[1]; return res; }
		inline Vector operator/(const Vector& v) { Vector res; res[0] = this->elem[0] / v[0]; res[1] = this->elem[1] / v[1]; return res; }

		//Utils
		int Distance(const Vector& ref)
		{
			//Get distance from this to reference vector
			Vector vDiff;
			vDiff[0] = ref[0] - elem[0];
			vDiff[1] = ref[1] - elem[1];
			return (int)(sqrt(vDiff[0] * vDiff[0] + vDiff[1] * vDiff[1]));
		}
		void Zero(void)
		{
			//Zero values
			elem[0] = elem[1] = 0;
		}

		//AngelScript interface methods
		void Constr_Class(const Vector& src) { this->SetX(src.GetX()); this->SetY(src.GetY()); }
		void Constr_Ints(int x, int y) { this->SetX(x); this->SetY(y); }
		void Construct(void* pMemory) { new (pMemory) Vector(); }
		void Destruct(void* pMemory) { ((Vector*)pMemory)->~Vector(); }
	private:
		int elem[2];
	};

	/* Sprite info data container */
	struct SpriteInfo {
		SpriteInfo() {}
		SpriteInfo(const Vector& v, int d, int f) : res(v), depth(d), format(f) {}
		~SpriteInfo() {}

		Vector res;
		int depth;
		int format;

		Vector& GetResolution(void) { return res; }
		int GetDepth(void) { return depth; }
		int GetFormat(void) { return format; }

		//AngelScript interface methods
		void Construct(void* pMemory) { new (pMemory) Color(); }
		void Destruct(void* pMemory) { ((Color*)pMemory)->~Color(); }
	};

	/* Bounding Box handler */
	class CBoundingBox {
	private:
		struct bbox_item_s {
			Vector pos;
			Vector size;
		};

		std::vector<bbox_item_s>* m_pvBBoxItems;

		void Release(void)
		{
			//Clear data

			if (!this->m_pvBBoxItems)
				return;
			
			this->m_pvBBoxItems->clear();

			delete this->m_pvBBoxItems;
		}

		bool IsItemCollided(const Vector& vMyAbsPos, const Vector& vRefAbsPos, const bbox_item_s& rBBoxItem) const
		{
			//Check if bbox item collides with any of own items

			if (!this->m_pvBBoxItems)
				return false;
			
			for (size_t i = 0; i < this->m_pvBBoxItems->size(); i++) { //Iterate through items
				bbox_item_s item = (*this->m_pvBBoxItems)[i];
				//Check if bbox item is inside current iterated item
				if (((vRefAbsPos[0] + rBBoxItem.pos[0] >= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0]) && (vRefAbsPos[0] + rBBoxItem.pos[0] <= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0] + (*this->m_pvBBoxItems)[i].size[0])) && ((vRefAbsPos[1] + rBBoxItem.pos[1] >= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1]) && (vRefAbsPos[1] + rBBoxItem.pos[1] <= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1] + (*this->m_pvBBoxItems)[i].size[1]))) {
					return true;
				}
			}

			return false;
		}
	public:
		CBoundingBox() {}
		~CBoundingBox() { this->Release(); }

		bool Alloc(void)
		{
			//Allocate memory for vector

			this->m_pvBBoxItems = new std::vector<bbox_item_s>;

			return this->m_pvBBoxItems != nullptr;
		}

		void AddBBoxItem(const Vector& pos, const Vector& size)
		{
			//Add bounding box item

			if (!this->m_pvBBoxItems)
				return;

			//Setup data
			bbox_item_s sItem;
			sItem.pos = pos;
			sItem.size = size;

			//Add to list
			this->m_pvBBoxItems->push_back(sItem);
		}

		bool IsCollided(const Vector& vMyAbsPos, const Vector& vRefAbsPos, const CBoundingBox& roBBox)
		{
			//Check if bounding boxes collide with each other

			if (!this->m_pvBBoxItems)
				return false;
			
			if (roBBox.IsEmpty())
				return false;
			
			for (size_t i = 0; i < this->m_pvBBoxItems->size(); i++) { //Iterate through own bbox list
				//Check if current item collides ref bbox
				if (roBBox.IsItemCollided(vRefAbsPos, vMyAbsPos, (*this->m_pvBBoxItems)[i]))
					return true;
			}

			return false;
		}

		bool IsInside(const Vector& vMyAbsPos, const Vector& vPosition)
		{
			//Check if position is inside bbox

			if (!this->m_pvBBoxItems)
				return false;

			if (this->IsEmpty())
				return false;

			for (size_t i = 0; i < this->m_pvBBoxItems->size(); i++) { //Loop through all bbox i tems
				//Check if position is inside
				if ((vPosition[0] >= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0]) && (vPosition[0] <= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0] + (*this->m_pvBBoxItems)[i].size[0]) && (vPosition[1] >= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1]) && (vPosition[1] <= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1] + (*this->m_pvBBoxItems)[i].size[1])) {
					return true;
				}
			}

			return false;
		}

		inline bool IsEmpty(void) const { return (this->m_pvBBoxItems) ? (this->m_pvBBoxItems->size() == 0) : true; }
		inline void Clear(void) { if (this->m_pvBBoxItems) this->m_pvBBoxItems->clear(); }
		inline size_t Count(void) const { return this->m_pvBBoxItems->size(); }
		inline const bbox_item_s& Item(const size_t uiItem) const { return (*this->m_pvBBoxItems)[uiItem]; }

		void operator=(const CBoundingBox& ref)
		{
			//Copy data from other bbox

			this->m_pvBBoxItems->clear();

			for (size_t i = 0; i < ref.Count(); i++) {
				this->m_pvBBoxItems->push_back(ref.Item(i));
			}
		}

		//AngelScript interface methods
		void Construct(void* pMemory) { new (pMemory) CBoundingBox(); }
		void Destruct(void* pMemory) { ((CBoundingBox*)pMemory)->~CBoundingBox(); }
	};

	/* Sprite component */
	class CTempSprite {
	private:
		bool m_bInfinite;
		CTimer m_oTimer;
		DxRenderer::HD3DSPRITE m_hSprite;
		int m_iCurrentFrame;
		int m_iMaxFrames;
	public:
		CTempSprite() {}
		CTempSprite(const std::string& szTexture, DWORD dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize) { this->Initialize(szTexture, dwSwitchDelay, bInfinite, iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize); }
		~CTempSprite() {}

		bool Initialize(const std::string& szTexture, DWORD dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize)
		{
			//Initialize sprite object

			//Load sprite
			this->m_hSprite = pGfxReference->LoadSprite(Utils::ConvertToWideString(szTexture), iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize);
			if (this->m_hSprite == GFX_INVALID_SPRITE_ID)
				return false;

			//Initialize timer
			this->m_oTimer = CTimer(dwSwitchDelay);

			//Store indicator
			this->m_bInfinite = bInfinite;

			//Init frame values
			this->m_iCurrentFrame = 0;
			this->m_iMaxFrames = iFrameCount;

			return true;
		}

		void Draw(const Vector& pos, float fRotation, float fScale1, float fScale2)
		{
			//Draw sprite items

			if (this->m_hSprite == GFX_INVALID_SPRITE_ID)
				return;

			//Draw current sprite frame
			pGfxReference->DrawSprite(this->m_hSprite, pos[0], pos[1], this->m_iCurrentFrame, fRotation, fScale1, fScale2);

			//Update timer
			this->m_oTimer.Update();

			//Handle frame change
			if (this->m_oTimer.Elapsed()) {
				this->m_iCurrentFrame++;
				if (this->m_iCurrentFrame >= this->m_iMaxFrames) {
					this->m_iCurrentFrame = 0;

					if (!this->m_bInfinite) {
						this->Release();
					}
				}
			}
		}

		void Release(void)
		{
			//Release resources

			if (this->m_hSprite == GFX_INVALID_SPRITE_ID)
				return;

			//Free sprite
			pGfxReference->FreeSprite(this->m_hSprite);

			//Stop timer
			this->m_oTimer.SetActive(false);

			//Clear value
			this->m_hSprite = GFX_INVALID_SPRITE_ID;
		}

		//AngelScript interface methods
		void Constr_Init(const std::string& szTexture, DWORD dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize) { this->Initialize(szTexture, dwSwitchDelay, bInfinite, iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize); }
		void Construct(void* pMemory) { new (pMemory) CTempSprite(); }
		void Destruct(void* pMemory) { ((CTempSprite*)pMemory)->~CTempSprite(); }
	};

	/* Model component */
	class CModel {
	private:
		bool m_bReady;
		DxRenderer::HD3DSPRITE m_hSprite;
		CBoundingBox m_oBBox;
		Vector m_vCenter;
	public:
		CModel() {}
		//CModel(const std::string& szInputFile) { this->Initialize(szInputFile); }
		//CModel(const std::string& szInputFile, bool bForceCustomSize) { this->Initialize(szInputFile, bForceCustomSize); }
		~CModel() { this->Release(); }

		bool Initialize(const std::string& szMdlFile, bool bForceCustomSize = false)
		{
			//Initialize model by using source file as input
			
			std::ifstream hFile;
			std::string szSpriteFile;
			int iaFrameInfos[4];

			//Allocate bounding box
			//if (!this->m_oBBox.Alloc())
			//	return false;

			//Attempt to open file for reading
			hFile.open(szMdlFile, std::ifstream::in);
			if (!hFile.is_open())
				return false;

			char szCurLine[MAX_PATH];

			//Read first line to get the model sprite file
			hFile.getline(szCurLine, sizeof(szCurLine), '\n');
			szSpriteFile = szCurLine;

			//Read second line to get the sprite infos
			hFile.getline(szCurLine, sizeof(szCurLine), '\n');
			std::vector<std::string> vSplit = Utils::Split(szCurLine, " ");
			if (vSplit.size() != 4) { hFile.close(); return false; }
			iaFrameInfos[0] = atoi(vSplit[0].c_str());
			iaFrameInfos[1] = atoi(vSplit[1].c_str());
			iaFrameInfos[2] = atoi(vSplit[2].c_str());
			iaFrameInfos[3] = atoi(vSplit[3].c_str());

			//Read remaining lines to get the bbox infos
			while (!hFile.eof()) {
				hFile.getline(szCurLine, sizeof(szCurLine), '\n');

				Vector vPos, vSize;

				vSplit = Utils::Split(szCurLine, " ");
				if (vSplit.size() != 4) { hFile.close(); return false; }
				vPos[0] = atoi(vSplit[0].c_str());
				vPos[1] = atoi(vSplit[1].c_str());
				vSize[0] = atoi(vSplit[2].c_str());
				vSize[1] = atoi(vSplit[3].c_str());

				//Add to bbox
				this->m_oBBox.AddBBoxItem(vPos, vSize);
			}

			//Close file
			hFile.close();

			//Load sprite
			this->m_hSprite = pGfxReference->LoadSprite(Utils::ConvertToWideString(szSpriteFile), iaFrameInfos[0], iaFrameInfos[1], iaFrameInfos[2], iaFrameInfos[3], bForceCustomSize);
			if (!this->m_hSprite)
				return false;

			//Return set indicator
			return this->m_bReady = true;
		}

		bool Initialize2(const CBoundingBox& bbox, const DxRenderer::HD3DSPRITE hSprite)
		{
			//Initialize model by using pre-initialized data objects

			//Copy data
			this->m_hSprite = hSprite;
			this->m_oBBox = bbox;

			//Return set indicator
			return this->m_bReady = true;
		}

		void Release(void)
		{
			//Release resources

			if (!this->m_bReady)
				return;

			//Free sprite
			pGfxReference->FreeSprite(this->m_hSprite);

			//Clear bbox data
			this->m_oBBox.Clear();

			//Clear indicator
			this->m_bReady = false;
		}

		bool IsCollided(const Vector& vMyAbsPos, const Vector& vRefAbsPos, const CModel& rRefMdl)
		{
			//Check for collision

			return this->m_oBBox.IsCollided(vMyAbsPos, vRefAbsPos, rRefMdl.BBox());
		}

		bool IsInside(const Vector& vMyAbsPos, const Vector& vPosition)
		{
			//Check if inside

			return this->m_oBBox.IsInside(vMyAbsPos, vPosition);
		}

		inline bool Alloc(void) { return this->m_oBBox.Alloc(); }
		inline void SetCenter(const Vector& vCenter) { this->m_vCenter = vCenter; }

		//Getters
		inline bool IsValid(void) const { return this->m_bReady; }
		inline const CBoundingBox& BBox(void) const { return this->m_oBBox; }
		inline const DxRenderer::HD3DSPRITE Sprite(void) const { return this->m_hSprite; }
		inline const Vector& Center(void) const { return this->m_vCenter; }

		//Assign operator
		void operator=(const CModel& ref)
		{
			this->m_hSprite = ref.Sprite();
			this->m_oBBox = ref.BBox();
			this->m_vCenter = ref.Center();
		}

		//AngelScript interface methods
		void Constr_IntputStr(const std::string& szInputFile) { this->Initialize(szInputFile); }
		void Constr_IntputStrBool(const std::string& szInputFile, bool bForceCustomSize) { this->Initialize(szInputFile, bForceCustomSize); }
		void Construct(void* pMemory) { new (pMemory) CModel(); }
		void Destruct(void* pMemory) { ((CModel*)pMemory)->~CModel(); }
	};

	/* Managed entity component */
	class CScriptedEntity {
	public:
		typedef byte DamageValue;
	private:
		std::string m_szClassName;
		Scripting::HSISCRIPT m_hScript;
		asIScriptObject* m_pScriptObject;

		void Release(void)
		{
			//Release resources

			this->m_pScriptObject->Release();
			this->m_pScriptObject = nullptr;
		}
	public:
		CScriptedEntity(const Scripting::HSISCRIPT hScript, asIScriptObject* pObject) : m_pScriptObject(pObject), m_hScript(hScript) {}
		CScriptedEntity(const Scripting::HSISCRIPT hScript, const std::string& szClassName) : m_szClassName(szClassName) { this->Initialize(hScript, szClassName); }
		~CScriptedEntity() { this->Release(); }

		bool Initialize(const Scripting::HSISCRIPT hScript, const std::string& szClassName)
		{
			//Initialize scripted entity

			if (!szClassName.length())
				return false;

			//Allocate class instance
			this->m_pScriptObject = pScrReference->AllocClass(hScript, szClassName);

			//Store data
			this->m_hScript = hScript;

			return this->m_pScriptObject != nullptr;
		}

		void OnSpawn(const Vector& vAtPos)
		{
			//Inform class instance of event

			Vector v(vAtPos);

			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT(&v);

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnSpawn(const Vector& in)", &vArgs, nullptr);

			END_PARAMS(vArgs);
		}

		void OnRelease(void)
		{
			//Inform class instance of event

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnRelease()", nullptr, nullptr);
		}

		void OnProcess(void)
		{
			//Inform class instance of event

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnProcess()", nullptr, nullptr);
		}

		void OnDraw(void)
		{
			//Inform class instance of event

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnDraw()", nullptr, nullptr);
		}

		void OnDrawOnTop(void)
		{
			//Inform class instance of event

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnDrawOnTop()", nullptr, nullptr);
		}

		bool DoUserCleaning(void)
		{
			//Query whether entity shall be cleaned by user

			bool bResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "bool DoUserCleaning()", nullptr, &bResult, Scripting::FA_BYTE);

			return bResult;
		}

		byte IsDamageable(void)
		{
			//Query if entity is damageable (0 = non-damageable, 1 = damage all, 2 = damage only entities with different name)

			byte ucResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "DamageType IsDamageable()", nullptr, &ucResult, Scripting::FA_BYTE);

			return ucResult;
		}

		void OnDamage(DamageValue dv)
		{
			//Inform of being damaged

			BEGIN_PARAMS(vArgs);
			PUSH_BYTE(dv);

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnDamage(DamageValue dv)", &vArgs, nullptr);

			END_PARAMS(vArgs);
		}

		CModel* GetModel(void)
		{
			//Query model object pointer

			CModel* pResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "Model& GetModel()", nullptr, &pResult, Scripting::FA_OBJECT);

			return pResult;
		}

		Vector GetPosition(void)
		{
			//Query position

			Vector* pResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "Vector& GetPosition()", nullptr, &pResult, Scripting::FA_OBJECT);

			return *pResult;
		}

		float GetRotation(void)
		{
			//Query rotation

			float flResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "float GetRotation()", nullptr, &flResult, Scripting::FA_FLOAT);

			return flResult;
		}

		bool IsMovable(void)
		{
			//Query movable indicator

			bool bResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "bool IsMovable()", nullptr, &bResult, Scripting::FA_BYTE);

			return bResult;
		}

		Vector GetSelectionSize(void)
		{
			//Query selection size

			Vector* pResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "Vector& GetSelectionSize()", nullptr, &pResult, Scripting::FA_OBJECT);

			return *pResult;
		}

		DamageValue GetDamageValue(void)
		{
			//Get damage value

			DamageValue dvResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "DamageValue GetDamageValue()", nullptr, &dvResult, Scripting::FA_BYTE);

			return dvResult;
		}

		bool NeedsRemoval(void)
		{
			//Inform class instance of event

			bool bResult;

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "bool NeedsRemoval()", nullptr, &bResult, Scripting::FA_BYTE);

			return bResult;
		}

		std::string GetName(void)
		{
			//Get name from class instance

			std::string szResult;
			
			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "string GetName()", nullptr, &szResult, Scripting::FA_STRING);

			return szResult;
		}

		void MoveTo(const Vector& vToPos)
		{
			//Inform to move to a target

			Vector v(vToPos);

			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT(&v);

			pScrReference->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void MoveTo(const Vector& in vec)", &vArgs, nullptr);

			END_PARAMS(vArgs);
		}

		//Getters
		inline bool IsReady(void) const { return (this->m_pScriptObject != nullptr); }
		inline asIScriptObject* Object(void) const { return this->m_pScriptObject; }
	};

	/* Scripted entity manager */
	class CScriptedEntsMgr {
	private:
		std::vector<CScriptedEntity*> m_vEnts;

		void Release(void)
		{
			//Release resources

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				this->m_vEnts[i]->OnRelease();
				delete this->m_vEnts[i];
			}

			this->m_vEnts.clear();
		}
	public:
		CScriptedEntsMgr() {}
		~CScriptedEntsMgr() { this->Release(); }

		bool Spawn(const Scripting::HSISCRIPT hScript, asIScriptObject* pObject, const Vector& vAtPos)
		{
			//Spawn new entity

			//Instantiate entity object
			CScriptedEntity* pEntity = new CScriptedEntity(hScript, pObject);
			if (!pEntity)
				return false;

			//Check if ready
			if (!pEntity->IsReady()) {
				delete pEntity;
				return false;
			}

			//Inform of being spawned
			pEntity->OnSpawn(vAtPos);

			//Add to list
			this->m_vEnts.push_back(pEntity);

			return true;
		}

		void Process(void)
		{
			//Inform entities

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				//Let entity process
				this->m_vEnts[i]->OnProcess();

				//Handle damaging
				byte ucDamageType = this->m_vEnts[i]->IsDamageable(); //Query damage type
				if (ucDamageType) {
					//Query model
					CModel* pModel = this->m_vEnts[i]->GetModel();
					if (pModel) {
						//Check for collisions with any other entities
						for (size_t j = 0; j < this->m_vEnts.size(); j++) {
							if ((j != i) && (this->m_vEnts[j]->IsDamageable())) {
								//Check if entity does not damage other entities with the same name and ignore damage handling then
								if (ucDamageType == DAMAGEABLE_NOTSQUAD) {
									if (this->m_vEnts[i]->GetName() == this->m_vEnts[j]->GetName())
										continue;
								}

								CModel* pRef = this->m_vEnts[j]->GetModel();
								if (pRef) {
									//Check if collided
									if (pModel->IsCollided(this->m_vEnts[i]->GetPosition(), this->m_vEnts[j]->GetPosition(), *pRef)) {
										//Inform both of being damaged
										this->m_vEnts[i]->OnDamage(this->m_vEnts[j]->GetDamageValue());
										this->m_vEnts[j]->OnDamage(this->m_vEnts[i]->GetDamageValue());
									}
								}
							}
						}
					}
				}

				//Check for removal
				if (this->m_vEnts[i]->NeedsRemoval()) {
					this->m_vEnts[i]->OnRelease();
					delete this->m_vEnts[i];
					this->m_vEnts.erase(this->m_vEnts.begin() + i);
				}
			}
		}

		void Draw(void)
		{
			//Inform entities

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				this->m_vEnts[i]->OnDraw();
			}
		}

		void DrawOnTop(void)
		{
			//Inform entities

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				this->m_vEnts[i]->OnDrawOnTop();
			}
		}

		void OnUserClean(void)
		{
			//Cleanup entities which indicate permission of user cleaning

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->DoUserCleaning()) {
					this->m_vEnts[i]->OnRelease();
					delete this->m_vEnts[i];
					this->m_vEnts.erase(this->m_vEnts.begin() + i);
				}
			}
		}

		//Entity querying
		size_t GetEntityCount() { return this->m_vEnts.size(); }
		CScriptedEntity* GetEntity(size_t uiEntityId)
		{
			if (uiEntityId >= this->m_vEnts.size())
				return nullptr;

			return this->m_vEnts[uiEntityId];
		}
		asIScriptObject* GetEntityHandle(size_t uiEntityId)
		{
			if (uiEntityId >= this->m_vEnts.size())
				return nullptr;

			if (!this->m_vEnts[uiEntityId])
				return nullptr;

			return this->m_vEnts[uiEntityId]->Object();
		}
		bool IsValidEntity(asIScriptObject* pEntity)
		{
			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->Object() == pEntity)
					return true;
			}

			return false;
		}
		size_t GetEntityId(asIScriptObject* pEntity)
		{
			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->Object() == pEntity)
					return true;
			}

			return (size_t)-1;
		}
	};

	extern CScriptedEntsMgr oScriptedEntMgr;

	class CEntityTrace { //Entity tracing utility class
	private:
		struct tracedata_s {
			Vector vStart;
			Vector vEnd;
			asIScriptObject* pIgnoreEnt;
		};

		std::vector<CScriptedEntity*> m_vEntities;
		tracedata_s m_sTraceData;

		void IncreaseVec(Vector& vVector)
		{
			//Increase vector value to move to the next position in direction to the end position
			
			if (vVector[0] != this->m_sTraceData.vEnd[0]) { (this->m_sTraceData.vEnd[0] - vVector[0] > 0) ? vVector[0]++ : vVector[0]--; }
			if (vVector[1] != this->m_sTraceData.vEnd[1]) { (this->m_sTraceData.vEnd[1] - vVector[1] > 0) ? vVector[1]++ : vVector[1]--; }
		}

		bool EndReached(const Vector& vCurPos)
		{
			//Indicate whether end has been reached

			return (vCurPos[0] == this->m_sTraceData.vEnd[0]) && (vCurPos[1] == this->m_sTraceData.vEnd[1]);
		}

		void EnumerateEntitiesAtPosition(const Vector& vPosition, asIScriptObject* pIgnoreEnt)
		{
			//Create a list with all entities at the given position

			//Clear list
			this->m_vEntities.clear();

			for (size_t i = 0; i < oScriptedEntMgr.GetEntityCount(); i++) { //Loop through all entities
				CScriptedEntity* pEntity = oScriptedEntMgr.GetEntity(i); //Get entity pointer
				if ((pEntity) && (pEntity->Object() != pIgnoreEnt) && (pEntity->IsDamageable())) { //Use only valid damageable entities
					//Obtain model pointer
					CModel* pModel = pEntity->GetModel(); 
					if (pModel) {
						if (pModel->IsInside(pEntity->GetPosition(), vPosition)) { //Check if position is inside
							//Add to list
							this->m_vEntities.push_back(pEntity);
						}
					}
				}
			}
		}
	public:
		CEntityTrace() {}
		CEntityTrace(const Vector& vStart, const Vector& vEnd, asIScriptObject* pIgnoreEnt)
		{
			this->SetStart(vStart);
			this->SetEnd(vEnd);
			this->SetIgnoredEnt(pIgnoreEnt);
			this->Run();
		}
		~CEntityTrace() { this->m_vEntities.clear(); }

		//Action
		void Run(void)
		{
			//Run trace computation

			//Initialize with start position
			Vector vCurrentPosition = this->m_sTraceData.vStart;

			while (!this->EndReached(vCurrentPosition)) { //Loop as long as end position has not yet been reached
				//Enumerate all entities at current position
				this->EnumerateEntitiesAtPosition(vCurrentPosition, this->m_sTraceData.pIgnoreEnt);
				if (!this->IsEmpty()) { //Break out if at least one entity has been found
					break;
				}

				//Update current position
				this->IncreaseVec(vCurrentPosition);
			}
		}

		//Setters
		inline void SetStart(const Vector& vVector) { this->m_sTraceData.vStart = vVector; }
		inline void SetEnd(const Vector& vVector) { this->m_sTraceData.vEnd = vVector; }
		inline void SetIgnoredEnt(asIScriptObject* pIgnoreEnt) { this->m_sTraceData.pIgnoreEnt = pIgnoreEnt; }

		//Getters
		inline const bool IsEmpty(void) const { return this->m_vEntities.size() == 0; }
		inline asIScriptObject* EntityObject(const size_t uiId) { if (uiId >= this->m_vEntities.size()) return nullptr; return this->m_vEntities[uiId]->Object(); }
		inline const size_t EntityCount(void) const { return this->m_vEntities.size(); }
	};

	/* File reader class */
	class CFileReader {
	public:
		enum SeekWay_e {
			SEEKW_BEGIN = std::ios_base::beg,
			SEEKW_CURRENT = std::ios_base::cur,
			SEEKW_END = std::ios_base::end
		};
	private:
		std::ifstream* m_poFile;

		void Release(void)
		{
			//Release resources

			if (this->m_poFile) {
				if (this->m_poFile->is_open()) {
					this->m_poFile->close();
				}

				delete this->m_poFile;
				this->m_poFile = nullptr;
			}
		}
	public:
		CFileReader() {}
		~CFileReader() { /*this->Release();*/ }

		bool Open(const std::string& szFile)
		{
			//Allocate object and open stream to file

			//if (this->m_poFile)
			//	return false;

			this->m_poFile = new std::ifstream();
			if (!this->m_poFile)
				return false;

			this->m_poFile->open(Utils::ConvertToAnsiString(wszBasePath) + "tools\\" + szFile, std::ifstream::in);

			return this->m_poFile->is_open();
		}

		bool IsOpen(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return false;

			return this->m_poFile->is_open();
		}

		bool Eof(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return true;

			return this->m_poFile->eof();
		}

		void Seek(int from, int offset)
		{
			//Set file pointer

			if (!this->m_poFile)
				return;

			this->m_poFile->seekg(offset, from);
		}

		std::string GetLine(void)
		{
			//Return current line content

			if (!this->m_poFile)
				return "";

			char szLineBuffer[1024 * 4] = { 0 };

			this->m_poFile->getline(szLineBuffer, sizeof(szLineBuffer), '\n');

			return std::string(szLineBuffer);
		}

		std::string GetEntireContent(bool bSkipNLChar)
		{
			//Return entire file content

			if (!this->m_poFile)
				return "";

			std::string szAllLines;
			this->m_poFile->seekg(0, std::ios_base::beg);
			while (!this->m_poFile->eof()) {
				szAllLines += this->GetLine() + ((bSkipNLChar) ? "" : "\n");
			}
			
			return szAllLines;
		}

		void Close(void)
		{
			//Close stream and free memory

			this->Release();
		}

		//AngelScript interface methods
		void Constr_Init(const std::string& szFile) { this->Open(szFile); }
		void Construct(void* pMemory) { new (pMemory) CFileReader(); }
		void Destruct(void* pMemory) { ((CFileReader*)pMemory)->~CFileReader(); }
	};

	/* File writer class */
	class CFileWriter {
	public:
		enum SeekWay_e {
			SEEKW_BEGIN = std::ios_base::beg,
			SEEKW_CURRENT = std::ios_base::cur,
			SEEKW_END = std::ios_base::end
		};
	private:
		std::ofstream* m_poFile;

		void Release(void)
		{
			//Release resources

			if (this->m_poFile) {
				if (this->m_poFile->is_open()) {
					this->m_poFile->close();
				}

				delete this->m_poFile;
				this->m_poFile = nullptr;
			}
		}
	public:
		CFileWriter() {}
		~CFileWriter() { /*this->Release();*/ }

		bool Open(const std::string& szFile, bool bAppend = true)
		{
			//Allocate object and open stream to file

			//if (this->m_poFile)
			//	return false;

			this->m_poFile = new std::ofstream();
			if (!this->m_poFile)
				return false;

			int openMode = std::ifstream::out;
			if (bAppend) openMode |= std::ifstream::app;
			
			this->m_poFile->open(Utils::ConvertToAnsiString(wszBasePath) + "tools\\" + szFile, openMode);

			return this->m_poFile->is_open();
		}

		bool IsOpen(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return false;

			return this->m_poFile->is_open();
		}

		bool Eof(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return true;

			return this->m_poFile->eof();
		}

		void Seek(int from, int offset)
		{
			//Set file pointer

			if (!this->m_poFile)
				return;

			this->m_poFile->seekp(offset, from);
		}

		void Write(const std::string& szText)
		{
			//Write text to file

			if (!this->m_poFile)
				return;

			this->m_poFile->write(szText.data(), szText.length());
		}

		void WriteLine(const std::string& szText)
		{
			//Write text line to file

			this->Write(szText + "\n");
		}

		void Close(void)
		{
			//Close stream and free memory

			this->Release();
		}

		//AngelScript interface methods
		void Constr_Init(const std::string& szFile) { this->Open(szFile); }
		void Construct(void* pMemory) { new (pMemory) CFileWriter(); }
		void Destruct(void* pMemory) { ((CFileWriter*)pMemory)->~CFileWriter(); }
	};

	namespace APIFuncs { //API functions to be called by scripts
		void Print(const std::string& in);
		void Print2(const std::string& in, const Console::ConColor& clr);
		void SetCursorOffset(const Vector& vOffset);
		void SetCursorRotation(float fRotation);
		DxRenderer::d3dfont_s* LoadFont(const std::string& szFontName, byte ucFontSizeW, byte ucFontSizeH);
		bool GetSpriteInfo(const std::string& szFile, SpriteInfo& out);
		DxRenderer::HD3DSPRITE LoadSprite(const std::string& szTexture, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize);
		bool FreeSprite(DxRenderer::HD3DSPRITE hSprite);
		bool DrawBox(const Vector& pos, const Vector& size, int iThickness, const Color& color);
		bool DrawFilledBox(const Vector& pos, const Vector& size, const Color& color);
		bool DrawLine(const Vector& start, const Vector& end, const Color& color);
		bool DrawSprite(const DxRenderer::HD3DSPRITE hSprite, const Vector& pos, int iFrame, float fRotation, const Vector& vRotPos, float fScale1, float fScale2, const bool bUseCustomColorMask, const Color& color);
		bool DrawString(const DxRenderer::d3dfont_s* pFont, const std::string& szText, const Vector& pos, const Color& color);
		DxSound::HDXSOUND QuerySound(const std::string& szSoundFile);
		bool PlaySound_(DxSound::HDXSOUND hSound, long lVolume);
		int GetWindowCenterX(void);
		int GetWindowCenterY(void);
		bool SpawnScriptedEntity(asIScriptObject* ref, const Vector& vPos);
		size_t GetEntityCount();
		asIScriptObject* GetEntityHandle(size_t uiEntityId);
		asIScriptObject* EntityTrace(const Vector& vStart, const Vector& vEnd, asIScriptObject* pIgnoredEnt);
		bool Ent_IsValid(asIScriptObject* pEntity);
		size_t Ent_GetId(asIScriptObject* pEntity);
		bool ListSprites(const std::string& szBaseDir, asIScriptFunction* pFunction);
		bool ListSounds(const std::string& szBaseDir, asIScriptFunction* pFunction);
		int Random(int start, int end);
		PConVar RegisterConVar(const std::string& szName, const CConVarManager::cvdatatype_e eType);
		PConVar QueryConVar(const std::string& szName);
		bool FreeConVar(const std::string& szName);
	}
	
	/* Tool component */
	class CGameTool {
	private:
		Scripting::HSISCRIPT m_hScript;
	public:
		CGameTool(Scripting::HSISCRIPT self) : m_hScript(self) {}
		~CGameTool() {}

		bool OnInitialize(void)
		{
			//Initialize tool
			//bool CDG_API_Initialize()

			bool bResult = false;

			//Attempt to call function
			if (!pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_Initialize", nullptr, &bResult, Scripting::FA_BYTE)) {
				return false;
			}

			return bResult;
		}

		void OnProcess(void)
		{
			//Process tool
			//void CDG_API_Process()

			pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_Process", nullptr, nullptr);
		}

		void OnDraw(void)
		{
			//Let tool draw its stuff
			//void CDG_API_Draw()

			pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_Draw", nullptr, nullptr);
		}

		void OnDrawOnTop(void)
		{
			//Let tool draw its stuff on top
			//void CDG_API_DrawOnTop()

			pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_DrawOnTop", nullptr, nullptr);
		}

		void OnKeyEvent(int iKey, bool bDown)
		{
			//Inform of key event
			//void CDG_API_KeyEvent(int iKey, bool bDown)

			//Setup call stack
			BEGIN_PARAMS(vArgs);
			PUSH_DWORD(iKey);
			PUSH_BOOL(bDown);

			//Attempt to call function
			if (!pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_KeyEvent", &vArgs, NULL)) {
				END_PARAMS(vArgs);
				return;
			}

			//Clear list
			END_PARAMS(vArgs);
		}

		void OnMouseEvent(const Vector& vCoords, int iKey, bool bDown)
		{
			//Inform of mouse event
			//void CDG_API_MouseEvent(const Vector in& coords, int iKey, bool bDown)

			//Setup call stack
			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT((void*)&vCoords);
			PUSH_DWORD(iKey);
			PUSH_BOOL(bDown);

			//Attempt to call function
			if (!pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_MouseEvent", &vArgs, NULL)) {
				END_PARAMS(vArgs);
				return;
			}

			//Clear list
			END_PARAMS(vArgs);
		}

		void OnSelect(bool bSelectionStatus)
		{
			//Inform of selection status
			//void CDG_API_SelectionStatus(bool bSelectionStatus)

			//Setup call stack
			BEGIN_PARAMS(vArgs);
			PUSH_BOOL(bSelectionStatus);

			//Attempt to call function
			if (!pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_SelectionStatus", &vArgs, NULL)) {
				END_PARAMS(vArgs);
				return;
			}

			//Clear list
			END_PARAMS(vArgs);
		}

		void OnTrigger(const Vector &vAtPos)
		{
			//Inform of being triggered
			//void CDG_API_Trigger(const Vector &in)

			Vector v(vAtPos);

			//Setup call stack
			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT(&v);

			pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_Trigger", &vArgs, nullptr);

			//Clear list
			END_PARAMS(vArgs);
		}

		void OnRelease(void)
		{
			//Let tool release its resources
			//void CDG_API_Release()

			pScrReference->CallScriptFunction(this->m_hScript, true, "CDG_API_Release", nullptr, nullptr);
		}

		bool IsReady(void) const { return this->m_hScript != SI_INVALID_ID; }
	};

	/* Tool manager component */
	class CToolMgr {
	public:
		typedef size_t HTOOL;
		static const HTOOL InvalidToolHandle = (HTOOL)-1;
		struct ToolInfo {
			std::string szName;
			std::string szVersion;
			std::string szAuthor;
			std::string szContact;
			std::string szPreviewImage;
			std::string szCursor;
			std::string szCategory;
			int iCursorWidth, iCursorHeight;
			DWORD uiTriggerDelay;
		};
		struct gt_list_item_s {
			CGameTool* pGameTool;
			ToolInfo sInfo;
			Scripting::HSISCRIPT hScript;
			CTimer oTriggerTimer;
			DxRenderer::HD3DSPRITE hPreviewImg;
			DxRenderer::HD3DSPRITE hCursor;
			Vector vCursorOffset;
			float fCursorRotation;
			std::string szPath;
			std::string szScript;
			std::string szToolScript;
		};
	private:
		std::vector<gt_list_item_s> m_vTools;
		HTOOL m_hSelectedTool;
		Vector m_vMousePos;
		std::vector<CScriptedEntity*> m_vSelectedEntities;
		bool m_bSurpressSelToolForwarding;
		DxRenderer::HD3DSPRITE m_hGotoCursor;
		DxRenderer::HD3DSPRITE m_hPointer;
		bool m_bDrawPointer;
		bool m_bCtrlHeld;

		bool QueryToolInfo(gt_list_item_s& sGtData, const game_keys_s& gamekeys)
		{
			//Query tool info
			//bool CDG_API_QueryToolInfo(HostVersion hvVersion, EntityInfo &out info, const game_keys_s &in, const string &in szToolPath)

			bool bResult = false;

			game_keys_s gkeys = gamekeys;

			//Setup call stack
			BEGIN_PARAMS(vArgs);
			PUSH_WORD(DNY_CDG_HOST_VERSION_W);
			PUSH_POINTER(&sGtData.sInfo);
			PUSH_OBJECT(&gkeys);
			PUSH_OBJECT(&sGtData.szPath);
			
			//Attempt to call function
			if (!pScrReference->CallScriptFunction(sGtData.hScript, true, "CDG_API_QueryToolInfo", &vArgs, &bResult, Scripting::FA_BYTE)) {
				END_PARAMS(vArgs);
				return false;
			}

			//Clear list
			END_PARAMS(vArgs);

			return bResult;
		}

		HTOOL LoadTool(const std::wstring& wszAbsoluteToolScript, const game_keys_s &gamekeys)
		{
			//Load tool from script

			if (!wszAbsoluteToolScript.length())
				return InvalidToolHandle;

			gt_list_item_s sToolInfo;
			sToolInfo.szScript = Utils::ConvertToAnsiString(wszAbsoluteToolScript);
			sToolInfo.szPath = Utils::ConvertToAnsiString(Utils::ExtractFilePath(wszAbsoluteToolScript));
			sToolInfo.szToolScript = Utils::ConvertToAnsiString(Utils::ExtractFileName(wszAbsoluteToolScript));
			sToolInfo.vCursorOffset.Zero();
			sToolInfo.fCursorRotation = 0.0f;

			//Execute script
			sToolInfo.hScript = pScrReference->LoadScript(Utils::ConvertToAnsiString(wszAbsoluteToolScript));
			if (sToolInfo.hScript == SI_INVALID_ID)
				return InvalidToolHandle;

			//Query tool info
			if (!this->QueryToolInfo(sToolInfo, gamekeys)) {
				pScrReference->UnloadScript(sToolInfo.hScript);
				return InvalidToolHandle;
			}

			//Allocate tool instance
			sToolInfo.pGameTool = new CGameTool(sToolInfo.hScript);
			if (!sToolInfo.pGameTool) {
				pScrReference->UnloadScript(sToolInfo.hScript);
				return InvalidToolHandle;
			}

			//Load preview image
			sToolInfo.hPreviewImg = pGfxReference->LoadSprite(Utils::ExtractFilePath(wszAbsoluteToolScript) + Utils::ConvertToWideString(sToolInfo.sInfo.szPreviewImage), 1, 195, 90, 1, true);
			if (sToolInfo.hPreviewImg == GFX_INVALID_SPRITE_ID) {
				pScrReference->UnloadScript(sToolInfo.hScript);
				return InvalidToolHandle;
			}

			//Load cursor image
			sToolInfo.hCursor = pGfxReference->LoadSprite(Utils::ExtractFilePath(wszAbsoluteToolScript) + Utils::ConvertToWideString(sToolInfo.sInfo.szCursor), 1, sToolInfo.sInfo.iCursorWidth, sToolInfo.sInfo.iCursorHeight, 1, true);
			if (sToolInfo.hCursor == GFX_INVALID_SPRITE_ID) {
				pGfxReference->FreeSprite(sToolInfo.hPreviewImg);
				pScrReference->UnloadScript(sToolInfo.hScript);
				return InvalidToolHandle;
			}

			//Inform tool of initialization
			if (!sToolInfo.pGameTool->OnInitialize()) {
				delete sToolInfo.pGameTool;
				pGfxReference->FreeSprite(sToolInfo.hPreviewImg);
				pGfxReference->FreeSprite(sToolInfo.hCursor);
				return InvalidToolHandle;
			}

			//Set trigger timer data
			sToolInfo.oTriggerTimer.SetDelay(sToolInfo.sInfo.uiTriggerDelay);
			sToolInfo.oTriggerTimer.SetActive(false);
			sToolInfo.oTriggerTimer.Reset();

			//Add to list
			this->m_vTools.push_back(sToolInfo);

			return this->m_vTools.size() - 1;
		}

		void Release(void)
		{
			//Release resources

			//Free all tools
			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				//Inform of unload event
				this->m_vTools[i].pGameTool->OnRelease();

				//Free sprites
				pGfxReference->FreeSprite(this->m_vTools[i].hPreviewImg);
				pGfxReference->FreeSprite(this->m_vTools[i].hCursor);

				//Free object memory
				delete this->m_vTools[i].pGameTool;
				
				//Unload script
				pScrReference->UnloadScript(this->m_vTools[i].hScript);
			}

			//Clear list
			this->m_vTools.clear();

			//Free sprites
			pGfxReference->FreeSprite(this->m_hGotoCursor);
			pGfxReference->FreeSprite(this->m_hPointer);
		}
	public:
		CToolMgr() : m_hSelectedTool(InvalidToolHandle), m_bSurpressSelToolForwarding(false), m_bDrawPointer(false), m_bCtrlHeld(false)
		{ 
			_pGameToolMgrInstance = this;

			this->m_hGotoCursor = pGfxReference->LoadSprite(L"res\\gotocursor.png", 1, 32, 32, 1, false);
			this->m_hPointer = pGfxReference->LoadSprite(L"res\\pointer.png", 1, 72, 72, 1, false);
		}
		~CToolMgr() { this->Release(); }


		inline HTOOL LoadToolFromName(const std::wstring& wszToolScript, const game_keys_s &gamekeys)
		{
			//Load tool from script

			return this->LoadTool(L"tools\\" + wszToolScript + L"\\" + wszToolScript + L".as", gamekeys);
		}
		
		inline HTOOL LoadToolFromPath(const std::wstring& wszToolPath, const game_keys_s &gamekeys)
		{
			//Load tool from path

			return this->LoadTool(wszToolPath, gamekeys);
		}

		void Process(void)
		{
			//Process stuff

			//Inform tools
			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				this->m_vTools[i].pGameTool->OnProcess();
			}

			//Inform scripted entities
			oScriptedEntMgr.Process();

			//Handle trigger
			if (this->m_hSelectedTool != InvalidToolHandle) {
				if (this->m_vTools[this->m_hSelectedTool].oTriggerTimer.Started()) {
					this->m_vTools[this->m_hSelectedTool].oTriggerTimer.Update();
					if (this->m_vTools[this->m_hSelectedTool].oTriggerTimer.Elapsed()) {
						//Needed to not inform the selected tool of a part of mouse click when handling entity selection
						if (!this->m_bSurpressSelToolForwarding) {
							this->m_vTools[this->m_hSelectedTool].pGameTool->OnTrigger(this->m_vMousePos);
						} else {
							this->m_bSurpressSelToolForwarding = false;
						}
						this->m_vTools[this->m_hSelectedTool].oTriggerTimer.Reset();
					}
				}
			}
		}

		void Draw(bool bDrawCursor)
		{
			//Process drawing

			//Inform tools
			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				this->m_vTools[i].pGameTool->OnDraw();
			}

			//Inform scripted entities
			oScriptedEntMgr.Draw();
		}

		void DrawOnTop(bool bDrawCursor)
		{
			//Process on-top-drawing

			//Inform tools
			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				this->m_vTools[i].pGameTool->OnDrawOnTop();
			}

			//Inform scripted entities
			oScriptedEntMgr.DrawOnTop();

			//Draw cursor of current tool if requested
			if ((bDrawCursor) && (this->m_hSelectedTool != InvalidToolHandle)) {
				if (this->m_bDrawPointer) {
					pGfxReference->DrawSprite(this->m_hPointer, this->m_vMousePos[0], this->m_vMousePos[1], 0, 0.0f, 0.5f, 0.5f, false, 0, 0, 0, 0);
				} else if (this->m_vSelectedEntities.size() != 0) {
					pGfxReference->DrawSprite(this->m_hGotoCursor, this->m_vMousePos[0], this->m_vMousePos[1], 0, 0.0f, - 1, -1, 1.5f, 1.5f, false, 0, 0, 0, 0);
				} else {
					pGfxReference->DrawSprite(this->m_vTools[this->m_hSelectedTool].hCursor, this->m_vMousePos[0] + this->m_vTools[this->m_hSelectedTool].vCursorOffset[0], this->m_vMousePos[1] + this->m_vTools[this->m_hSelectedTool].vCursorOffset[1], 0, this->m_vTools[this->m_hSelectedTool].fCursorRotation, -1, -1, 0.0f, 0.0f, false, 0, 0, 0, 0);
				}
			}
		}

		void SetCursorOffset(const Vector& vOffset)
		{
			//Set cursor offset

			if (this->m_hSelectedTool != InvalidToolHandle) {
				this->m_vTools[this->m_hSelectedTool].vCursorOffset = vOffset;
			}
		}

		void SetCursorRotation(float fRotation)
		{
			//Set cursor rotation

			if (this->m_hSelectedTool != InvalidToolHandle) {
				this->m_vTools[this->m_hSelectedTool].fCursorRotation = fRotation;
			}
		}

		void OnKeyEvent(int iKey, bool bDown)
		{
			//Inform of key event

			if (this->m_hSelectedTool != InvalidToolHandle) {
				this->m_vTools[this->m_hSelectedTool].pGameTool->OnKeyEvent(iKey, bDown);
			}
		}

		void OnMouseEvent(int x, int y, int iKey, bool bDown, bool bCtrlHeld, const game_keys_s& gamekeys)
		{
			//Inform of mouse event

			//Store position
			if (iKey == 0) {
				this->m_vMousePos[0] = x;
				this->m_vMousePos[1] = y;

				this->m_bDrawPointer = false;

				if (GetAsyncKeyState(VK_CONTROL) & (1 << 15)) {
					for (size_t i = 0; i < oScriptedEntMgr.GetEntityCount(); i++) {
						CScriptedEntity* pEntity = oScriptedEntMgr.GetEntity(i);
						if ((pEntity != nullptr) && (pEntity->IsMovable()) && (oScriptedEntMgr.IsValidEntity(pEntity->Object()))) {
							Vector vCurPos = pEntity->GetPosition();
							Vector vSelSize = pEntity->GetSelectionSize();
							if ((this->m_vMousePos[0] > vCurPos[0] - vSelSize[0] / 2) && (this->m_vMousePos[0] < vCurPos[0] - vSelSize[0] / 2 + vSelSize[0]) && (this->m_vMousePos[1] > vCurPos[1] - vSelSize[1] / 2) && (this->m_vMousePos[1] < vCurPos[1] - vSelSize[1] / 2 + vSelSize[1])) {
								this->m_bDrawPointer = true;
								return;
							}
						}
					}
				}
			}

			//Clear if not valid anymore
			for (size_t i = 0; i < this->m_vSelectedEntities.size(); i++) {
				if ((this->m_vSelectedEntities[i] != nullptr) && (!oScriptedEntMgr.IsValidEntity(this->m_vSelectedEntities[i]->Object()))) {
					this->m_vSelectedEntities.erase(this->m_vSelectedEntities.begin() + i);
				}
			}

			//Process entity movement selection and action
			if ((iKey == gamekeys.vkTrigger) && (bDown)) {
				if (bCtrlHeld) {
					for (size_t i = 0; i < oScriptedEntMgr.GetEntityCount(); i++) {
						CScriptedEntity* pEntity = oScriptedEntMgr.GetEntity(i);
						if ((pEntity != nullptr) && (pEntity->IsMovable()) && (oScriptedEntMgr.IsValidEntity(pEntity->Object()))) {
							Vector vCurPos = pEntity->GetPosition();
							Vector vSelSize = pEntity->GetSelectionSize();
							if ((this->m_vMousePos[0] > vCurPos[0] - vSelSize[0] / 2) && (this->m_vMousePos[0] < vCurPos[0] - vSelSize[0] / 2 + vSelSize[0]) && (this->m_vMousePos[1] > vCurPos[1] - vSelSize[1] / 2) && (this->m_vMousePos[1] < vCurPos[1] - vSelSize[1] / 2 + vSelSize[1])) {
								this->m_vSelectedEntities.push_back(pEntity);
								this->m_bSurpressSelToolForwarding = true;
								return;
							}
						}
					}
				} else {
					for (size_t i = 0; i < this->m_vSelectedEntities.size(); i++) {
						if (this->m_vSelectedEntities[i] != nullptr) {
							this->m_vSelectedEntities[i]->MoveTo(this->m_vMousePos);
						}
					}

					if (this->m_vSelectedEntities.size() != 0) {
						this->m_bSurpressSelToolForwarding = true;
					}

					this->m_vSelectedEntities.clear();
				}
			}

			//Inform tools
			if (this->m_hSelectedTool != InvalidToolHandle) {
				this->m_vTools[this->m_hSelectedTool].pGameTool->OnMouseEvent(Vector(x, y), iKey, bDown);
			}
		}

		bool Select(HTOOL hTool)
		{
			//Select tool (by handle)

			if (hTool >= this->m_vTools.size())
				return false;

			//Inform previous selected tool 
			if (this->m_hSelectedTool != InvalidToolHandle) {
				this->m_vTools[this->m_hSelectedTool].pGameTool->OnSelect(false);
			}

			this->m_vTools[hTool].pGameTool->OnSelect(true);

			//Save current selection
			this->m_hSelectedTool = hTool;

			return true;
		}

		bool Select(const std::wstring& wszName)
		{
			//Select tool (by name)

			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				if (this->m_vTools[i].szToolScript == Utils::ConvertToAnsiString(wszName)) {
					return this->Select(i);
				}
			}

			return false;
		}

		void Unselect(void)
		{
			//Clear selection

			this->m_hSelectedTool = InvalidToolHandle;
		}

		bool Trigger(bool bStatus)
		{
			//Handle tool trigger

			if (this->m_hSelectedTool >= this->m_vTools.size())
				return false;

			this->m_vTools[this->m_hSelectedTool].oTriggerTimer.SetActive(bStatus);

			return true;
		}

		HTOOL Selection(void) { return this->m_hSelectedTool; }
		size_t Count(void) { return this->m_vTools.size(); }
		const gt_list_item_s& GetBaseInfo(const HTOOL hTool) { static gt_list_item_s sEmpty; if (hTool >= this->m_vTools.size()) return sEmpty; return this->m_vTools[hTool]; }
		const ToolInfo& GetInfo(const HTOOL hTool) { if (hTool >= this->m_vTools.size()) return ToolInfo(); return this->m_vTools[hTool].sInfo; }
		DxRenderer::HD3DSPRITE GetPreviewImage(const HTOOL hTool) { if (hTool >= this->m_vTools.size()) return GFX_INVALID_SPRITE_ID; return this->m_vTools[hTool].hPreviewImg; }
		Scripting::HSISCRIPT GetScriptHandleOfTool(HTOOL hTool) { if (hTool >= this->m_vTools.size()) return SI_INVALID_ID; return this->m_vTools[hTool].hScript; }
		Scripting::HSISCRIPT GetScriptHandleOfSelection(void) { if (this->m_hSelectedTool == InvalidToolHandle) return SI_INVALID_ID; return this->m_vTools[this->m_hSelectedTool].hScript; }
		HTOOL FindTool(const std::wstring& wszName)
		{
			//Find loaded tool by name

			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				if (this->m_vTools[i].szScript == Utils::ConvertToAnsiString(wszName))
					return i;
			}

			return SI_INVALID_ID;
		}
		std::wstring GetToolVersion(const std::wstring& wszName)
		{
			for (size_t i = 0; i < this->m_vTools.size(); i++) {
				if (this->m_vTools[i].szScript == Utils::ConvertToAnsiString(wszName))
					return Utils::ConvertToWideString(this->m_vTools[i].sInfo.szVersion);
			}
			return L"";
		}
		const std::vector<CScriptedEntity*>& GetSelectionEntities(void) const { return this->m_vSelectedEntities; }
		bool IsSelectionEntityValid(size_t index) { if ((index < this->m_vSelectedEntities.size()) && (this->m_vSelectedEntities[index] != nullptr)) { return oScriptedEntMgr.IsValidEntity(this->m_vSelectedEntities[index]->Object()); } else { return false;  } }
	};

	CToolMgr* Initialize(DxRenderer::CDxRenderer* pGfx, DxSound::CDxSound* pSnd, Scripting::CScriptInt* pScr, Console::CConsole* pCns);
	void DoUserCleaning(void);
	void SetBasePath(const std::wstring& path);
}