/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2019 by Daniel Brendel
	
	Tool: Fireworks (developed by Daniel Brendel)
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

string g_szToolPath = "";

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
class CSparkEntity : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	SpriteHandle m_hSpark;
	int m_iSpriteIndex;
	Timer m_tmrSpark;
	
	CSparkEntity()
    {
		this.m_iSpriteIndex = 0;
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_vecPos[1] += 220;
		
		this.m_hSpark = R_LoadSprite(g_szToolPath + "spark" + Util_Random(0, 4) + ".png", 8, 42, 336, 8, false);
		
		this.m_tmrSpark.SetDelay(100);
		this.m_tmrSpark.Reset();
		this.m_tmrSpark.SetActive(true);
		
		SoundHandle hExplosion = S_QuerySound(g_szToolPath + "explosion.wav");
		S_PlaySound(hExplosion, 10);
		
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Process sprite index change
		if (this.m_tmrSpark.IsActive()) {
			this.m_tmrSpark.Update();
			if (this.m_tmrSpark.IsElapsed()) {
				this.m_tmrSpark.Reset();
				
				this.m_iSpriteIndex++;
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hSpark, this.m_vecPos, this.m_iSpriteIndex, 0.0, Vector(-1, -1), 2.5, 2.5, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_iSpriteIndex >= 6;
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
		return "Spark";
	}
}
class CRocketEntity : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	SpriteHandle m_hRocket;
	SpriteHandle m_hTrail;
	int m_iTargetY;
	Timer m_tmrMove;
	Timer m_tmrTrail;
	int m_iTrailIndex;
	int m_iSpeed;
	bool m_bRemove;
	
	void SetTargetY(int y)
	{
		this.m_iTargetY = y;
	}
	
	CRocketEntity()
    {
		this.m_iTrailIndex = 0;
		this.m_iSpeed = 6;
		this.m_bRemove = false;
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hRocket = R_LoadSprite(g_szToolPath + "missile.png", 1, 32, 64, 1, false);
		this.m_hTrail = R_LoadSprite(g_szToolPath + "trail.png", 64, 128, 128, 8, false);
		
		this.m_tmrMove.SetDelay(10);
		this.m_tmrMove.Reset();
		this.m_tmrMove.SetActive(true);
		
		this.m_tmrTrail.SetDelay(100);
		this.m_tmrTrail.Reset();
		this.m_tmrTrail.SetActive(true);
		
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CSparkEntity @obj = CSparkEntity();
		Ent_SpawnEntity(@obj, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Process movement
		if (this.m_tmrMove.IsActive()) {
			this.m_tmrMove.Update();
			if (this.m_tmrMove.IsElapsed()) {
				this.m_tmrMove.Reset();
				this.m_vecPos[1] -= this.m_iSpeed;
				
				if (this.m_vecPos[1] <= this.m_iTargetY) {
					this.m_bRemove = true;
				}
			}
		}
		
		//Process trail
		if (this.m_tmrTrail.IsActive()) {
			this.m_tmrTrail.Update();
			if (this.m_tmrTrail.IsElapsed()) {
				this.m_tmrTrail.Reset();
				
				this.m_iTrailIndex++;
				if (this.m_iTrailIndex >= 64) {
					this.m_iTrailIndex = 0;
				}
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hRocket, this.m_vecPos, 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		R_DrawSprite(this.m_hTrail, Vector(this.m_vecPos[0] - 48, this.m_vecPos[1] + 25), this.m_iTrailIndex, 3.15, Vector(-1, -1), 0.5, 0.5, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_bRemove;
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
		return "Fireworks";
	}
}

/*
	This function shall be used for global initializations. Return true on success, otherwise false.
	This function gets called after CDG_API_QueryToolInfo().
*/
bool CDG_API_Initialize()
{
	return true;
}

/*
	Called for processing stuff
*/
void CDG_API_Process()
{
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
	CRocketEntity @obj = CRocketEntity();
	obj.SetTargetY(vAtPos[1]);
	Ent_SpawnEntity(@obj, Vector(vAtPos[0] + 10, Wnd_GetWindowCenterY() * 2 + 64));
	
	SoundHandle hSpawnSound = S_QuerySound(g_szToolPath + "spawn.wav");
	S_PlaySound(hSpawnSound, 10);
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
}

/*
	Called for tool selection status.
*/
void CDG_API_SelectionStatus(bool bSelectionStatus)
{
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
	info.szName = "Fireworks";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.1";
	info.szContact = "dbrendel1988<at>yahoo<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Demo";
	info.iCursorWidth = 64;
	info.iCursorHeight = 64;
	info.uiTriggerDelay = 125;
	
	g_szToolPath = szToolPath;

	return true;
}