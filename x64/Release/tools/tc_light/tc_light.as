/*
	Casual Desktop Game (dnycasualDeskGame) v0.8 developed by Daniel Brendel
	
	(C) 2018 - 2019 by Daniel Brendel
	
	Tool: TC Light Tank (developed by Daniel Brendel)
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
const int TANK_TEAM_1 = 1;
const int TANK_TEAM_2 = 2;
class TankAttributes
{
	int Health;
	int Damage;
	int Speed;
	float Rotation;
	uint8 Team;
	Vector BodySize;
	Vector HeadSize;
}
class color_s
{
	uint8 r, g, b, a;
}
class CBaseTank : IScriptedEntity
{
	Vector m_vecPosition;
	Model m_oModel;
	SpriteHandle m_hSprBody;
	SpriteHandle m_hSprHead;
	float m_flBodyRot;
	float m_flHeadRot;
	TankAttributes m_sTankAttrs;
	Timer m_tmrAiming;
	float m_flAimDest;
	float m_flAimStep;
	Timer m_tmrMovement;
	Timer m_tmrDirChange;
	Timer m_tmrDirMove;
	float m_flDirDest;
	IScriptedEntity@ m_pTarget;
	Timer m_tmrAttack;
	SoundHandle m_hFireSound;
	Timer m_tmrMuzzle;
	SpriteHandle m_hMuzzle;
	
	CBaseTank()
    {
		@m_pTarget = null;
    }
	
	//Getters
	int GetHealth() { return this.m_sTankAttrs.Health; }
	int GetDamage() { return this.m_sTankAttrs.Damage; }
	int GetSpeed() { return this.m_sTankAttrs.Speed; }
	float GetRotation_() { return this.m_sTankAttrs.Rotation; }
	uint8 GetTeam() { return this.m_sTankAttrs.Team; }
	
	//Setters
	void SetHealth(int iHealth) { this.m_sTankAttrs.Health = iHealth; }
	void SetDamage(int iDamage) { this.m_sTankAttrs.Damage = iDamage; }
	void SetSpeed(int iSpeed) { this.m_sTankAttrs.Speed = iSpeed; }
	void SetRotation_(float fRotation) { this.m_sTankAttrs.Rotation = fRotation; }
	void SetTeam(uint8 ui8Team) { this.m_sTankAttrs.Team = ui8Team; }
	void SetBodySize(const Vector& in vec) { this.m_sTankAttrs.BodySize = vec; }
	void SetHeadSize(const Vector& in vec) { this.m_sTankAttrs.HeadSize = vec; }
	
	//Actions
	void AimTo(const Vector& in vecPos)
	{
		//Init aiming process
		
		//if ((this.m_tmrAiming.IsActive()) || (this.m_tmrAttack.IsActive()))
		//	return;
		
		//Calculate aim rotation
		float flAngle = atan2(float(vecPos[1] - this.m_vecPosition[1]), float(vecPos[0] - this.m_vecPosition[0]));
		this.m_flHeadRot = flAngle + 6.30 / 4;
		
		//Activate attack timing if not already
		if (!this.m_tmrAttack.IsActive()) {
			this.m_tmrAttack.SetDelay(Util_Random(2000, 4500));
			this.m_tmrAttack.Reset();
			this.m_tmrAttack.SetActive(true);
		}
		//Calculate rotation value
		/*float flAngle = atan2(float(vecPos[1] - this.m_vecPosition[1]), float(vecPos[0] - this.m_vecPosition[0]));
		this.m_flAimDest = flAngle + 6.30 / 4;
		
		//Calculate step value
		float flAddRange = this.m_flAimDest - this.m_flHeadRot;
		float flSubRange = 6.30 - this.m_flAimDest + this.m_flHeadRot;
		if (flAddRange < flSubRange)
			this.m_flAimStep = this.m_sTankAttrs.Rotation;
		else
			this.m_flAimStep = -this.m_sTankAttrs.Rotation;
		
		//Init timer
		this.m_tmrAiming.SetDelay(100);
		this.m_tmrAiming.Reset();
		this.m_tmrAiming.SetActive(true);*/
	}
	void Move()
	{
		//Move tank
		
		//Set next position according to view
		this.m_vecPosition[0] += int(sin(this.m_flBodyRot + 0.014) * this.m_sTankAttrs.Speed);
		this.m_vecPosition[1] -= int(cos(this.m_flBodyRot + 0.014) * this.m_sTankAttrs.Speed);
		
		//Correct if out of screen
		if (this.m_vecPosition[0] < 0) this.m_vecPosition[0] = 0; else if (this.m_vecPosition[0] > Wnd_GetWindowCenterX() * 2) this.m_vecPosition[0] = Wnd_GetWindowCenterX() * 2;
		if (this.m_vecPosition[1] < 0) this.m_vecPosition[1] = 0; else if (this.m_vecPosition[1] > Wnd_GetWindowCenterY() * 2) this.m_vecPosition[1] = Wnd_GetWindowCenterY() * 2;
	}
	void ValidateTarget()
	{
		//Validate target
		
		if (!Ent_IsValid(this.m_pTarget))
			@this.m_pTarget = null;
	}
	void CheckForTargets()
	{
		//Check for targets and set target if not already
		
		const int MAX_CHECK_RANGE = 300;
		
		IScriptedEntity@ pTargetEnt = null;
		
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) { //Loop through entities
			IScriptedEntity@ pEntity = @Ent_GetEntityHandle(i); //Get current entity
			if ((@pEntity != null) && (@pEntity != @this) && (pEntity.GetName().length() > 0) && (pEntity.IsDamageable() != DAMAGEABLE_NO)) { //Validate entity
				//Filter teammates
				if (pEntity.GetName() == this.GetName()) {
					continue;
				}
				
				//Check if enemy in range
				if ((this.m_vecPosition.Distance(pEntity.GetPosition()) < MAX_CHECK_RANGE)) {
					@pTargetEnt = @pEntity;
				}
			}
		}
		
		@this.m_pTarget = @pTargetEnt;
	}
	void AttackTarget()
	{
		//Attack target if any
		
		//Deactivate attacking if no target is set
		if (@this.m_pTarget == null) {
			this.m_tmrAttack.SetActive(false);
			this.m_flHeadRot = this.m_flBodyRot;
			return;
		}
		
		//Perform aiming at target
		this.AimTo(this.m_pTarget.GetPosition());
	}
	void Fire()
	{
		//Fire at target
		
		if (@this.m_pTarget == null)
			return;
		
		//Play fire sound
		S_PlaySound(this.m_hFireSound, 10);
		//Calculate sub-entity position
		Vector vecPosition = this.m_pTarget.GetPosition();
		Vector vecCenter = this.m_pTarget.GetModel().GetCenter();
		Vector vecAbsPos = vecPosition + vecCenter;
		//Spawn explosion at destination
		CExplosion@ expl = CExplosion();
		Ent_SpawnEntity(@expl, Vector(vecPosition[0] - this.m_sTankAttrs.BodySize[0], vecPosition[1] - this.m_sTankAttrs.BodySize[1]));
		//Spawn decal
		CDamageDecal@ dcl = CDamageDecal();
		Ent_SpawnEntity(@dcl, Vector(vecAbsPos[0] - this.m_sTankAttrs.BodySize[0] / 2, vecAbsPos[1] - this.m_sTankAttrs.BodySize[1] / 2));
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPosition = vec;
		float flInitRot = float(Util_Random(1, 630)) / 100;
		this.m_flBodyRot = flInitRot;
		this.m_flHeadRot = flInitRot;
		this.m_hSprBody = R_LoadSprite(g_szToolPath + "tc_l_body_team" + this.m_sTankAttrs.Team + ".png", 1, this.m_sTankAttrs.BodySize[0], this.m_sTankAttrs.BodySize[1], 1, false);
		this.m_hSprHead = R_LoadSprite(g_szToolPath + "tc_l_head_team" + this.m_sTankAttrs.Team + ".png", 1, this.m_sTankAttrs.HeadSize[0], this.m_sTankAttrs.HeadSize[1], 1, false);
		this.m_hMuzzle = R_LoadSprite(g_szToolPath + "muzzle.png", 1, 256, 256, 1, false);
		this.m_hFireSound = S_QuerySound(g_szToolPath + "fire.wav");
		this.m_tmrAiming.SetActive(false);
		this.m_tmrAttack.SetActive(false);
		this.m_tmrDirMove.SetActive(false);
		this.m_tmrMuzzle.SetActive(false);
		this.m_tmrMovement.SetDelay(100);
		this.m_tmrMovement.Reset();
		this.m_tmrMovement.SetActive(true);
		this.m_tmrDirChange.SetDelay(Util_Random(5000, 10000));
		this.m_tmrDirChange.Reset();
		this.m_tmrDirChange.SetActive(true);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(this.m_sTankAttrs.BodySize[0], this.m_sTankAttrs.BodySize[1]));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(this.m_sTankAttrs.BodySize[0] / 2, this.m_sTankAttrs.BodySize[1] / 2));
		this.m_oModel.Initialize2(bbox, this.m_hSprBody);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		//Spawn explosion at destination
		CExplosion@ expl = CExplosion();
		Ent_SpawnEntity(@expl, Vector(this.m_vecPosition[0] - this.m_sTankAttrs.BodySize[0], this.m_vecPosition[1] - this.m_sTankAttrs.BodySize[1]));
		
		//Play disposal sound
		SoundHandle hDispose = S_QuerySound(g_szToolPath + "dispose.wav");
		S_PlaySound(hDispose, 10);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.ValidateTarget(); //Validate target
	
		//Process movement
		if ((this.m_tmrMovement.IsActive()) && (!this.m_tmrAttack.IsActive())) {
			this.m_tmrMovement.Update();
			if (this.m_tmrMovement.IsElapsed()) {
				this.Move();
			}
		}
		
		//Process directory change
		if ((this.m_tmrDirChange.IsActive()) && (!this.m_tmrAttack.IsActive())) {
			this.m_tmrDirChange.Update();
			if (this.m_tmrDirChange.IsElapsed()) {
				//Calculate new destination body rotation value
				this.m_flDirDest = float(Util_Random(1, 630)) / 100;
				//Activate rotation change timer
				this.m_tmrDirMove.SetDelay(100);
				this.m_tmrDirMove.Reset();
				this.m_tmrDirMove.SetActive(true);
				//Deactivate this timer
				this.m_tmrDirChange.SetActive(false);
			}
		}
		if (this.m_tmrDirMove.IsActive()) {
			this.m_tmrDirMove.Update();
			//Add rotation value to body rotation
			this.m_flBodyRot += this.m_sTankAttrs.Rotation;
			//Correct rotation value if required
			if (this.m_flBodyRot < 0.00) this.m_flBodyRot = 6.30;
			else if (this.m_flBodyRot > 6.30) this.m_flBodyRot = 0.00;
			//Deactivate body rotation change if reached dest rotation
			if (closeTo(this.m_flBodyRot, this.m_flDirDest, 0.01f)) {
				this.m_tmrDirMove.SetActive(false);
				this.m_tmrDirChange.SetDelay(Util_Random(5000, 10000));
				this.m_tmrDirChange.Reset();
				this.m_tmrDirChange.SetActive(true);
			}
		}
		
		//Process aiming
		if (this.m_tmrAiming.IsActive()) {
			this.m_tmrAiming.Update();
			//Add rotation step to current head rot
			this.m_flHeadRot += this.m_flAimStep;
			//Correct rotation value if required
			if (this.m_flHeadRot < 0.00) this.m_flHeadRot = 6.30;
			else if (this.m_flHeadRot > 6.30) this.m_flHeadRot = 0.00;
			//Deactivate aiming if rotation has reached the dest rotation and then activate firing
			if (int(this.m_flHeadRot * 100) == int(this.m_flAimDest * 100)) {
				this.m_tmrAiming.SetActive(false);
				this.m_tmrAttack.SetDelay(Util_Random(2000, 4500));
				this.m_tmrAttack.Reset();
				this.m_tmrAttack.SetActive(true);
			}
		}
		
		//Process attacking
		if (this.m_tmrAttack.IsActive()) {
			this.m_tmrAttack.Update();
			if (this.m_tmrAttack.IsElapsed()) {
				this.m_tmrAttack.Reset();
				this.Fire();
				this.m_tmrMuzzle.SetDelay(10);
				this.m_tmrMuzzle.Reset();
				this.m_tmrMuzzle.SetActive(true);
			}
		}
		
		//Process muzzle flash
		if (this.m_tmrMuzzle.IsActive()) {
			this.m_tmrMuzzle.Update();
			if (this.m_tmrMuzzle.IsElapsed()) {
				this.m_tmrMuzzle.SetActive(false);
			}
		}
		
		//Process target related operations
		this.CheckForTargets();
		this.AttackTarget();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		color_s sDrawingColor;
		if (this.GetTeam() == TANK_TEAM_1) {
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
		
		R_DrawSprite(this.m_hSprBody, this.m_vecPosition, 0, this.m_flBodyRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		R_DrawSprite(this.m_hSprHead, Vector(this.m_vecPosition[0] - 11, this.m_vecPosition[1] - 3), 0, this.m_flHeadRot, Vector(-1, -1), 0.0, 0.0, true, Color(sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, sDrawingColor.a));
		
		if (this.m_tmrMuzzle.IsActive()) {
			Vector vecCenter = this.GetModel().GetCenter();
			Vector vecMuzzlePos(this.m_vecPosition[0] + vecCenter[0] - 128, this.m_vecPosition[1] + vecCenter[1] - 128);
			vecMuzzlePos[0] += int(sin(this.m_flHeadRot) * (this.m_sTankAttrs.HeadSize[0] + 25));
			vecMuzzlePos[1] -= int(cos(this.m_flHeadRot) * (this.m_sTankAttrs.HeadSize[0] + 25));
			R_DrawSprite(this.m_hMuzzle, vecMuzzlePos, 0, this.m_flHeadRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		}
	}
	
	//Indicate whether the user is allowed to clean this entity
	bool DoUserCleaning()
	{
		return false;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_sTankAttrs.Health <= 0;
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
		this.m_sTankAttrs.Health -= dv;
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
		return this.m_vecPosition;
	}
	
	//Return the rotation. This is actually not used by the host application, but might be useful to other entities
	float GetRotation()
	{
		return this.m_flBodyRot;
	}
	
	//Called for querying the damage value for this entity
	DamageValue GetDamageValue()
	{
		return this.m_sTankAttrs.Damage;
	}
	
	//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
	string GetName()
	{
		return "CBaseTank";
	}
}
class CLightTank : CBaseTank
{
	CLightTank()
	{
		this.SetHealth(500);
		this.SetDamage(4);
		this.SetSpeed(3);
		this.SetRotation_(0.03);
		this.SetTeam((g_bTeamToggle) ? TANK_TEAM_1 : TANK_TEAM_2);
		this.SetBodySize(Vector(34, 48));
		this.SetHeadSize(Vector(56, 56));
	}
	
	string GetName()
	{
		return (this.GetTeam() == TANK_TEAM_1) ? "Combat.Team1" : "Combat.Team2";
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
		string szInfoStr = (g_bTeamToggle) ? "[Light] Team 1" : "[Light] Team 2";
		R_DrawString(R_GetDefaultFont(), szInfoStr, Vector(g_vecMousePos[0], g_vecMousePos[1]), Color(128, 0, 64, 150));
	}
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	CLightTank @obj = CLightTank();
	Ent_SpawnEntity(@obj, vAtPos);
	SoundHandle hSpawnSound = S_QuerySound(g_szToolPath + "spawn.wav");
	S_PlaySound(hSpawnSound, 9);
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
	info.szName = "TC - Light Tank";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.2";
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