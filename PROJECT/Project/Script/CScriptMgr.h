#pragma once

#include <vector>
#include <string>

enum class SCRIPT_TYPE
{
	AFTERIMAGESCRIPT,
	ATTACKSCRIPT,
	BANSHEEBULLETSCRIPT,
	BANSHEESCRIPT,
	BATBULLETSCRIPT,
	BOSSBULLETSCRIPT,
	BOSSHANDSCRIPT,
	BOSSHPSCRIPT,
	BOSSLASERSCRIPT,
	BOSSSCRIPT,
	BOSSSWORDSCRIPT,
	CAMERASCRIPT,
	COLLIDERTILESCRIPT,
	DAMAGEUISCRIPT,
	DASHUISCRIPT,
	DONTDESTROYONLOADSCRIPT,
	DOORSCRIPT,
	FADEEFFECTSCRIPT,
	GHOSTSCRIPT,
	GIANTBATSCRIPT,
	GRAVITYSCRIPT,
	HPBARSCRIPT,
	INVENTORYUISCRIPT,
	ITEMHOLDERSCRIPT,
	MINOTAURSATTACKSCRIPT,
	MINOTAURSSCRIPT,
	MONSTERHPBARSCRIPT,
	MONSTERSCRIPT,
	MONSTERSUMMONSCRIPT,
	MOUSESCRIPT,
	OBJECTMANAGERSCRIPT,
	PLAYERINFOUISCRIPT,
	PLAYERSCRIPT,
	SKELARCHERSCRIPT,
	SKELARROWSCRIPT,
	SKELBOWSCRIPT,
	SKELDOGSCRIPT,
	SKELKNIGHTATTACKSCRIPT,
	SKELKNIGHTSCRIPT,
	SKELSCRIPT,
	SKELSWORDSCRIPT,
	SMALLBATSCRIPT,
	STAGESCRIPT,
	TEXTSCRIPT,
	TITLESCRIPT,
	UIMANAGERSCRIPT,
	UNITSCRIPT,
	WARNEFFECTSCRIPT,
	WEAPONSCRIPT,
	WEAPONUISCRIPT,
	END,
};

using namespace std;

class CScript;

class CScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptName(CScript * _pScript);
};