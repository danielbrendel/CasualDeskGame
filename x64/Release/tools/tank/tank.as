/*
	Casual Desktop Game (dnycasualDeskGame) v1.0 developed by Daniel Brendel
	
	(C) 2018 - 2020 by Daniel Brendel
	
	Tool: Tank (developed by Daniel Brendel)
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
	
	//Indicate if this entity is movable
	bool IsMovable()
	{
		return false;
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSelectionSize()
	{
		return this.m_vecPos;
	}
	
	//This method is used to set the movement destination position
	void MoveTo(const Vector& in vec)
	{
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
	
	//Indicate if this entity is movable
	bool IsMovable()
	{
		return false;
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSelectionSize()
	{
		return this.m_vecPos;
	}
	
	//This method is used to set the movement destination position
	void MoveTo(const Vector& in vec)
	{
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
	
	//Indicate if this entity is movable
	bool IsMovable()
	{
		return false;
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSelectionSize()
	{
		return this.m_vecPos;
	}
	
	//This method is used to set the movement destination position
	void MoveTo(const Vector& in vec)
	{
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
	
	//Indicate if this entity is movable
	bool IsMovable()
	{
		return false;
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSelectionSize()
	{
		return this.m_vecPos;
	}
	
	//This method is used to set the movement destination position
	void MoveTo(const Vector& in vec)
	{
	}
}
const int TANK_CANON_NEAREST_DEST = 100;
class CTank : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecHead;
	Model m_oModel;
	SpriteHandle m_hSprHead;
	SpriteHandle m_hSprBody;
	SpriteHandle m_hMuzzleFlash;
	Timer m_oHeadRotChange;
	Timer m_oHeadMovement;
	Timer m_oDirChange;
	Timer m_oLifeTime;
	Timer m_oHitSound;
	float m_fHeadRot;
	float m_fRotStep;
	float m_fWalkRot;
	float m_fSpeed;
	uint8 m_hvHealth;
	bool m_bHasShot;
	
	CTank()
    {
		this.m_hvHealth = 100;
		this.m_fSpeed = 1.5;
		this.m_fHeadRot = 0.0;
		this.m_bHasShot = false;
    }
	
	void Move(void)
	{
		//Set next position according to view
		this.m_vecPos[0] += int(sin(this.m_fWalkRot + 0.014) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fWalkRot + 0.014) * this.m_fSpeed);
	}
	
	Vector CanonNearestVector(void)
	{
		//Calculate nearest vector for canon
		
		Vector vResult;
		vResult[0] = (this.m_vecHead[0] + 50) + int(sin(this.m_fHeadRot) * TANK_CANON_NEAREST_DEST);
		vResult[1] = (this.m_vecHead[1] + 50) - int(cos(this.m_fHeadRot) * TANK_CANON_NEAREST_DEST);
		return vResult;
	}
	
	bool ShallRemove(void)
	{
		//Indicate whether entity shall be removed (life time exceeded, exceeded screen coords or zero health)
		return ((this.m_oLifeTime.IsElapsed())
			|| (this.m_vecPos[0] < -32) || (this.m_vecPos[0] > Wnd_GetWindowCenterX() * 2 + 32) || (this.m_vecPos[1] < -32) || (this.m_vecPos[1] > Wnd_GetWindowCenterY() * 2 + 32)
			|| (this.m_hvHealth == 0));
	}
	
	void Fire()
	{
		//Fire cannon
		
		//Calculate destination position
		Vector vDest;
		//Calculate random destination position
		float fRandomDest = float(Util_Random(TANK_CANON_NEAREST_DEST, 500));
		//Calculate destination vector according to destination
		vDest[0] = this.m_vecPos[0] + int(sin(this.m_fHeadRot) * fRandomDest);
		vDest[1] = this.m_vecPos[1] - int(cos(this.m_fHeadRot) * fRandomDest);
		//Correct destination position if would go out of screen
		if (vDest[0] < 0) vDest[0] = 0;
		if (vDest[1] < 0) vDest[1] = 0;
		if (vDest[0] > Wnd_GetWindowCenterX() * 2) vDest[0] = Wnd_GetWindowCenterX() * 2 - 128;
		if (vDest[1] > Wnd_GetWindowCenterY() * 2) vDest[1] = Wnd_GetWindowCenterY() * 2 - 128;
		//Attempt to find first entity in range
		IScriptedEntity@ pEntity = Ent_TraceLine(this.CanonNearestVector(), vDest, this);
		if (@pEntity != null) {
			//Set destination vector to be entities position
			vDest = pEntity.GetPosition();
		}
		//Spawn decal
		CDamageDecal@ dcl = CDamageDecal();
		Ent_SpawnEntity(@dcl, Vector(vDest[0] + 32, vDest[1] + 50));
		//Spawn explosion
		CExplosion@ expl = CExplosion();
		Ent_SpawnEntity(@expl, vDest);
		//Set indicator
		this.m_bHasShot = true;
	}
	
	void Hitsound()
	{
		//Play hitsound if allowed
		if (this.m_oHitSound.IsElapsed()) {
			this.m_oHitSound.Reset();
			SoundHandle hStartSound = S_QuerySound(g_szToolPath + "hit.wav");
			S_PlaySound(hStartSound, 8);
		}
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_fHeadRot = this.m_fWalkRot = float(Util_Random(1, 630)) / 100;
		this.m_vecPos = vec;
		this.m_vecPos[0] += 32;
		this.m_vecHead = this.m_vecPos;
		this.m_vecHead[0] -= 10;
		this.m_vecHead[1] -= 25;
		this.m_hSprHead = R_LoadSprite(g_szToolPath + "head.png", 1, 100, 100, 1, false);
		this.m_hSprBody = R_LoadSprite(g_szToolPath + "body.png", 1, 100, 100, 1, false);
		this.m_hMuzzleFlash = R_LoadSprite(g_szToolPath + "muzzle.png", 1, 256, 256, 1, false);
		this.m_oLifeTime.SetDelay(240000);
		this.m_oLifeTime.Reset();
		this.m_oLifeTime.SetActive(true);
		this.m_oDirChange.SetDelay(5000);
		this.m_oDirChange.Reset();
		this.m_oDirChange.SetActive(true);
		this.m_oHeadRotChange.SetDelay(3000);
		this.m_oHeadRotChange.Reset();
		this.m_oHeadRotChange.SetActive(true);
		this.m_oHeadMovement.SetActive(false);
		this.m_oHitSound.SetDelay(1000);
		this.m_oHitSound.Reset();
		this.m_oHitSound.SetActive(true);
		SoundHandle hStartSound = S_QuerySound(g_szToolPath + "spawn.wav");
		S_PlaySound(hStartSound, 9);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(2, 16), Vector(44, 44));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(50, 50));
		this.m_oModel.Initialize2(bbox, this.m_hSprBody);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CDetonation @obj = CDetonation();
		Ent_SpawnEntity(@obj, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Update timers
		this.m_oLifeTime.Update();
		this.m_oHitSound.Update();
	
		//Change rotation timing
		
		if (this.m_oHeadRotChange.IsActive()) {
			this.m_oHeadRotChange.Update();
			if (this.m_oHeadRotChange.IsElapsed()) { //If elapsed, rotation change shall be processed
				//Deactivate wait-timer
				this.m_oHeadRotChange.SetActive(false);
				//Set random rotation change direction factor
				this.m_fRotStep = ((Util_Random(0, 1) == 0) ? -1.0 : 1.0) / float(Util_Random(10, 100));
				//Initialize head rotation change timer
				this.m_oHeadMovement.SetDelay(Util_Random(500, 2500));
				this.m_oHeadMovement.Reset();
				this.m_oHeadMovement.SetActive(true);
			}
		}
		
		if (this.m_oHeadMovement.IsActive()) {
			this.m_oHeadMovement.Update();
			if (this.m_oHeadMovement.IsElapsed()) { //Handle stuff if rotation change is done
				//Deactivate this timer
				this.m_oHeadMovement.SetActive(false);
				//Reset rotation change wait-timer
				this.m_oHeadRotChange.SetDelay(Util_Random(2500, 3500));
				this.m_oHeadRotChange.Reset();
				this.m_oHeadRotChange.SetActive(true);
				//Fire cannon
				this.Fire();
			} else { //Handle stuff as long as the rotation is changed
				//Add to value in order to change rotation
				this.m_fHeadRot += this.m_fRotStep;
				//Correct value if exceeded ranges
				if (this.m_fHeadRot < 0.00) this.m_fHeadRot = 6.30;
				else if (this.m_fHeadRot > 6.30) this.m_fHeadRot = 0.00;
			}
		}
		
		//Change movement direction timing
		this.m_oDirChange.Update();
		if (this.m_oDirChange.IsElapsed()) {
			this.m_oDirChange.Reset();
			this.m_fWalkRot = float(Util_Random(1, 630)) / 100;
		}
		
		//Move to next position and update head position
		this.Move();
		this.m_vecHead = this.m_vecPos;
		this.m_vecHead[0] -= 10;
		this.m_vecHead[1] -= 25;
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		R_DrawSprite(this.m_hSprBody, this.m_vecPos, 0, this.m_fWalkRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		R_DrawSprite(this.m_hSprHead, this.m_vecPos, 0, this.m_fHeadRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		
		//Draw muzzle flash once when has shot
		if (this.m_bHasShot) {
			Vector vMuzzlePos(this.m_vecHead[0] - 50, this.m_vecHead[1] - 50);
			vMuzzlePos[0] += int(sin(this.m_fHeadRot) * 107.5);
			vMuzzlePos[1] -= int(cos(this.m_fHeadRot) * 107.5);
			R_DrawSprite(this.m_hMuzzleFlash, vMuzzlePos , 0, this.m_fHeadRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
			this.m_bHasShot = false;
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
		return this.ShallRemove();
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
		if (dv == 0) return;
		
		if (int(this.m_hvHealth) - int(dv) >= 0) { this.m_hvHealth -= dv; this.Hitsound(); }
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
		return "Tank";
	}
	
	//Indicate if this entity is movable
	bool IsMovable()
	{
		return false;
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSelectionSize()
	{
		return this.m_vecPos;
	}
	
	//This method is used to set the movement destination position
	void MoveTo(const Vector& in vec)
	{
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
	CTank @obj = CTank();
	Ent_SpawnEntity(@obj, Vector(vAtPos[0] - 50, vAtPos[1] - 50));
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
	info.szName = "Tank";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.2";
	info.szContact = "dbrendel1988<at>yahoo<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Military";
	info.iCursorWidth = 32;
	info.iCursorHeight = 32;
	info.uiTriggerDelay = 125;
	
	g_szToolPath = szToolPath;

	return true;
}