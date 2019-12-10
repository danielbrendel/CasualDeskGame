/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2019 by Daniel Brendel
	
	Tool: Chainsaw (developed by Daniel Brendel)
	Version: 0.1
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
string g_szToolPath = "";
SoundHandle g_hSound;
Timer g_tmrSound;

/* 
	Scripted entity 
	
	A scripted entity is a dynamic interface class for implementing
	custom entities such as explosions, decals, etc
	
	It is derived from the IScriptedEntity interface and must define
	all the following interface functions. Also it must have at least
	one Vector and Model which are returned in certain methods. Tho
	you don't need to use them. A scripted entity is passed to the 
	game engine via the SpawnEntity(<object handle>, <spawn position>) function. 
*/
class CChainsawDecal : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	SpriteHandle m_hDecal;
	Timer m_tmrLifespan;
	
	CChainsawDecal()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hDecal = R_LoadSprite(g_szToolPath + "decal.png", 1, 10, 10, 1, false);
		this.m_tmrLifespan.SetDelay(120000);
		this.m_tmrLifespan.Reset();
		this.m_tmrLifespan.SetActive(true);
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_tmrLifespan.Update();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
		R_DrawSprite(this.m_hDecal, this.m_vecPos, 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
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
		return this.m_tmrLifespan.IsElapsed();
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
class CDamageDecal : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	SpriteHandle m_hDecal;
	Timer m_tmrLifespan;
	
	CDamageDecal()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hDecal = R_LoadSprite(g_szToolPath + "decal.png", 1, 10, 10, 1, false);
		this.m_tmrLifespan.SetDelay(10);
		this.m_tmrLifespan.Reset();
		this.m_tmrLifespan.SetActive(true);
		this.m_oModel.Alloc();
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(10, 10));
		this.m_oModel.Initialize2(bbox, this.m_hDecal);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_tmrLifespan.Update();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
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
		return this.m_tmrLifespan.IsElapsed();
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
		return 10;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "";
	}
}

/*
	This function shall be used for global initializations. Return true on success, otherwise false.
	This function gets called after CDG_API_QueryToolInfo().
*/
bool CDG_API_Initialize()
{
	g_hSound = S_QuerySound(g_szToolPath + "chainsaw.wav");

	g_tmrSound.SetDelay(900);
	g_tmrSound.Reset();
	g_tmrSound.SetActive(false);

	return true;
}

/*
	Called for processing stuff
*/
void CDG_API_Process()
{
	if (g_tmrSound.IsActive()) {
		g_tmrSound.Update();
		if (g_tmrSound.IsElapsed()) {
			g_tmrSound.Reset();
			S_PlaySound(g_hSound, 8);
		}
	}
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
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	CDamageDecal @dmg = CDamageDecal();
	Ent_SpawnEntity(@dmg, Vector(vAtPos[0] + 80, vAtPos[1] + 180));

	CChainsawDecal @obj = CChainsawDecal();
	Ent_SpawnEntity(@obj, Vector(vAtPos[0] + 80, vAtPos[1] + 180));
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
	if ((iKey == 1) && (g_bSelectionStatus)) {
		if (bDown) {
			SetCursorRotation(-1.0);

			if (!g_tmrSound.IsActive()) {
				g_tmrSound.Reset();
				g_tmrSound.SetActive(true);

				S_PlaySound(g_hSound, 8);
			}
		} else {
			SetCursorRotation(0.0);
			g_tmrSound.SetActive(false);
		}
	}
}

/*
	Called for tool selection status.
*/
void CDG_API_SelectionStatus(bool bSelectionStatus)
{
	g_bSelectionStatus = bSelectionStatus;
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
	The gamekeys holds virtual key codes of bound actions. The tool path can be used to load objects from. Return 
	true on success, otherwise false.
*/
bool CDG_API_QueryToolInfo(HostVersion hvVersion, ToolInfo &out info, const GameKeys& in gamekeys, const string &in szToolPath)
{
	info.szName = "Chainsaw";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.1";
	info.szContact = "dbrendel1988<at>yahoo<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "chainsaw.png";
	info.szCategory = "Weapons";
	info.iCursorWidth = 256;
	info.iCursorHeight = 256;
	info.uiTriggerDelay = 1;

	g_szToolPath = szToolPath;

	return true;
}