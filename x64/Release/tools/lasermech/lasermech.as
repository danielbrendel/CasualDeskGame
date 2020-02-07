/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2020 by Daniel Brendel
	
	Tool: Laser Mech (developed by Daniel Brendel)
	Version: 0.2
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
class CDamageDecal : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	Timer m_oLifeTime;
	SpriteHandle m_hSprite;
	
	CDamageDecal()
    {
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(g_szToolPath + "decal.png", 1, 64, 64, 1, false);
		this.m_oLifeTime.SetDelay(10);
		this.m_oLifeTime.Reset();
		this.m_oLifeTime.SetActive(true);
		this.m_oModel.Alloc();
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(64, 64));
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
class CLaserEntity : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecTarget;
	Model m_oModel;
	SpriteHandle m_hLaser;
	float m_fRotation;
	float m_fSpeed;
	bool m_bRemove;
	
	CLaserEntity()
    {
		this.m_fSpeed = 75.0;
		this.m_bRemove = false;
    }
	
	void SetRotation(float fRotation)
	{
		//Set rotation
		
		this.m_fRotation = fRotation;
	}
	
	void SetTarget(const Vector& in vec)
	{
		//Set target position
		
		this.m_vecTarget = vec;
	}
	
	void Move()
	{
		//Move laser
		
		//Set next position according to view
		this.m_vecPos[0] += int(sin(this.m_fRotation + 0.014) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fRotation + 0.014) * this.m_fSpeed);
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hLaser = R_LoadSprite(g_szToolPath + "laser" + Util_Random(1, 5) + ".png", 1, 60, 99, 1, true);
		this.m_oModel.Alloc();
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(60, 99));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(60 / 2, 99 / 2));
		this.m_oModel.Initialize2(bbox, this.m_hLaser);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.Move(); //Move laser
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hLaser, this.m_vecPos, 0, this.m_fRotation, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return true;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return (((this.m_vecPos[0] == this.m_vecTarget[0]) && (this.m_vecPos[1] == this.m_vecTarget[1])) || (this.m_bRemove)); //Remove laser when reached position
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
		this.m_bRemove = true;
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
		return "";
	}
}
const int MECH_TEAM_1 = 1;
const int MECH_TEAM_2 = 2;
class color_s
{
	uint8 r, g, b, a;
}
class CLaserMech : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	SpriteHandle m_hMech;
	bool m_bWalking;
	bool m_bInCombat;
	float m_fRotation;
	Timer m_tmrRotChange;
	Timer m_tmrWalkSound;
	SoundHandle m_hStep;
	float m_fSpeed;
	int m_iHealth;
	DamageValue m_dvDamage;
	IScriptedEntity@ m_pTarget;
	int m_iCombatActivationRange;
	int m_iAttackRange;
	Timer m_tmrAttack;
	bool m_bLaserCanon;
	SoundHandle m_hAttack;
	bool m_bFocusSound;
	SoundHandle m_hFocus;
	uint8 m_ucTeam;
	
	uint8 GetTeam()
	{
		return this.m_ucTeam;
	}
	
	void SetTeam(uint8 ucTeam)
	{
		this.m_ucTeam = ucTeam;
	}
	
	void Move()
	{
		//Move mech
		
		//Set next position according to view
		this.m_vecPos[0] += int(sin(this.m_fRotation + 0.014) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fRotation + 0.014) * this.m_fSpeed);
		
		//Correct if out of screen
		if (this.m_vecPos[0] < 0) this.m_vecPos[0] = 0; else if (this.m_vecPos[0] > Wnd_GetWindowCenterX() * 2) this.m_vecPos[0] = Wnd_GetWindowCenterX() * 2;
		if (this.m_vecPos[1] < 0) this.m_vecPos[1] = 0; else if (this.m_vecPos[1] > Wnd_GetWindowCenterY() * 2) this.m_vecPos[1] = Wnd_GetWindowCenterY() * 2;
	}
	
	void AimTo(const Vector& in vecPos)
	{
		//Calculate aim rotation
		
		if (!this.m_bInCombat)
			return;
		
		float flAngle = atan2(float(vecPos[1] - this.m_vecPos[1]), float(vecPos[0] - this.m_vecPos[0]));
		this.m_fRotation = flAngle + 6.30 / 4;
	}
	
	void CheckForTargets()
	{
		//Check for targets in range
		
		//Iterate through entities
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) {
			IScriptedEntity@ pEntity = @Ent_GetEntityHandle(i);
			if ((@pEntity != null) && (@pEntity != @this) && (Ent_IsValid(@pEntity)) && (pEntity.IsDamageable() != DAMAGEABLE_NO)) { //Validate entity
				//Ignore non-identifying and own class entities
				string szEntName = pEntity.GetName();
				if ((szEntName.length == 0) || (szEntName == this.GetName())) {
					continue;
				}
				
				//Check for range
				if (this.m_vecPos.Distance(pEntity.GetPosition()) <= this.m_iCombatActivationRange) {
					@this.m_pTarget = @pEntity; //Use this entity as target
					this.m_bInCombat = true; //Indicate combat
					break;
				}
			}
		}
		
		//Validate target entity
		if (!Ent_IsValid(this.m_pTarget)) {
			@this.m_pTarget = null;
			this.m_bInCombat = false;
			this.m_bWalking = true;
			if (this.m_bFocusSound)
				this.m_bFocusSound = false;
		} else {
			//Handle focus sound
			if (!this.m_bFocusSound) {
				this.m_bFocusSound = true;
				S_PlaySound(this.m_hFocus, 9);
			}
		}
	}
	
	void InitAttack()
	{
		//Init attacking
		
		if (!this.m_tmrAttack.IsActive()) {
			this.m_tmrAttack.SetDelay(475);
			this.m_tmrAttack.Reset();
			this.m_tmrAttack.SetActive(true);
			this.m_bLaserCanon = (Util_Random(0, 1) == 0) ? true : false;
		}
	}
	
	void Fire()
	{
		//Fire laser
		
		//Instantiate entity object
		CLaserEntity @obj = CLaserEntity();
		
		//Calculate position
		Vector vecLaserPos = this.m_vecPos;
		Vector vecCenter = this.m_oModel.GetCenter();
		vecLaserPos[0] += vecCenter[0];
		vecLaserPos[1] += vecCenter[1];
		vecLaserPos[0] += int(sin(this.m_fRotation + 0.014) * 1.5);
		vecLaserPos[1] -= int(cos(this.m_fRotation + 0.014) * 1.5);
		if (this.m_bLaserCanon) {
			vecLaserPos[0] += int(sin(this.m_fRotation + 0.014 + 6.30 / 4) * 20);
			vecLaserPos[1] -= int(cos(this.m_fRotation + 0.014 + 6.30 / 4) * 20);
		} else {
			vecLaserPos[0] += int(sin(this.m_fRotation + 0.014 - 6.30 / 4) * 20);
			vecLaserPos[1] -= int(cos(this.m_fRotation + 0.014 - 6.30 / 4) * 20);
		}
		this.m_bLaserCanon = !this.m_bLaserCanon;
		
		//Set rotation
		obj.SetRotation(this.m_fRotation);
		
		//Set target position
		obj.SetTarget(this.m_pTarget.GetPosition());
		
		//Spawn entity
		Ent_SpawnEntity(@obj, vecLaserPos);
		
		//Play sound
		S_PlaySound(this.m_hAttack, 10);
		
		//Damage target entity
		this.m_pTarget.OnDamage(this.m_dvDamage);
	}
	
	CLaserMech()
    {	
		this.m_bWalking = true;
		this.m_bInCombat = false;
		this.m_fSpeed = 2.0;
		this.m_iHealth = 400;
		this.m_dvDamage = 100;
		this.m_iCombatActivationRange = 350;
		this.m_iAttackRange = 310;
		@this.m_pTarget = null;
		this.m_bFocusSound = false;
    }
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = float(Util_Random(1, 630)) / 100;
		this.m_hMech = R_LoadSprite(g_szToolPath + "mech.png", 1, 59, 52, 1, true);
		this.m_hStep = S_QuerySound(g_szToolPath + "step.wav");
		this.m_hAttack = S_QuerySound(g_szToolPath + "laser.wav");
		this.m_hFocus = S_QuerySound(g_szToolPath + "target.wav");
		this.m_tmrRotChange.SetDelay(Util_Random(5000, 7000));
		this.m_tmrRotChange.Reset();
		this.m_tmrRotChange.SetActive(true);
		this.m_tmrWalkSound.SetDelay(750);
		this.m_tmrWalkSound.Reset();
		this.m_tmrWalkSound.SetActive(true);
		this.m_tmrAttack.SetActive(false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(59, 12));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(59 / 2, 12 / 2));
		this.m_oModel.Initialize2(bbox, this.m_hMech);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		//Spawn explosion at destination
		CExplosion@ expl = CExplosion();
		Ent_SpawnEntity(@expl, Vector(this.m_vecPos[0] - 30, this.m_vecPos[1] - 51));
		
		//Play disposal sound
		SoundHandle hDispose = S_QuerySound(g_szToolPath + "dispose.wav");
		S_PlaySound(hDispose, 10);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Handle walking
		if (this.m_bWalking) {
			//Process walking direction change
			this.m_tmrRotChange.Update();
			if (this.m_tmrRotChange.IsElapsed()) {
				this.m_tmrRotChange.SetDelay(Util_Random(5000, 7000));
				this.m_tmrRotChange.Reset();
				this.m_fRotation = float(Util_Random(1, 630)) / 100;
			}
			
			//Handle step sound
			this.m_tmrWalkSound.Update();
			if (this.m_tmrWalkSound.IsElapsed()) {
				this.m_tmrWalkSound.Reset();
				S_PlaySound(this.m_hStep, 8);
			}
			
			//Move entity
			this.Move();
		}
		
		//Handle combat
		this.CheckForTargets(); //Check for targets
		if (this.m_bInCombat) {
			if (@this.m_pTarget != null) {
				//Aim at target
				this.AimTo(this.m_pTarget.GetPosition());
				
				//Activate attacking if in range
				if (this.m_vecPos.Distance(this.m_pTarget.GetPosition()) <= this.m_iAttackRange) {
					this.m_bWalking = false; //Stop walking
					this.InitAttack();
				} else {
					//Lost target, so continue walking
					this.m_bWalking = true;
					this.m_tmrAttack.SetActive(false);
				}
				
				//Handle attacking
				if (this.m_tmrAttack.IsActive()) {
					this.m_tmrAttack.Update();
					if (this.m_tmrAttack.IsElapsed()) {
						this.Fire();
						this.m_tmrAttack.Reset();
					}
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
		color_s sDrawingColor;
		if (this.GetTeam() == MECH_TEAM_1) {
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
		
		R_DrawSprite(this.m_hMech, this.m_vecPos, 0, this.m_fRotation, Vector(-1, -1), 0.0, 0.0, true, Color(sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, sDrawingColor.a));
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_iHealth <= 0;
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
		this.m_iHealth -= dv;
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
		return 10;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return (this.GetTeam() == MECH_TEAM_1) ? "Combat.Team1" : "Combat.Team2";
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
		string szInfoStr = (g_bTeamToggle) ? "[Mech] Team 1" : "[Mech] Team 2";
		R_DrawString(R_GetDefaultFont(), szInfoStr, Vector(g_vecMousePos[0], g_vecMousePos[1] - 20), Color(128, 0, 64, 150));
	}
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	CLaserMech @obj = CLaserMech();
	obj.SetTeam((g_bTeamToggle) ? MECH_TEAM_1 : MECH_TEAM_2);
	Ent_SpawnEntity(@obj, vAtPos);
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
	The gamekeys holds virtual key codes of bound actions. The tool path can be used to load objects from. Return 
	true on success, otherwise false.
*/
bool CDG_API_QueryToolInfo(HostVersion hvVersion, ToolInfo &out info, const GameKeys& in gamekeys, const string &in szToolPath)
{
	info.szName = "Laser Mech";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.2";
	info.szContact = "dbrendel1988<at>yahoo<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Military";
	info.iCursorWidth = 64;
	info.iCursorHeight = 64;
	info.uiTriggerDelay = 500;
	
	g_szToolPath = szToolPath;
	g_GameKeys = gamekeys;

	return true;
}