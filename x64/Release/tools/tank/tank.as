/*
	Casual Desktop Game (dnycasualDeskGame) v1.0 developed by Daniel Brendel
	
	(C) 2018 - 2021 by Daniel Brendel
	
	Tool: Tank (developed by Daniel Brendel)
	Version: 0.2
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/*
	API documentation
	*****************

	Basic data types:
	-----------------
	size_t: Can be used as entity iterator (64 Bit)
	HostVersion: A 16-bit value containing a version number (high and low)
	SpriteHandle: A handle to a sprite (64 Bit)
	SoundHandle: A handle to a sound (64 Bit)
	DamageValue: Value containing a damage value. 0 - 255 (1 Byte)
	
	Enums:
	------
	DamageType:
		- DAMAGEABLE_NO: Specifies a non-damageable entity
		- DAMAGEABLE_ALL: Specifies an entity that can be damaged by all
		- DAMAGEABLE_NOTSQUAD: Specifies an entity that cannot be damaged by entities with the same name
	FileSeekWay:
		- SEEKW_BEGIN: Start from the begin of a file
		- SEEKW_CURRENT: Start from current file offset
		- SEEKW_END: Start from the end of a file
		
	Callbacks:
	----------
	//Called for every file object in list
	bool FuncFileListing(const string& in);
	
	Structs:
	--------
	ToolInfo:
		- string szName //The name of the tool
		- string szVersion //The tool version
		- string szAuthor //The author identifier
		- string szContact //Contact information
		- string szPreviewImage //Preview image file relative to the directory of the tool
		- string szCursor //Cursor sprite file relative to the directory of the tool
		- int iCursorWidth //The desired width of the cursor
		- int iCursorHeight //The desired height of the cursor
		- uint32 uiTriggerDelay //Using the tool via the trigger button is available when this time in milliseconds has elapsed
	GameKeys:
		- int vkTrigger //Virtual key code of the trigger mouse button
		- int vkClearn //Virtual key code of the clean mouse button
		- int vkMenu //Virtual key code of the menu button
		- int vkScrollUp //Virtual key code of the mouse button to scroll up (defaulted to wheel up)
		- int vkScrollDown //Virtual key code of the mouse button to scroll down (defaulted to wheel down)
		- int vkTeamSelect //Virtual key code of the team selection button
		- int vkTakeScreen //Virtual key code of the button to take screenshots
		- int vkKey0 - vkKey9 //Virtual key code of a bound tool
		- int vkExit //Virtual key code of the exit button
		
	Classes:
	--------
	Color (Used to define colors for sprites, boxes, strings, etc.):
		- Color() //Default constructor
		- Color(uint8 r, uint8 g, uint8 b, uint8 a) //Construct with default values
		- uint8 R() //Getter for red channel
		- uint8 G() //Getter for green channel
		- uint8 B() //Getter for blue channel
		- uint8 A() //Getter for alpha channel
	ConColor (Used to define colors when printing to the console):
		- ConColor() //Default constructor
		- ConColor(uint8 r, uint8 g, uint8 b) //Construct with default values
		- uint8 R() //Getter for red channel
		- uint8 G() //Getter for green channel
		- uint8 B() //Getter for blue channel
	Vector (Screen positions and resolutions are defined as 2D vectors):
		- Vector() //Default constructor
		- Vector(Vector &in) //Construct from a different Vector
		- Vector(int x, int y) //Construct from x and y coordinates
		- Vector[0/1] //Getter and setter for a specific dimension
		- Vector=Vector //Assign the values of a Vector to a different Vector
		- Vector==Vector //Compare two Vectors of equality
		- Vector[0/1]++ //Increment Vector dimension value
		- Vector[0/1]-- //Decrement Vector dimension value
		- Vector+Vector //Perform addition of two Vectors
		- Vector-Vector //Substract a Vector from another
		- int GetX() const //Get X dimension value
		- int GetY() const //Get Y dimension value
		- int Distance(const Vector &in) //Calculate the distance between two Vectors
		- void Zero() //Fill the Vector with zero
	SpriteInfo (Useful for obtaining information about a sprite before loading it):
		- SpriteInfo() //Default constructor
		- Vector& GetResolution() //Get resolution of the sprite
		- int GetDepth() //Get color depth of the sprite
		- int GetFormat() //Get format of the sprite
	BoundingBox (Used to define the bounding box of a model):
		- BoundingBox() //Default constructor
		- bool Alloc() //Allocate memory needed for using the object instance
		- void AddBBoxItem(const Vector& in, const Vector& in) //Add a bbox item. 
			First Vector is the relative position of the item and the second Vector 
			is the dimensions of the item
		- bool IsCollided(const Vector& in, const Vector& in, const BoundingBox& in) //Check for collision. 
			First Vector is the absolute screen position of the owner entity, second Vector is the absolute screen
			position of the reference entity. Last parameter is the BoundingBox object instance of the reference entity
		- bool IsInside(const Vector &in, const Vector &in) //Check if a Vector is inside the BoundingBox.
			First argument is the absolute position of the owner entity, second argument is the position to be checked.
		- bool IsEmpty() //Returns true if there are no items added to the BoundingBox
		- void Clear() //Remove all added items
	TempSprite (Useful for creating non-specific temporary sprites which do only have a visual relevance):
		- TempSprite() //Default constructor
		- TempSprite(const string &in, uint32 dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)
			//Instantiate with the given data. First argument is the sprite file name relative to the directory of the tool. 
			Second argument defines the delay between each frame change. Third argument defines whether this entity shall last permanently.
			iFrameCount defines the amount of frames in the sprite sheet. iFrameWidth and iFrameHeight are for the dimensions of the sprite. 
			If the sprite sheet has multiple lines then you can define the amount of frames per each line with iFramesPerLine. bForceCustomSize is used to
			determine if the sprite shall be loaded with a custom size instead of the original size
		- bool Initialize(const string &in, uint32 dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)
			//Used if not constructed with the custom initialization constructor. For the arguments see the custom constructor definition above.
		- void Draw(const Vector &in, float fRotation) //Draw the sprite at the given position using the given rotation
		- void Release() //Release the temp sprite. Use this when you don't need the sprite anymore or when the game shuts down
	Model (Useful for associating a model with a scripted entity. Used by the game engine to process collisions):
		- Model() //Default constructor
		- Model(const string &in) //Construct with providing a relative file name to the model file
		- Model(const string &in, bool b) //Same as above, but you can define if a custom size shall be used
		- Model=Model //Basically used to copy the entire data to a new Model object instance
		- bool Initialize(const string&in szMdlFile, bool bForceCustomSize) //Initialize the model. Same as the second custom constructor above.
		- bool Initialize2(const BoundingBox& in, SpriteHandle hSprite) //This is to be used with an already existing sprite file and a Bounding Box.
		- void Release() //Release the Model data. Use this when shutting down or not using the Model anymore
		- bool IsCollided(const Vector& in mypos, const Vector& in refpos, const Model& in mdl) //Check if a reference model collides with this model using the given absolute coordinates
		- bool IsValid() //Returns true if the model has been initialized successfully and can be used
		- bool Alloc() //Allocate memory for the model. Call this before Initialize()/Initialize2()
		- void SetCenter(const Vector &in) //This can be used to define the center of a model
		- SpriteHandle Handle() //Returns the handle to the loaded sprite associated with the model
		- const Vector& GetCenter() const //Getter for the center of the model
		- BoundingBox& GetBBox() //Returns the associated bounding box
	Timer (Useful for processing stuff after a period of time):
		- Timer() //Default constructor
		- Timer(uint32 delay) //Construct with a given delay value. Also sets the timer in active state
		- void Reset() //Resets the internal timing values. Call this when IsElapsed() returns true in order to run the timer again
		- void Update() //Perform internal update calculations
		- void SetActive(bool bStatus) //Set the activation state of the timer
		- void SetDelay(uint32 delay) //Set the delay of the timer
		- bool IsActive() //Indicate activation state
		- uint32 GetDelay() //Get delay value
		- bool IsElapsed() //Indicates whether the time has elapsed
	FileReader (can be used to read from files, e.g. custom configs):
		- FileReader() //Default constructor
		- FileReader(const string& in) //Construct object and open the given file
			for reading. Note that you can only read files relative to the directory of your tool
		- bool Open(const string &in) //Open a file for reading. 
		- bool IsOpen() //Indicate whether the file has been successfully opened 
		- bool Eof() //Returns true if the internal file pointer has reached the end of the file
		- void Seek(FileSeekWay from, int offset) //Jump "offset" bytes from the seek way inside the file
		- string GetLine() //Get line at current file pointer
		- string GetEntireContent(bool bSkipNLChar = false) //Read the entire file. The boolean argument
			specifies whether no newline char shall be appended to each read line
		- void Close() //Closes the file
	FileWriter (can be used to write to files):
		- FileWriter() //Default constructor
		- FileWriter(const string& in) //Construct and opens a file for writing. Using the constructor it defaults to append to the file
		- bool Open(const string &in, bool bAppend = true)
			//Opens a file for writing. The boolean parameter can be used to define whether to append to the file 
			if it already exists. Otherwise the file is overwritten.
		- bool Eof() //Returns true if the internal file pointer has reached the end of the file
		- void Seek(FileSeekWay from, int offset) //Jump "offset" bytes from the seek way inside the file
		- void Write(const string &in) //Write a string to the file
		- void WriteLine(const string &in) //Write a line to the file
		- void Close() //Close the file
	
	Interface:
	----------
		IScriptedEntity:
			- Used to implement entities related to the tool
			- See example in this demo script file
			//Default constructor
			IScriptedEntity()
			//Called when the entity gets spawned. The position on the screen is passed as argument
			void OnSpawn(const Vector& in vec)
			//Called when the entity gets released
			void OnRelease()
			//Process entity stuff
			void OnProcess()
			//Entity can draw everything in default order here
			void OnDraw()
			//Entity can draw on-top stuff here
			void OnDrawOnTop()
			//Indicate whether the user is allowed to clean this entity
			bool DoUserCleaning()
			//Indicate whether this entity shall be removed by the game
			bool NeedsRemoval()
			//Indicate whether this entity is damageable. Damageable entities can collide with other
			//entities (even with entities from other tools) and recieve and strike damage. 
			//0 = not damageable, 1 = damage all, 2 = not damaging entities with same name
			DamageType IsDamageable()
			//Called when the entity recieves damage
			void OnDamage(DamageValue dv)
			//Called for recieving the model data for this entity. This is only used for
			//damageable entities. 
			Model& GetModel()
			//Called for recieving the current position. This is useful if the entity shall move.
			Vector& GetPosition()
			//Return the rotation. This is actually not used by the host application, but might be useful to other entities
			float GetRotation()
			//Called for querying the damage value for this entity
			DamageValue GetDamageValue()
			//Return a name string here, e.g. the class name or instance name. This is used when DAMAGE_NOTSQUAD is defined as damage-type, but can also be useful to other entities
			//Indicate if this entity is movable
			bool IsMovable()
			//This vector is used for drawing the selection box
			Vector& GetSelectionSize()
			//This method is used to set the movement destination position
			void MoveTo(const Vector& in vec)

	Available API functions:
	------------------------
	//Print a text to the console
	void Print(const string& in)
	//Print text to the console with the given color
	void PrintClr(const string& in, const ConColor &in)
	//Set an offset to the cursor in order to draw it on a different position
	void SetCursorOffset(const Vector& in)
	//Set the cursor rotation
	void SetCursorRotation(float fRotation)
	//Load a font
	FontHandle R_LoadFont(const string& in, uint8 ucFontSizeW, uint8 ucFontSizeH)
	//Get information about a sprite file. Path is relative to the directory of the tool
	bool R_GetSpriteInfo(const string &in, SpriteInfo &out)
	//Load a sprite into memory. First argument is the sprite file name relative to the directory of the tool. 
			iFrameCount defines the amount of frames in the sprite sheet. iFrameWidth and iFrameHeight are for the dimensions of the sprite. 
			If the sprite sheet has multiple lines then you can define the amount of frames per each line with iFramesPerLine. bForceCustomSize is used to
			determine if the sprite shall be loaded with a custom size instead of the original size.
			The szFile path is relative to the directory of the tool
	SpriteHandle R_LoadSprite(const string& in szFile, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, bool bForceCustomSize)
	//Release a sprite if not used anymore
	bool R_FreeSprite(SpriteHandle hSprite)
	//Draw a box on the screen. It is not filled. You can define the line thickness of the box with iThickness.
	bool R_DrawBox(const Vector& in pos, const Vector&in size, int iThickness, const Color&in color)
	//Draw a filled box on the screen.
	bool R_DrawFilledBox(const Vector&in pos, const Vector&in size, const Color&in color)
	//Draw a line on the screen.
	bool R_DrawLine(const Vector&in start, const Vector&in end, const Color&in color)
	//Draw a loaded sprite on the screen. iFrame specifies the frame of the sprite sheet if the sprite is loaded from a sheet.
		You can define a vertical and horizontal scaling value. Also you can define a rotation vector which is used for
		rotating the sprite from. Also you can define a custom color which the sprite shall be rendered with.
	bool R_DrawSprite(const SpriteHandle hSprite, const Vector&in pos, int iFrame, float fRotation, const Vector &in vRotPos, float fScale1, float fScale2, bool bUseCustomColorMask, const Color&in color)
	//Draw a string on the screen.
	bool R_DrawString(const FontHandle font, const string&in szText, const Vector&in pos, const Color&in color)
	//Get the handle to the default loaded game engine font
	FontHandle R_GetDefaultFont()
	//Query a sound file located on the disk. Path is relative to the directory of the tool
	SoundHandle S_QuerySound(const string&in szSoundFile)
	//Play a sound with the given volume (1-10)
	bool S_PlaySound(SoundHandle hSound, int32 lVolume)
	//Get the center of the width of the screen
	int Wnd_GetWindowCenterX()
	//Get the center of the height of the screen
	int Wnd_GetWindowCenterY()
	//Spawn a scripted entity. Every IScriptedEntity class instance must be registered with the game engine using this method
	bool Ent_SpawnEntity(IScriptedEntity @obj, const Vector& in)
	//Get the amount of all existing entities. This includes entities from other tools
	size_t Ent_GetEntityCount()
	//Get a handle to a given entity using it's ID. You can also obtain entity handles of entities from other tools
	IScriptedEntity@+ Ent_GetEntityHandle(size_t uiEntityId)
	//Perform a trace line calculation from one point to another. The first found entity inside the trace line is returned.
		You can specify an entity that shall be skipped by the search
	IScriptedEntity@+ Ent_TraceLine(const Vector&in vStart, const Vector&in vEnd, IScriptedEntity@+ pIgnoredEnt)
	//Check if an entity is still valid. This is useful for validating if an entity has not yet been disposed
	bool Ent_IsValid(IScriptedEntity@ pEntity)
	//Get the ID of an entity by the entity handle
	size_t Ent_GetId(IScriptedEntity@ pEntity)
	//List all sprites of a directory relative to the directory of the tool.
	bool Util_ListSprites(const string& in, FuncFileListing @cb)
	//List all sounds of a directory relative to the directory of the tool
	bool Util_ListSounds(const string& in, FuncFileListing @cb)
	//Return a random number between the given values
	int Util_Random(int start, int end)
	
	AngelScript internals:
	----------------------
		You can use all things from
		- AngelScript std string
		- AngelScript script array
		- AngelScript script math
		
	In order to learn how AngelScript generally
	works visit the official AngelScript website.
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
	info.szContact = "dbrendel1988<at>gmail<dot>com";
	info.szPreviewImage = "preview.png";
	info.szCursor = "cursor.png";
	info.szCategory = "Military";
	info.iCursorWidth = 32;
	info.iCursorHeight = 32;
	info.uiTriggerDelay = 125;
	
	g_szToolPath = szToolPath;

	return true;
}