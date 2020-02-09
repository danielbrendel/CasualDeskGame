/*
	Casual Desktop Game (dnycasualDeskGame) v1.0 developed by Daniel Brendel
	
	(C) 2018 - 2020 by Daniel Brendel
	
	Tool: Soldier (developed by Daniel Brendel)
	Version: 0.2
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

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

string g_szToolPath = "";
bool g_bTeamToggle = false;
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
const int SOL_STATE_WALKING = 1;
const int SOL_STATE_COMBAT = 2;
const int SOL_TEAM_1 = 1;
const int SOL_TEAM_2 = 2;
const int MAX_CHECK_RANGE = 300;
const int STOP_WALK_RANGE = 45;
class color_s
{
	uint8 r, g, b, a;
}
class CBaseSoldierEntity : IScriptedEntity
{
	Vector m_vecPos;
	Model m_oModel;
	array<SpriteHandle> m_arrMovement;
	array<SpriteHandle> m_arrShooting;
	float m_fRotation;
	int m_iCurrentState;
	int m_iCurrentSprite;
	Timer m_tmrAnim;
	float m_fSpeed;
	Timer m_tmrAnimAction;
	Timer m_tmrStep;
	SoundHandle m_hStep;
	float m_fWalkRot;
	bool m_bWalkRotSwitch;
	IScriptedEntity@ m_pTarget;
	int m_iTeam;
	int m_iHealth;
	Timer m_tmrShoot;
	SoundHandle m_hFire;
	SpriteHandle m_hMuzzle;
	Timer m_tmrDrawMuzzle;
	DamageValue m_ucAttackDamage;
	Vector m_vecSelSize;
	Vector m_vecTargetDest;
	bool m_bMove;
	
	CBaseSoldierEntity()
    {
		this.m_fSpeed = 8;
		this.m_bWalkRotSwitch = false;
		@this.m_pTarget = null;
		this.m_iHealth = 50;
		this.m_ucAttackDamage = 5;
		this.m_vecSelSize = Vector(50, 50);
		this.m_bMove = false;
    }
	
	void SetTeam(int iTeam)
	{
		//Set team flag
		
		this.m_iTeam = iTeam;
	}
	
	int GetTeam()
	{
		//Return team
		
		return this.m_iTeam;
	}
	
	void EnterState(int iState)
	{
		//Enter state of soldier
		
		this.m_iCurrentSprite = 0;
		this.m_iCurrentState = iState;
		this.m_tmrAnim.Reset();
		
		switch (iState) {
			case SOL_STATE_WALKING:
				this.m_tmrAnimAction.SetDelay(100);
				this.m_tmrAnimAction.Reset();
				this.m_tmrStep.SetDelay(500);
				this.m_tmrStep.Reset();
				this.m_tmrStep.SetActive(true);
				this.m_tmrShoot.SetActive(false);
				this.m_tmrDrawMuzzle.SetActive(false);
				break;
			case SOL_STATE_COMBAT:
				this.m_tmrAnimAction.SetDelay(100);
				this.m_tmrAnimAction.Reset();
				this.m_tmrStep.SetActive(false);
				this.m_tmrShoot.SetDelay(500);
				this.m_tmrShoot.Reset();
				this.m_tmrShoot.SetActive(true);
				this.m_tmrDrawMuzzle.Reset();
				break;
			default:
				break;
		}
	}
	
	void CheckForTargets()
	{
		//Check for targets and set target if not already
		
		IScriptedEntity@ pTargetEnt = null;
		
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) { //Loop through entities
			IScriptedEntity@ pEntity = @Ent_GetEntityHandle(i); //Get current entity
			if ((@pEntity != null) && (@pEntity != @this) && (pEntity.GetName().length() > 0) && (pEntity.IsDamageable() != DAMAGEABLE_NO)) { //Validate entity
				//Filter teammates
				if (pEntity.GetName() == this.GetName()) {
					continue;
				}
				
				//Check if enemy in range
				if ((this.m_vecPos.Distance(pEntity.GetPosition()) < MAX_CHECK_RANGE)) {
					@pTargetEnt = @pEntity;
				}
			}
		}
		
		@this.m_pTarget = @pTargetEnt;
	}
	
	void ValidateTarget()
	{
		//Validate target
		
		if ((!Ent_IsValid(@this.m_pTarget)) || (this.m_vecPos.Distance(this.m_pTarget.GetPosition()) >= MAX_CHECK_RANGE)) {
			@this.m_pTarget = null;
			EnterState(SOL_STATE_WALKING);
		}
	}
	
	void AimTo(const Vector& in vecPos)
	{
		//Aim at position
		
		//Calculate aim rotation
		float flAngle = atan2(float(vecPos[1] - this.m_vecPos[1]), float(vecPos[0] - this.m_vecPos[0]));
		this.m_fRotation = flAngle + 6.30 / 4;
	}
	
	void Move(void)
	{
		//Update position according to speed
		
		if (!this.m_bMove)
			return;

		this.m_vecPos[0] += int(sin(this.m_fRotation) * this.m_fSpeed);
		this.m_vecPos[1] -= int(cos(this.m_fRotation) * this.m_fSpeed);
		
		if (this.m_vecPos.Distance(this.m_vecTargetDest) < STOP_WALK_RANGE) {
			this.m_bMove = false;
		}

		if (this.m_vecPos[0] < -313)
			this.m_vecPos[0] = Wnd_GetWindowCenterX() * 2;
		else if (this.m_vecPos[0] > Wnd_GetWindowCenterX() * 2 + 313)
			this.m_vecPos[0] = -313;
			
		if (this.m_vecPos[1] <  -206)
			this.m_vecPos[1] = Wnd_GetWindowCenterY() * 2;
		else if (this.m_vecPos[1] > Wnd_GetWindowCenterY() * 2 + 206)
			this.m_vecPos[1] = -206;
	}
	
	void UpdateAnimSpriteIndex()
	{
		//Update animation sprite index for current state
		
		this.m_iCurrentSprite++;
		
		switch (this.m_iCurrentState) {
			case SOL_STATE_WALKING:
				if (this.m_iCurrentSprite >= int(this.m_arrMovement.length() - 1)) this.m_iCurrentSprite = 0;
				break;
			case SOL_STATE_COMBAT:
				if (this.m_iCurrentSprite >= int(this.m_arrShooting.length() - 1)) this.m_iCurrentSprite = 0;
				break;
			default:
				break;
		}
	}
	
	//Called when the entity gets spawned. The position on the screen is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = Util_Random(0, 630) / 100.0;
		
		for (int i = 0; i < 1; i++) {
			this.m_arrMovement.insertLast(R_LoadSprite(g_szToolPath + "sprites\\move\\survivor-move_shotgun_" + i + ".png", 1, 313, 206, 1, false));
		}
		
		for (int i = 0; i < 3; i++) {
			this.m_arrShooting.insertLast(R_LoadSprite(g_szToolPath + "sprites\\shoot\\survivor-shoot_shotgun_" + i + ".png", 1, 313, 206, 1, false));
		}
		
		this.m_hMuzzle = R_LoadSprite(g_szToolPath + "muzzle.png", 1, 256, 256, 1, false);
		
		this.m_hStep = S_QuerySound(g_szToolPath + "step.wav");
		this.m_hFire = S_QuerySound(g_szToolPath + "shoot.wav");
		
		this.m_tmrAnim.SetDelay(250);
		this.m_tmrAnim.Reset();
		this.m_tmrAnim.SetActive(true);
		
		this.m_tmrShoot.SetActive(false);
		this.m_tmrDrawMuzzle.SetActive(false);
		this.m_tmrDrawMuzzle.SetDelay(10);
		
		this.m_tmrAnimAction.SetActive(true);
		this.EnterState(SOL_STATE_WALKING);
		
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(140, 95), Vector(43, 32));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(162, 112));
		this.m_oModel.Initialize2(bbox, this.m_arrMovement[0]);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CBloodSplash @obj = CBloodSplash();
		Ent_SpawnEntity(@obj, Vector(this.m_vecPos[0], this.m_vecPos[1]));
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//Update animation timer
		if (this.m_tmrAnim.IsActive()) {
			this.m_tmrAnim.Update();
			if (this.m_tmrAnim.IsElapsed()) {
				this.m_tmrAnim.Reset();
				this.UpdateAnimSpriteIndex();
			}
		}
		
		//Process action timer
		if (this.m_tmrAnimAction.IsActive()) {
			this.m_tmrAnimAction.Update();
			if (this.m_tmrAnimAction.IsElapsed()) {
				this.m_tmrAnimAction.Reset();
				
				switch (this.m_iCurrentState) {
					case SOL_STATE_WALKING:
						this.Move();
						this.CheckForTargets();
						if (@this.m_pTarget != null) EnterState(SOL_STATE_COMBAT);
						break;
					case SOL_STATE_COMBAT:
						this.ValidateTarget();
						if (@this.m_pTarget != null) this.AimTo(this.m_pTarget.GetPosition());
						break;
					default:
						break;
				}
			}
		}
		
		//Process step timer
		if ((this.m_tmrStep.IsActive()) && (this.m_bMove)) {
			this.m_tmrStep.Update();
			if (this.m_tmrStep.IsElapsed()) {
				this.m_tmrStep.Reset();
				this.m_bWalkRotSwitch = !this.m_bWalkRotSwitch;
				this.m_fWalkRot = (this.m_bWalkRotSwitch) ? -0.1 : 0.1;
				S_PlaySound(this.m_hStep, 8);
			}
		}
		
		//Process attacking
		if (this.m_tmrShoot.IsActive()) {
			this.m_tmrShoot.Update();
			if (this.m_tmrShoot.IsElapsed()) {
				this.m_tmrShoot.Reset();
				
				if (!this.m_tmrDrawMuzzle.IsActive()) {
					this.m_tmrDrawMuzzle.SetDelay(10);
					this.m_tmrDrawMuzzle.Reset();
					this.m_tmrDrawMuzzle.SetActive(true);
				}
				
				S_PlaySound(this.m_hFire, 8);
				
				if (@this.m_pTarget != null) {
					this.m_pTarget.OnDamage(this.m_ucAttackDamage);
				}
			}
		}
		
		//Process muzzle timer
		if (this.m_tmrDrawMuzzle.IsActive()) {
			this.m_tmrDrawMuzzle.Update();
			if (this.m_tmrDrawMuzzle.IsElapsed()) {
				this.m_tmrDrawMuzzle.SetActive(false);
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
		if (this.GetTeam() == SOL_TEAM_1) {
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
		
		//Draw animation sprite
		switch (this.m_iCurrentState) {
			case SOL_STATE_WALKING:
				R_DrawSprite(this.m_arrMovement[this.m_iCurrentSprite], Vector(this.m_vecPos[0] - 125, this.m_vecPos[1] - 90), 0, this.m_fRotation - 6.30 / 4 + this.m_fWalkRot, Vector(-1, -1), 0.15, 0.15, true, Color(sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, sDrawingColor.a));
				break;
			case SOL_STATE_COMBAT:
				R_DrawSprite(this.m_arrShooting[this.m_iCurrentSprite], Vector(this.m_vecPos[0] - 125, this.m_vecPos[1] - 90), 0, this.m_fRotation - 6.30 / 4, Vector(-1, -1), 0.15, 0.15, true, Color(sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, sDrawingColor.a));
				break;
			default:
				break;
		}
		
		if (this.m_tmrDrawMuzzle.IsActive()) {
			Vector vecCenter = this.GetModel().GetCenter();
			Vector vecMuzzlePos(this.m_vecPos[0] - 125 + 25, this.m_vecPos[1] - 90 - 25);
			vecMuzzlePos[0] += int(sin(this.m_fRotation) * 25);
			vecMuzzlePos[1] -= int(cos(this.m_fRotation) * 25);
			R_DrawSprite(this.m_hMuzzle, vecMuzzlePos, 0, this.m_fRotation - 6.30 / 4, Vector(-1, -1), 0.3, 0.3, false, Color(0, 0, 0, 0));
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
		this.m_iHealth -= int(dv);
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
		return "CBaseSoldierEntity";
	}
	
	//Indicate if this entity is movable
	bool IsMovable()
	{
		return true;
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSelectionSize()
	{
		return this.m_vecSelSize;
	}
	
	//This method is used to set the movement destination position
	void MoveTo(const Vector& in vec)
	{
		//Store target destination
		this.m_vecTargetDest = vec;
		
		//Calculate body rotation
		float flAngle = atan2(float(vec[1] - this.m_vecPos[1]), float(vec[0] - this.m_vecPos[0]));
		this.m_fRotation = flAngle + 6.30 / 4;

		//Enable movement
		this.m_bMove = true;
	}
}
class CSoldierEntity : CBaseSoldierEntity
{
	CSoldierEntity()
	{
		this.SetTeam((g_bTeamToggle) ? SOL_TEAM_1 : SOL_TEAM_2);
	}
	
	string GetName()
	{
		return (this.GetTeam() == SOL_TEAM_1) ? "Combat.Team1" : "Combat.Team2";
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
		string szInfoStr = (g_bTeamToggle) ? "[Soldier] Team 1" : "[Soldier] Team 2";
		R_DrawString(R_GetDefaultFont(), szInfoStr, Vector(g_vecMousePos[0], g_vecMousePos[1]), Color(128, 0, 64, 150));
	}
}

/*
	This function is called when this tool is triggered. The screen position is also passed.
	You can spawn scripted entities here.
*/
void CDG_API_Trigger(const Vector& in vAtPos)
{
	CSoldierEntity @obj = CSoldierEntity();
	Ent_SpawnEntity(@obj, Vector(vAtPos[0], vAtPos[1]));
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
	info.szName = "Soldier";
	info.szAuthor = "Daniel Brendel";
	info.szVersion = "0.2";
	info.szContact = "dbrendel1988<at>gmail<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "sprites\\move\\survivor-move_shotgun_0.png";
	info.szCategory = "Military";
	info.iCursorWidth = 64;
	info.iCursorHeight = 64;
	info.uiTriggerDelay = 250;

	g_szToolPath = szToolPath;
	g_GameKeys = gamekeys;
	
	return true;
}