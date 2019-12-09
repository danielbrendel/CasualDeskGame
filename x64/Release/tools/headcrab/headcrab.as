/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2019 by Daniel Brendel
	
	Tool: Headcrab (developed by Daniel Brendel)
	Version: 0.2
	Contact: Daniel Brendel<at>gmail<dot>com
	GitHub: https://github.com/dny-coder

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

string g_szToolPath;

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
class CBloodSplash : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oLifeTime;
	SpriteHandle m_hSprite;
	
	CBloodSplash()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "blood.png", 1, 64, 64, 1, false);
		this.m_oLifeTime.SetDelay(120000);
		this.m_oLifeTime.Reset();
		this.m_oLifeTime.SetActive(true);
		this.m_oModel.Alloc();
		SoundHandle hSound = S_QuerySound(g_szToolPath + "splash.wav");
		S_PlaySound(hSound, 10);
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
		R_DrawSprite(this.m_hSprite, this.m_vecPos, 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
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
		return 1;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "";
	}
}

const float C_HEADCRAB_DEFAULT_SPEED = 2;
const int C_HEADCRAB_REACT_RANGE = 250;
class CHeadcrab : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	SpriteHandle m_hSprite;
	Timer m_oShaking;
	Timer m_oDirChange;
	Timer m_oLifeTime;
	Timer m_oWalkSound;
	Timer m_oEnemyCheck;
	float m_fShakeRot;
	float m_fWalkRot;
	float m_fSpeed;
	uint8 m_hvHealth;
	bool m_bGotEnemy;
	SoundHandle m_hWalkSound;
	SoundHandle m_hPainSound;
	
	CHeadcrab()
    {
		this.m_hvHealth = 25;
		this.m_fSpeed = C_HEADCRAB_DEFAULT_SPEED;
		this.m_fShakeRot = 0.0;
		this.m_bGotEnemy = false;
    }
	
	void Move(void)
	{
		//Update position according to speed
		this.m_vecPos[0] += int(sin(this.m_fWalkRot) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fWalkRot) * this.m_fSpeed);
		
		//Fix positions
		
		if (this.m_vecPos[0] <= -32) {
			this.m_vecPos[0] = Wnd_GetWindowCenterX() * 2 + 32;
		} else if (this.m_vecPos[0] >= Wnd_GetWindowCenterX() * 2 + 32) {
			this.m_vecPos[0] = -32;
		}
		
		if (this.m_vecPos[1] <= -32) {
			this.m_vecPos[1] = Wnd_GetWindowCenterY() * 2 + 32;
		} else if (this.m_vecPos[1] >= Wnd_GetWindowCenterY() * 2 + 32) {
			this.m_vecPos[1] = -32;
		}
	}
	
	bool ShallRemove(void)
	{
		//Indicate removal
		return ((this.m_oLifeTime.IsElapsed()) || (this.m_hvHealth == 0));
	}
	
	void LookAt(const Vector &in vPos)
	{
		//Look at position
		const float PI = 3.141592f;
		this.m_fWalkRot = atan2(float(vPos[1] - this.m_vecPos[1]), float(vPos[0] - this.m_vecPos[0]));
	}
	
	void CheckForEnemiesInRange()
	{
		//Check for enemies in close range and act accordingly
		
		this.m_bGotEnemy = false;
		IScriptedEntity@ pEntity = null;
		
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) {
			@pEntity = @Ent_GetEntityHandle(i);
			if ((@pEntity != null) && (pEntity.GetName() != "Headcrab") && (pEntity.IsDamageable() != DAMAGEABLE_NO)) {
				if (this.m_vecPos.Distance(pEntity.GetPosition()) <= C_HEADCRAB_REACT_RANGE) {
					this.m_bGotEnemy = true;
					break;
				}
			}
		}
		
		if (this.m_bGotEnemy) {
			if (this.m_fSpeed == C_HEADCRAB_DEFAULT_SPEED)
				this.m_fSpeed *= 2;
				
			//this.LookAt(pEntity.GetPosition());
		} else {
			if (this.m_fSpeed != C_HEADCRAB_DEFAULT_SPEED)
				this.m_fSpeed = C_HEADCRAB_DEFAULT_SPEED;
		}
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_fWalkRot = float(Util_Random(1, 360));
		this.m_vecPos = vec;
		this.m_vecPos[0] += 32;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "headcrab.png", 1, 32, 32, 1, false);
		this.m_oLifeTime.SetDelay(240000);
		this.m_oLifeTime.Reset();
		this.m_oLifeTime.SetActive(true);
		this.m_oDirChange.SetDelay(10000);
		this.m_oDirChange.Reset();
		this.m_oDirChange.SetActive(true);
		this.m_oEnemyCheck.SetDelay(1);
		this.m_oEnemyCheck.Reset();
		this.m_oEnemyCheck.SetActive(true);
		this.m_oShaking.SetDelay(1000);
		this.m_oShaking.Reset();
		this.m_oShaking.SetActive(true);
		this.m_oWalkSound.SetDelay(1000 + Util_Random(1, 2000));
		this.m_oWalkSound.Reset();
		this.m_oWalkSound.SetActive(true);
		this.m_hWalkSound = S_QuerySound(g_szToolPath + "walk.wav");
		this.m_hPainSound = S_QuerySound(g_szToolPath + "hurt.wav");
		S_PlaySound(this.m_hWalkSound, 8);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(32, 32));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(16, 16));
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CBloodSplash @obj = CBloodSplash();
		Ent_SpawnEntity(@obj, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oLifeTime.Update();
	
		this.m_oShaking.Update();
		if (this.m_oShaking.IsElapsed()) {
			if (!this.m_bGotEnemy)
				this.m_fShakeRot = -0.25 + float(Util_Random(1, 5)) / 10.0;
		}
		
		this.m_oDirChange.Update();
		if (this.m_oDirChange.IsElapsed()) {
			this.m_oDirChange.Reset();
			if (!this.m_bGotEnemy)
				this.m_fWalkRot = float(Util_Random(1, 360));
		}
		
		this.m_oWalkSound.Update();
		if (this.m_oWalkSound.IsElapsed()) {
			this.m_oWalkSound.Reset();
			S_PlaySound(this.m_hWalkSound, 8);
		}
		
		/*this.m_oEnemyCheck.Update();
		if (this.m_oEnemyCheck.IsElapsed()) {
			this.m_oEnemyCheck.Reset();
			this.CheckForEnemiesInRange();
		}*/
		this.CheckForEnemiesInRange();
		
		this.Move();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hSprite, this.m_vecPos, 0, this.m_fWalkRot + this.m_fShakeRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.ShallRemove();
	}
	
	//Indicate whether this entity is damageable. Damageable entities can collide with other
	//entities (even with entities from other tools) and recieve and strike damage. 
	//0 = not damageable, 1 = damage all, 2 = not damaging entities with same name
	DamageType IsDamageable()
	{
		return DAMAGEABLE_NOTSQUAD;
	}
	
	//Called when the entity recieves damage
	void OnDamage(DamageValue dv)
	{
		if (dv == 0) return;
		
		if (int(this.m_hvHealth) - int(dv) >= 0) { this.m_hvHealth -= dv; S_PlaySound(this.m_hPainSound, 10); }
		else { this.m_hvHealth = 0; }
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
		return 1;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "Headcrab";
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
	CHeadcrab @obj = CHeadcrab();
	Ent_SpawnEntity(@obj, Vector(vAtPos[0] - 32, vAtPos[1] - 10));
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
	The tool path can be used to load objects from. Return true on success, otherwise false.
*/
bool CDG_API_QueryToolInfo(HostVersion hvVersion, ToolInfo &out info, const GameKeys& in gamekeys, const string &in szToolPath)
{
	info.szName = "Headcrab";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.2";
	info.szContact = "Daniel Brendel<at>gmail<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Monsters";
	info.iCursorWidth = 32;
	info.iCursorHeight = 32;
	info.uiTriggerDelay = 125;
	
	g_szToolPath = szToolPath;

	return true;
}