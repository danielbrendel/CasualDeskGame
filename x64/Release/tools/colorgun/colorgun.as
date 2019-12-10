/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2019 by Daniel Brendel
	
	Tool: Color Gun (developed by Daniel Brendel)
	Version: 0.2
	Contact: dbrendel1988<at>yahoo<dot>com
	GitHub: https://github.com/danielbrendel

	Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

/*
	Available API functions:
	
	void Print(const string& in)
	void PrintClr(const string& in, const ConColor &in)
	void SetCursorOffset(const Vector& in)
	void SetCursorRotation(float fRotation)
	FontHandle R_LoadFont(const string& in, uint8 ucFontSizeW, uint8 ucFontSizeH)
	bool R_GetSpriteInfo(const string &in, SpriteInfo &out)
	SpriteHandle R_LoadSprite(const string& in szFile, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)
	bool R_FreeSprite(SpriteHandle hSprite)
	bool R_DrawBox(const Vector& in pos, const Vector&in size, int iThickness, const Color&in color)
	bool R_DrawFilledBox(const Vector&in pos, const Vector&in size, const Color&in color)
	bool R_DrawLine(const Vector&in start, const Vector&in end, const Color&in color)
	bool R_DrawSprite(const SpriteHandle hSprite, const Vector&in pos, int iFrame, float fRotation, const Vector &in vRotPos, float fScale1, float fScale2, bool bUseCustomColorMask, const Color&in color)
	bool R_DrawString(const FontHandle font, const string&in szText, const Vector&in pos, const Color&in color)
	FontHandle R_GetDefaultFont()
	SoundHandle S_QuerySound(const string&in szSoundFile)
	bool S_PlaySound(SoundHandle hSound, int32 lVolume)
	int Wnd_GetWindowCenterX()
	int Wnd_GetWindowCenterY()
	bool Ent_SpawnEntity(IScriptedEntity @obj, const Vector& in)
	size_t Ent_GetEntityCount()
	IScriptedEntity@+ Ent_GetEntityHandle(size_t uiEntityId)
	IScriptedEntity@+ Ent_TraceLine(const Vector&in vStart, const Vector&in vEnd, IScriptedEntity@+ pIgnoredEnt)
	bool Ent_IsValid(IScriptedEntity@ pEntity)
	size_t Ent_GetId(IScriptedEntity@ pEntity)
	bool Util_ListSprites(const string& in, FuncFileListing @cb)
	bool Util_ListSounds(const string& in, FuncFileListing @cb)
	int Util_Random(int start, int end)
*/

bool g_bSelectionStatus = false;
Vector g_vCursorPos;
string g_szToolPath;
SpriteHandle g_hMuzzleFlash;
bool g_bDrawMuzzleFlashOnce = false;
array<string> g_arDecalList;
SpriteHandle g_hXHair;

/* 
	Scripted entity 
	
	A scripted entity is a dynamic interface class for implementing
	custom entities such as explosions, decals, etc
	
	It is derived from the IScriptedEntity interface and must define
	all the following interface functions. Also it must have at least
	one Vector and Model which are returned in certain methods. Tho
	you don't need to use them. A scripted entity is passed to the 
	game engine via the Ent_SpawnEntity(<object handle>, <spawn position>) function. 
*/
class CDecalSprite : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oLifeTime;
	SpriteHandle m_hSprite;
	float m_fSpriteRot;
	
	CDecalSprite()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "decals\\" + g_arDecalList[Util_Random(1, g_arDecalList.length()) - 1], 1, 128, 128, 1, false);
		this.m_fSpriteRot = float(Util_Random(1, 360));
		this.m_oLifeTime.SetDelay(120000);
		this.m_oLifeTime.Reset();
		this.m_oLifeTime.SetActive(true);
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oLifeTime.Update();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
		R_DrawSprite(this.m_hSprite, this.m_vecPos, 0, this.m_fSpriteRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return true;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_oLifeTime.IsElapsed();
	}
	
	//Indicate whether this entity is damageable. Damageable entities can collide with other
	//entities (even with entities from other tools) and recieve and strike damage. 
	//0 = not damageable, 1 = damage all, 2 = not damaging entities with same name
	DamageType IsDamageable()
	{
		return DAMAGEABLE_NO;
	}
	
	//Called when the entity recieves damage
	void OnDamage(DamageValue dv)
	{
	}
	
	//Called for recieving the model data for this entity. This is only used for
	//damageable entities. 
	Model& GetModel()
	{
		return this.m_oModel;
	}
	
	//Called for recieving the current position. This is useful if the entity shall move.
	Vector& GetPosition()
	{
		return this.m_vecPos;
	}
	
	//Return the rotation. This is actually not used by the host application, but might be useful to other entities
	float GetRotation()
	{
		return 0.0;
	}
	
	//Called for querying the damage value for this entity
	DamageValue GetDamageValue()
	{
		return 0;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "";
	}
}
class CDecalEntity : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oLifeTime;
	SpriteHandle m_hSprite;
	
	CDecalEntity()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_vecPos[0] -= 400;
		this.m_vecPos[1] -= 170;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "decals\\" + g_arDecalList[0], 1, 128, 128, 1, false);
		this.m_oLifeTime.SetDelay(10);
		this.m_oLifeTime.Reset();
		this.m_oLifeTime.SetActive(true);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(128, 128));
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CDecalSprite @obj = CDecalSprite();
		Ent_SpawnEntity(@obj, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oLifeTime.Update();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_oLifeTime.IsElapsed();
	}
	
	//Indicate whether this entity is damageable. Damageable entities can collide with other
	//entities (even with entities from other tools) and recieve and strike damage. 
	//0 = not damageable, 1 = damage all, 2 = not damaging entities with same name
	DamageType IsDamageable()
	{
		return DAMAGEABLE_ALL;
	}
	
	//Called when the entity recieves damage
	void OnDamage(DamageValue dv)
	{
	}
	
	//Called for recieving the model data for this entity. This is only used for
	//damageable entities. 
	Model& GetModel()
	{
		return this.m_oModel;
	}
	
	//Called for recieving the current position. This is useful if the entity shall move.
	Vector& GetPosition()
	{
		return this.m_vecPos;
	}
	
	//Return the rotation. This is actually not used by the host application, but might be useful to other entities
	float GetRotation()
	{
		return 0.0;
	}
	
	//Called for querying the damage value for this entity
	DamageValue GetDamageValue()
	{
		return 25;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "";
	}
}

bool ListSpritesCallback(const string &in name)
{
	g_arDecalList.insertLast(name);
	
	return true;
}

/*
	This function shall be used for global initializations. Return true on success, otherwise false.
	This function gets called after CDG_API_QueryToolInfo().
*/
bool CDG_API_Initialize()
{
	g_hMuzzleFlash = R_LoadSprite(g_szToolPath + "muzzle.png", 1, 256, 256, 1, false);
	g_hXHair = R_LoadSprite(g_szToolPath + "xhair.png", 1, 32, 32, 1, false);
	
	Util_ListSprites(g_szToolPath + "decals", @ListSpritesCallback);

	return true;
}

/*
	Called for processing stuff
*/
void CDG_API_Process()
{
	if (g_bSelectionStatus)
		SetCursorRotation(179.5);
}

/*
	Default drawing can be done here
*/
void CDG_API_Draw()
{
}

/*
	On-Top drawing can be done here
*/
void CDG_API_DrawOnTop()
{
	if (g_bDrawMuzzleFlashOnce) {
		g_bDrawMuzzleFlashOnce = false;
		
		Vector vDrawingPos = g_vCursorPos;
		vDrawingPos[0] -= 160;
		vDrawingPos[1] -= 80;
		
		R_DrawSprite(g_hMuzzleFlash, vDrawingPos, 0, 179.5, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	R_DrawSprite(g_hXHair, Vector(g_vCursorPos[0] - 400 + 57, g_vCursorPos[1] - 170 + 54), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	g_bDrawMuzzleFlashOnce = true;
	
	CDecalEntity@ obj = CDecalEntity();
	Ent_SpawnEntity(@obj, vAtPos);
	
	SoundHandle hSound = S_QuerySound(g_szToolPath + "gunshot.wav");
	S_PlaySound(hSound, 10);
}

/*
	This function is called for any keyboard key event. This is even the case if 
	this tool is not currently selected.
*/
void CDG_API_KeyEvent(int iKey, bool bDown)
{
}

/*
	This function is called for any mouse event. This is even the case if 
	this tool is not currently selected.
*/
void CDG_API_MouseEvent(const Vector &in coords, int iKey, bool bDown)
{
	if (iKey == 0) g_vCursorPos = coords;
}

/*
	Called for tool selection status.
*/
void CDG_API_SelectionStatus(bool bSelectionStatus)
{
	g_bSelectionStatus = bSelectionStatus;
	if (g_bSelectionStatus) {
		SetCursorRotation(179.5);
	}
}

/*
	This function shall be used for any global cleanup
*/
void CDG_API_Release()
{
}

/*
	This function is called for recieving the tool information.  The host version is passed which can be used
	to determine if the tool works for this game version. Tool information must be stored into the info struct.
	The tool path can be used to load objects from. Return true on success, otherwise false.
*/
bool CDG_API_QueryToolInfo(HostVersion hvVersion, ToolInfo &out info, const GameKeys& in gamekeys, const string &in szToolPath)
{
	info.szName = "Color Gun";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.2";
	info.szContact = "dbrendel1988<at>yahoo<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "gun.png";
	info.szCategory = "Weapons";
	info.iCursorWidth = 256;
	info.iCursorHeight = 256;
	info.uiTriggerDelay = 500;
	
	g_szToolPath = szToolPath;

	return true;
}