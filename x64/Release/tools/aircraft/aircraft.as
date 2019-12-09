/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2019 by Daniel Brendel
	
	Tool: Aircraft (developed by Daniel Brendel)
	Version: 0.1
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

string g_szToolPath = "";
bool g_bTeamSelection = true;
bool g_bSelectionStatus = false;
Vector g_vecMousePos;
GameKeys g_GameKeys;

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
class CExplosion : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oExplosion;
	int m_iFrameCount;
	SpriteHandle m_hSprite;
	SoundHandle m_hSound;
	
	CExplosion()
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
class CMissile : IScriptedEntity
{
	Vector m_vecPos;
	float m_fRotation;
	Model m_oModel;
	SpriteHandle m_hMissile;
	SpriteHandle m_hTrail;
	int m_iTrailIndex;
	IScriptedEntity@ m_pTarget;
	Timer m_tmrForward;
	float m_fTargetRot;
	float m_fRotStep;
	bool m_bHasCollided;
	DamageValue m_dvHealth;
	float m_fSpeed;
	Timer m_tmrLifeTime;
	SoundHandle m_hExplode;
	
	CMissile()
    {
		@this.m_pTarget = null;
		this.m_bHasCollided = false;
		this.m_dvHealth = 1;
		this.m_fSpeed = 5.5;
		this.m_tmrLifeTime.SetDelay(10000);
		this.m_tmrLifeTime.Reset();
		this.m_tmrLifeTime.SetActive(true);
		this.m_iTrailIndex = 0;
    }
	
	void SetTarget(IScriptedEntity@ pEntity)
	{
		@m_pTarget = @pEntity;
	}
	
	void SetRotation(float fRot)
	{
		this.m_fRotation = fRot;
	}
	
	float GetAimRot()
	{
		//Get aim rotation
		Vector vecTargetPos = this.m_pTarget.GetPosition();
		Vector vecTargetCenter = this.m_pTarget.GetModel().GetCenter();
		float fAngle = atan2(float(vecTargetPos[1] + vecTargetCenter[1] - this.m_vecPos[1]), float(vecTargetPos[0] + vecTargetCenter[0] - this.m_vecPos[0]));
		return fAngle + 6.30 / 4;
	}
	
	void Move(void)
	{
		//Set next position according to view
		this.m_vecPos[0] += int(sin(this.m_fRotation + 0.014) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fRotation + 0.014) * this.m_fSpeed);
		
		//Correct position according to screen pos
		if (this.m_vecPos[0] - 81 < 0)
			this.m_vecPos[0] = Wnd_GetWindowCenterX() * 2;
		else if (this.m_vecPos[0] > Wnd_GetWindowCenterX() * 2)
			this.m_vecPos[0] = 0;
		if (this.m_vecPos[1] < 0)
			this.m_vecPos[1] = Wnd_GetWindowCenterY() * 2;
		else if (this.m_vecPos[1] > Wnd_GetWindowCenterY() * 2)
			this.m_vecPos[1] = 0;
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hMissile = R_LoadSprite(g_szToolPath + "missile.png", 1, 80, 60, 1, false);
		this.m_hTrail = R_LoadSprite(g_szToolPath + "trail.png", 64, 128, 128, 8, false);
		this.m_hExplode = S_QuerySound(g_szToolPath + "hit.wav");
		this.m_tmrForward.SetDelay(2500);
		this.m_tmrForward.Reset();
		this.m_tmrForward.SetActive(true);
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosion @expl = CExplosion();
		Ent_SpawnEntity(@expl, Vector(this.m_vecPos[0] - 20, this.m_vecPos[1] - 20));
		
		S_PlaySound(this.m_hExplode, 8);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Move missile
		this.Move();
		
		//Process lifetime timer
		this.m_tmrLifeTime.Update();
		
		//Validate target entity handle
		if (!Ent_IsValid(@this.m_pTarget)) {
			@this.m_pTarget = null;
			this.m_bHasCollided = true;
			return;
		}
		
		//Process forward timer
		if (this.m_tmrForward.IsActive()) { //Fly forward
			this.m_tmrForward.Update();
			if (this.m_tmrForward.IsElapsed()) {
				this.m_tmrForward.SetActive(false);
				this.m_fRotStep = Util_Random(0, 2) == 0 ? -0.01 : 0.01;
			}
		} else { //Head to target
			this.m_fRotation = this.GetAimRot();
			this.m_fSpeed = 10.0;
			if (this.m_pTarget.GetModel().GetBBox().IsInside(this.m_pTarget.GetPosition(), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 10))) {
				this.m_bHasCollided = true;
				this.m_pTarget.OnDamage(this.GetDamageValue());
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
		Vector vecTrailPos = Vector(this.m_vecPos[0] + 20 - 45, this.m_vecPos[1] + 15 - 50);
		vecTrailPos[0] += int(sin(this.m_fRotation + 0.014) * -30);
		vecTrailPos[1] -= int(cos(this.m_fRotation + 0.014) * -30);
		R_DrawSprite(this.m_hTrail, vecTrailPos, this.m_iTrailIndex, this.m_fRotation + 6.30 / 2, Vector(-1, -1), 0.3, 0.3, false, Color(0, 0, 0, 0));
		
		this.m_iTrailIndex++;
		if (this.m_iTrailIndex >= 64)
			this.m_iTrailIndex = 0;
		
		R_DrawSprite(this.m_hMissile, this.m_vecPos, 0, this.m_fRotation, Vector(-1, -1), 0.5, 0.5, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return true;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return (this.m_tmrLifeTime.IsElapsed()) || (this.m_bHasCollided);
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
		return 50;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "CMissile";
	}
}
const int AC_TEAM_1 = 1;
const int AC_TEAM_2 = 2;
class color_s
{
	uint8 r, g, b, a;
}
class CAircraft : IScriptedEntity
{
	Vector m_vecPos;
	float m_fRotation;
	Model m_oModel;
	SpriteHandle m_hAicraft;
	SpriteHandle m_hShadow;
	float m_fSpeed;
	Timer m_tmrMovement;
	Timer m_tmrRotation;
	Timer m_tmrChangeRot;
	float m_fRotChangeDir;
	DamageValue m_dvHealth;
	int m_iTeam;
	Timer m_tmrAttack;
	SoundHandle m_hNoise;
	SoundHandle m_hFire;
	SoundHandle m_hExplode;
	Timer m_tmrNoise;
	
	CAircraft()
    {
		this.m_fSpeed = 4.5;
		this.m_dvHealth = 100;
    }
	
	void SetTeam(int iTeam)
	{
		this.m_iTeam = iTeam;
	}
	
	int GetTeam(void)
	{
		return this.m_iTeam;
	}
	
	void Move(void)
	{
		//Set next position according to view
		this.m_vecPos[0] += int(sin(this.m_fRotation + 0.014) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fRotation + 0.014) * this.m_fSpeed);
		
		//Correct position according to screen pos
		if (this.m_vecPos[0] < 0)
			this.m_vecPos[0] = Wnd_GetWindowCenterX() * 2;
		else if (this.m_vecPos[0] > Wnd_GetWindowCenterX() * 2)
			this.m_vecPos[0] = 0;
		if (this.m_vecPos[1] < 0)
			this.m_vecPos[1] = Wnd_GetWindowCenterY() * 2;
		else if (this.m_vecPos[1] > Wnd_GetWindowCenterY() * 2)
			this.m_vecPos[1] = 0;
	}
	
	void CheckForTarget()
	{
		//Check for opponents in range
		
		const int AC_ATTACK_RANGE = 550;
		
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) {
			IScriptedEntity@ pEntity = @Ent_GetEntityHandle(i);
			if ((@pEntity != null) && (@pEntity != @this) && (pEntity.GetName().length > 0) && (pEntity.IsDamageable() != DAMAGEABLE_NO) && (pEntity.GetName() != this.GetName())) {
				if (this.m_vecPos.Distance(pEntity.GetPosition()) <= AC_ATTACK_RANGE) {
					if (this.m_tmrAttack.IsElapsed()) {
						this.m_tmrAttack.Reset();
						this.Attack(@pEntity);
					}
				}
			}
		}
	}
	
	void Attack(IScriptedEntity@ pEntity)
	{
		//Attack opponent entity
		
		CMissile @missile = CMissile();
		missile.SetTarget(@pEntity);
		missile.SetRotation(this.m_fRotation);
		Ent_SpawnEntity(@missile, this.m_vecPos);
		
		S_PlaySound(this.m_hFire, 8);
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = Util_Random(1, 630) / 100;
		this.m_hAicraft = R_LoadSprite(g_szToolPath + "aircraft_sprite.png", 1, 81, 55, 1, true);
		this.m_hShadow = R_LoadSprite(g_szToolPath + "aircraft_shadow.png", 1, 81, 55, 1, true);
		this.m_hNoise = S_QuerySound(g_szToolPath + "plane.wav");
		this.m_hFire = S_QuerySound(g_szToolPath + "fire.wav");
		this.m_hExplode = S_QuerySound(g_szToolPath + "explosion.wav");
		S_PlaySound(this.m_hNoise, 10);
		this.m_tmrMovement.SetDelay(10);
		this.m_tmrMovement.Reset();
		this.m_tmrMovement.SetActive(true);
		this.m_tmrRotation.SetDelay(Util_Random(5000, 10000));
		this.m_tmrRotation.Reset();
		this.m_tmrRotation.SetActive(true);
		this.m_tmrAttack.SetDelay(3500);
		this.m_tmrAttack.Reset();
		this.m_tmrAttack.SetActive(true);
		this.m_tmrNoise.SetDelay(17000);
		this.m_tmrNoise.Reset();
		this.m_tmrNoise.SetActive(true);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(81, 55));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(81 / 2, 55 / 2));
		this.m_oModel.Initialize2(bbox, this.m_hAicraft);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosion @expl = CExplosion();
		Ent_SpawnEntity(@expl, Vector(this.m_vecPos[0] - 30, this.m_vecPos[1] - 30));
		
		S_PlaySound(this.m_hExplode, 8);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Process movement
		this.m_tmrMovement.Update();
		if (this.m_tmrMovement.IsElapsed()) {
			this.m_tmrMovement.Reset();
			this.Move();
		}
		
		//Process rotation
		if (this.m_tmrRotation.IsActive()) {
			this.m_tmrRotation.Update();
			if (this.m_tmrRotation.IsElapsed()) {
				this.m_tmrRotation.SetActive(false);
				this.m_tmrChangeRot.SetDelay(Util_Random(250, 1500));
				this.m_tmrChangeRot.Reset();
				this.m_tmrChangeRot.SetActive(true);
				this.m_fRotChangeDir = (Util_Random(0, 2) == 0) ? -0.1 : 0.1;
			}
		}
		if (this.m_tmrChangeRot.IsActive()) {
			this.m_tmrChangeRot.Update();
			this.m_fRotation += this.m_fRotChangeDir;
			if (this.m_tmrChangeRot.IsElapsed()) {
				this.m_tmrChangeRot.SetActive(false);
				this.m_tmrRotation.SetDelay(Util_Random(5000, 10000));
				this.m_tmrRotation.Reset();
				this.m_tmrRotation.SetActive(true);
			}
		}
		
		//Process noise sound timer
		this.m_tmrNoise.Update();
		if (this.m_tmrNoise.IsElapsed()) {
			this.m_tmrNoise.Reset();
			S_PlaySound(this.m_hNoise, 7);
		}
		
		//Process attack permission timer
		this.m_tmrAttack.Update();
		
		//Check for opponent attacking
		this.CheckForTarget();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		color_s sDrawingColor;
		if (this.GetTeam() == AC_TEAM_1) {
			sDrawingColor.r = 105;
			sDrawingColor.g = 201;
			sDrawingColor.b = 224;
			sDrawingColor.a = 255;
		} else {
			sDrawingColor.r = 255;
			sDrawingColor.g = 0;
			sDrawingColor.b = 0;
			sDrawingColor.a = 255;
		}
		
		R_DrawSprite(this.m_hShadow, Vector(this.m_vecPos[0], this.m_vecPos[1] + 35), 0, this.m_fRotation, Vector(-1, -1), 0.7, 0.7, false, Color(0, 0, 0, 0));
		R_DrawSprite(this.m_hAicraft, this.m_vecPos, 0, this.m_fRotation, Vector(-1, -1), 0.0, 0.0, true, Color(sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, sDrawingColor.a));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_dvHealth == 0;
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
		int iHealth = int(this.m_dvHealth);
		iHealth -= dv;
		if (iHealth < 0)
			iHealth = 0;
		
		this.m_dvHealth = DamageValue(iHealth);
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
		return (this.GetTeam() == AC_TEAM_1) ? "Combat.Team1" : "Combat.Team2";
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
	if (g_bSelectionStatus) {
		string szInfoStr = (g_bTeamSelection) ? "[Aircraft] Team 1" : "[Aircraft] Team 2";
		R_DrawString(R_GetDefaultFont(), szInfoStr, Vector(g_vecMousePos[0], g_vecMousePos[1]), Color(128, 0, 64, 150));
	}
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	CAircraft @obj = CAircraft();
	obj.SetTeam((g_bTeamSelection) ? AC_TEAM_1 : AC_TEAM_2);
	Ent_SpawnEntity(@obj, vAtPos);
}

/*
	This function is called for any keyboard key event. This is even the case if 
	this tool is not currently selected.
*/
void CDG_API_KeyEvent(int iKey, bool bDown)
{
	if ((iKey == g_GameKeys.vkTeamSelect) && (bDown))
		g_bTeamSelection = !g_bTeamSelection;
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
	The gamekeys holds virtual key codes of bound actions. The tool path can be used to load objects from. Return 
	true on success, otherwise false.
*/
bool CDG_API_QueryToolInfo(HostVersion hvVersion, ToolInfo &out info, const GameKeys& in gamekeys, const string &in szToolPath)
{
	info.szName = "Aicraft";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.1";
	info.szContact = "Daniel Brendel<at>gmail<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Military";
	info.iCursorWidth = 64;
	info.iCursorHeight = 64;
	info.uiTriggerDelay = 250;

	g_szToolPath = szToolPath;
	g_GameKeys = gamekeys;
	
	return true;
}