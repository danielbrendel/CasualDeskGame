Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel
==============================================================

(C) 2018 - 2019 by Daniel Brendel

Version: 0.8
Contact: Daniel Brendel<at>gmail<dot>com
GitHub: https://github.com/dny-coder
Licence: Creative Commons Attribution-NoDerivatives 4.0 International

This program is a casual game with the purpose to pass time via spawning and 
handling custom created entities on your Desktop. Therefore you will be provided 
with a set of tools. As a developer you can also create your own tools using 
AngelScript. Available tools are selected via the ingame menu. New tools can be 
viewed and downloaded via the ingame menu, too. You can also view/upload taken 
screenshots during gameplay (uploaded screenshots are available via the official 
game website).

Keys:
*****
Use: Left mouse button (Use the selected tool)
Clean: Right mouse button (Clean your "work")
Menu: TAB (Handle tools, Workshop items, screenshots and view news)
Team: Space (Change tool team if applicable)
0-9: Select bound tool
Screenshot: F10 (Saves a screenshot to disk)
Console: F11 (A handy tool for developers, e.g., to view scripting errors)
Exit: ESC (Exits the program)

Tools:
******
The following basic steps are required to create a tool:
- Create a new folder inside the tool directory and name it according
  your tool name.
- Grab the demo_sdk.as script file, copy it inside the directory and
  give it the same file name as the directory is named.
- Create a preview image (size: 195x90) and a cursor image and place
  them somewhere in the directory
- Edit the tool script file according the provided instructions inside
  the script file
- When debugging, use the ingame console to view output

Changelog:
**********
# Version 0.1:
	+ (Initial release)
# Version 0.2:
	+ Tools are now grouped in categories
# Version 0.3:
	+ Category "All" in order to access all tools
	+ Added tool API functions 'Ent_IsValid' and 'Ent_GetId'
	+ Added new tools 'tc_light', 'tc_heavy' and 'tc_dualgun'
	+ Fixed menu tool section scrolling category indexing bug
# Version 0.4:
	+ Added new tool 'lasermech'
	+ Fixed drawing of team in above left corner of tc tanks
	+ Adjusted tc tanks and teslatower attack range
# Version 0.5:
	+ Updated tc tanks, tesla tower and laser mech for team combat
	+ Increased health of tesla tower
# Version 0.6:
	+ Fixed explosion of certain tools in order to decrease FPS drops
# Version 0.7:
	+ Added 'opAssign' to Model
	+ Added 'GetBBox' to Model
	+ Added 'IsInside()' to BoundingBox
	+ Added new tool 'aircraft'
	+ Added new tool 'gunner'
	+ Fixed explosion position of bomb tool
# Version 0.8:
	+ Fixed decal not drawing of bomb tool
	+ Added new tool 'lasergun'
	+ Added new tool 'hhg'
	+ Added new tool 'soldier'
	+ Added new tool 'fireworks'
	+ Added new tool 'bazooka'
	+ Added new tool 'chainsaw'
	+ Added new tool 'wand'
	+ Fixed scrollbar buttons
	+ Fixed scroll bar cube position
	+ Added teamselect key
	+ Added Steam integration
	+ Fixed bug that empty tool could be selected
	+ Fixed performance issues
	+ Fixed scrollbar showing if no further content
	+ Fixed static key usage of team toggle in team scripts
	+ Headcrabs now teleport to opposite instead of getting removed
	+ Fixed Lasermech shooting at nuke
	+ Switched some default keys
	+ Fixed drawing of sprite trail of Fireworks tool
	+ Changed explosion sounds of tc_dualgun and tc_heavy
	+ New sound for aircraft and fixed volume
	+ Crosshair for mp5 and colorgun
	+ Steam Workshop integration
	+ Added volume control
	+ Added tool selection key bindings