/*
	Casual Desktop Game (dnycasualDeskGame) v1.0 developed by Daniel Brendel
	
	(C) 2018 - 2020 by Daniel Brendel
	
	Tool: Tesla Tower (developed by Daniel Brendel)
	Version: 0.3
	Contact: dbrendel1988<at>yahoo<dot>com
	GitHub: https://github.com/danielbrendel

	License: see LICENSE.txt
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
SoundHandle g_hSpawnSound;
bool g_bTeamToggle = true;
Vector g_vecMousePos;
bool g_bSelectionStatus = false;
GameKeys g_GameKeys;

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
	
	CDecalSprite()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "decal.png", 1, 64, 64, 1, false);
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
		return 0;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "";
	}
}
class CDetonation : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oExplosion;
	int m_iFrameCount;
	SpriteHandle m_hSprite;
	SoundHandle m_hSound;
	
	CDetonation()
    {
		this.m_iFrameCount = 0;
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = Vector(vec[0] + 50, vec[1] + 50);
		this.m_hSprite = R_LoadSprite(g_szToolPath + "explosion.png", 6, 32, 32, 6, false);
		this.m_oExplosion.SetDelay(100);
		this.m_oExplosion.Reset();
		this.m_oExplosion.SetActive(true);
		this.m_hSound = S_QuerySound(g_szToolPath + "explosion.wav");
		S_PlaySound(this.m_hSound, 10);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(14, 14), Vector(100, 100));
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oExplosion.Update();
		if (this.m_oExplosion.IsElapsed()) {
			this.m_oExplosion.Reset();
			this.m_iFrameCount++;
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw everything on top here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hSprite, this.m_vecPos, this.m_iFrameCount, 0.0, Vector(-1, -1), 2.0, 2.0, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_iFrameCount >= 6;
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
		return 1;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "";
	}
}
class CTeslaLightning : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecTarget;
	Model m_oModel;
	Timer m_oFrameTime;
	SpriteHandle m_hSprite;
	int m_iFrameCounter;
	float m_fRotation;
	float m_fRange;
	
	CTeslaLightning()
    {
		this.m_iFrameCounter = 0;
		this.m_fRotation = 0.0;
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRange = float(Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 5).Distance(this.m_vecTarget)) / 300;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "lightning.png", 8, 32, 256, 8, false);
		this.m_oFrameTime.SetDelay(10);
		this.m_oFrameTime.Reset();
		this.m_oFrameTime.SetActive(true);
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oFrameTime.Update();
		if (this.m_oFrameTime.IsElapsed()) {
			this.m_oFrameTime.Reset();
			this.m_iFrameCounter++;
			if (this.m_iFrameCounter >= 8)
				this.m_oFrameTime.SetActive(false);
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw everything on top here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hSprite, this.m_vecPos, this.m_iFrameCounter, this.m_fRotation, Vector(16, 0), 0.75, this.m_fRange, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_iFrameCounter >= 8;
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
		return this.m_fRotation;
	}
	
	//Called for querying the damage value for this entity
	DamageValue GetDamageValue()
	{
		return 0;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "TeslaLightning";
	}
	
	//Custom methods
	
	void SetRotation(float fRotation)
	{
		this.m_fRotation = fRotation;
	}
	
	void SetTarget(const Vector &in vTarget)
	{
		this.m_vecTarget = vTarget;
	}
}
const int TESLA_TEAM_1 = 1;
const int TESLA_TEAM_2 = 2;
class color_s
{
	uint8 r, g, b, a;
}
class CTeslaTower : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oAttacking;
	Timer m_oAlpha;
	SpriteHandle m_hSprite;
	SoundHandle m_hAttack;
	SoundHandle m_hCharge;
	IScriptedEntity@ m_pTarget;
	uint16 m_hvHealth;
	uint8 m_ucAlpha;
	uint8 m_ucTeam;
	
	CTeslaTower()
    {
		this.m_hvHealth = 400;
		@this.m_pTarget = null;
		this.m_ucAlpha = 255;
    }
	
	void SetTeam(uint8 ucTeam)
	{
		this.m_ucTeam = ucTeam;
	}
	
	uint8 GetTeam()
	{
		return this.m_ucTeam;
	}
	
	float GetAimRotation(const Vector &in vPos)
	{
		//Get aim rotation
		const float PI = 3.141592f;
		float fAngle = atan2(float(vPos[1] - this.m_vecPos[1]), float(vPos[0] - this.m_vecPos[0]));
		return fAngle - 6.30 / 4;
	}
	
	void CheckForEnemiesInRange()
	{
		//Check for enemies in close range and act accordingly
		
		const int TESLATOWER_ATTACK_RANGE = 320;
		
		@this.m_pTarget = null;
		
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) {
			IScriptedEntity@ pEntity = @Ent_GetEntityHandle(i);
			if ((@pEntity != null) && (@pEntity != @this) && (pEntity.GetName() != "") && (pEntity.GetName() != this.GetName()) && (pEntity.IsDamageable() != DAMAGEABLE_NO)) {
				if (this.m_vecPos.Distance(pEntity.GetPosition()) <= TESLATOWER_ATTACK_RANGE) {
					@this.m_pTarget = @pEntity;
					break;
				}
			}
		}
		
		if (@this.m_pTarget != null) {
			if (!this.m_oAttacking.IsActive()) {
				this.m_oAttacking.SetDelay(2500);
				this.m_oAttacking.Reset();
				this.m_oAttacking.SetActive(true);
				this.m_oAlpha.SetDelay(10);
				this.m_oAlpha.Reset();
				this.m_oAlpha.SetActive(true);
				this.m_ucAlpha = 255;
				S_PlaySound(this.m_hCharge, 9);
			}
		} else {
			if (this.m_oAttacking.IsActive())
				this.m_oAttacking.SetActive(false);
			
			if (this.m_oAlpha.IsActive())
				this.m_oAlpha.SetActive(false);
				
			this.m_ucAlpha = 255;
		}
	}
	
	void Fire()
	{
		//Fire tesla lightning
		
		CTeslaLightning@ obj = CTeslaLightning();
		Vector vTargetPos = this.m_pTarget.GetPosition();
		Vector vTargetCenter = this.m_pTarget.GetModel().GetCenter();
		Vector vAbsTargetPos = Vector(vTargetPos[0] + vTargetCenter[0], vTargetPos[1] + vTargetCenter[1]);
		obj.SetRotation(this.GetAimRotation(vAbsTargetPos));
		obj.SetTarget(vAbsTargetPos);
		Ent_SpawnEntity(@obj, this.m_vecPos);
		this.m_pTarget.OnDamage(this.GetDamageValue());
		S_PlaySound(this.m_hAttack, 10);
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "teslatower.png", 1, 32, 55, 1, false);
		this.m_hAttack = S_QuerySound(g_szToolPath + "attack.wav");
		this.m_hCharge = S_QuerySound(g_szToolPath + "charge.wav");
		this.m_oAttacking.SetActive(false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(32, 55));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(16, 38));
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CDetonation @expl = CDetonation();
		Ent_SpawnEntity(@expl, Vector(this.m_vecPos[0] - 32, this.m_vecPos[1] - 30));
		
		CDecalSprite @dcl = CDecalSprite();
		Ent_SpawnEntity(@dcl, Vector(this.m_vecPos[0] - 10, this.m_vecPos[1]));
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.CheckForEnemiesInRange();
		
		if (this.m_oAttacking.IsActive()) {
			this.m_oAttacking.Update();
			if (this.m_oAttacking.IsElapsed()) {
				this.m_oAttacking.Reset();
				this.Fire();
				this.m_ucAlpha = 255;
			}
		}
		
		if (this.m_oAlpha.IsActive()) {
			this.m_oAlpha.Update();
			if (this.m_oAlpha.IsElapsed()) {
				this.m_oAlpha.Reset();
				
				this.m_ucAlpha -= 2;
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw everything on top here
	void OnDrawOnTop()
	{
		color_s sDrawingColor;
		if (this.GetTeam() == TESLA_TEAM_1) {
			sDrawingColor.r = 105;
			sDrawingColor.g = 201;
			sDrawingColor.b = 224;
		} else {
			sDrawingColor.r = 255;
			sDrawingColor.g = 0;
			sDrawingColor.b = 0;
		}
		
		R_DrawSprite(this.m_hSprite, this.m_vecPos, 0, 0.0, Vector(-1, -1), 0.0, 0.0, true, Color(sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, this.m_ucAlpha));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return (this.m_hvHealth == 0);
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
		
		if (int(this.m_hvHealth) - int(dv) >= 0) { this.m_hvHealth -= dv; }
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
		return 45;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return (this.GetTeam() == TESLA_TEAM_1) ? "Combat.Team1" : "Combat.Team2";
	}
}

/*
	This function shall be used for global initializations. Return true on success, otherwise false.
	This function gets called after CDG_API_QueryToolInfo().
*/
bool CDG_API_Initialize()
{
	g_hSpawnSound = S_QuerySound(g_szToolPath + "spawn.wav");

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
	if (g_bSelectionStatus) {
		string szInfoStr = (g_bTeamToggle) ? "[Tesla] Team 1" : "[Tesla] Team 2";
		R_DrawString(R_GetDefaultFont(), szInfoStr, Vector(g_vecMousePos[0], g_vecMousePos[1] - 20), Color(128, 0, 64, 150));
	}
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	CTeslaTower @obj = CTeslaTower();
	obj.SetTeam((g_bTeamToggle) ? TESLA_TEAM_1 : TESLA_TEAM_2);
	Ent_SpawnEntity(@obj, vAtPos);
	
	S_PlaySound(g_hSpawnSound, 10);
}

/*
	This function is called for any keyboard key event. This is even the case if 
	this tool is not currently selected.
*/
void CDG_API_KeyEvent(int iKey, bool bDown)
{
	if ((iKey == g_GameKeys.vkTeamSelect) && (!bDown))
		g_bTeamToggle = !g_bTeamToggle;
}

/*
	This function is called for any mouse event. This is even the case if 
	this tool is not currently selected.
*/
void CDG_API_MouseEvent(const Vector &in coords, int iKey, bool bDown)
{
	g_vecMousePos = coords;
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
	The tool path can be used to load objects from. Return true on success, otherwise false.
*/
bool CDG_API_QueryToolInfo(HostVersion hvVersion, ToolInfo &out info, const GameKeys& in gamekeys, const string &in szToolPath)
{
	info.szName = "Tesla Tower";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.3";
	info.szContact = "dbrendel1988<at>yahoo<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Military";
	info.iCursorWidth = 32;
	info.iCursorHeight = 55;
	info.uiTriggerDelay = 500;
	
	g_szToolPath = szToolPath;
	g_GameKeys = gamekeys;
	
	return true;
}