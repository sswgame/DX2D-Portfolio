#include "pch.h"
#include "CScriptMgr.h"

#include "CAfterImageScript.h"
#include "CAttackScript.h"
#include "CBansheeBulletScript.h"
#include "CBansheeScript.h"
#include "CBatBulletScript.h"
#include "CBossBulletScript.h"
#include "CBossHandScript.h"
#include "CBossHPScript.h"
#include "CBossLaserScript.h"
#include "CBossScript.h"
#include "CBossSwordScript.h"
#include "CCameraScript.h"
#include "CColliderTileScript.h"
#include "CDamageUIScript.h"
#include "CDashUIScript.h"
#include "CDontDestroyOnLoadScript.h"
#include "CDoorScript.h"
#include "CFadeEffectScript.h"
#include "CGhostScript.h"
#include "CGiantBatScript.h"
#include "CGravityScript.h"
#include "CHPBarScript.h"
#include "CInventoryUIScript.h"
#include "CItemHolderScript.h"
#include "CMinotaursAttackScript.h"
#include "CMinotaursScript.h"
#include "CMonsterHPBarScript.h"
#include "CMonsterScript.h"
#include "CMonsterSummonScript.h"
#include "CMouseScript.h"
#include "CObjectManagerScript.h"
#include "CPlayerInfoUIScript.h"
#include "CPlayerScript.h"
#include "CSkelArcherScript.h"
#include "CSkelArrowScript.h"
#include "CSkelBowScript.h"
#include "CSkelDogScript.h"
#include "CSkelKnightAttackScript.h"
#include "CSkelKnightScript.h"
#include "CSkelScript.h"
#include "CSkelSwordScript.h"
#include "CSmallBatScript.h"
#include "CStageScript.h"
#include "CTextScript.h"
#include "CTitleScript.h"
#include "CUIManagerScript.h"
#include "CUnitScript.h"
#include "CWarnEffectScript.h"
#include "CWeaponScript.h"
#include "CWeaponUIScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CAfterImageScript");
	_vec.push_back(L"CAttackScript");
	_vec.push_back(L"CBansheeBulletScript");
	_vec.push_back(L"CBansheeScript");
	_vec.push_back(L"CBatBulletScript");
	_vec.push_back(L"CBossBulletScript");
	_vec.push_back(L"CBossHandScript");
	_vec.push_back(L"CBossHPScript");
	_vec.push_back(L"CBossLaserScript");
	_vec.push_back(L"CBossScript");
	_vec.push_back(L"CBossSwordScript");
	_vec.push_back(L"CCameraScript");
	_vec.push_back(L"CColliderTileScript");
	_vec.push_back(L"CDamageUIScript");
	_vec.push_back(L"CDashUIScript");
	_vec.push_back(L"CDontDestroyOnLoadScript");
	_vec.push_back(L"CDoorScript");
	_vec.push_back(L"CFadeEffectScript");
	_vec.push_back(L"CGhostScript");
	_vec.push_back(L"CGiantBatScript");
	_vec.push_back(L"CGravityScript");
	_vec.push_back(L"CHPBarScript");
	_vec.push_back(L"CInventoryUIScript");
	_vec.push_back(L"CItemHolderScript");
	_vec.push_back(L"CMinotaursAttackScript");
	_vec.push_back(L"CMinotaursScript");
	_vec.push_back(L"CMonsterHPBarScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CMonsterSummonScript");
	_vec.push_back(L"CMouseScript");
	_vec.push_back(L"CObjectManagerScript");
	_vec.push_back(L"CPlayerInfoUIScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CSkelArcherScript");
	_vec.push_back(L"CSkelArrowScript");
	_vec.push_back(L"CSkelBowScript");
	_vec.push_back(L"CSkelDogScript");
	_vec.push_back(L"CSkelKnightAttackScript");
	_vec.push_back(L"CSkelKnightScript");
	_vec.push_back(L"CSkelScript");
	_vec.push_back(L"CSkelSwordScript");
	_vec.push_back(L"CSmallBatScript");
	_vec.push_back(L"CStageScript");
	_vec.push_back(L"CTextScript");
	_vec.push_back(L"CTitleScript");
	_vec.push_back(L"CUIManagerScript");
	_vec.push_back(L"CUnitScript");
	_vec.push_back(L"CWarnEffectScript");
	_vec.push_back(L"CWeaponScript");
	_vec.push_back(L"CWeaponUIScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CAfterImageScript" == _strScriptName)
		return new CAfterImageScript;
	if (L"CAttackScript" == _strScriptName)
		return new CAttackScript;
	if (L"CBansheeBulletScript" == _strScriptName)
		return new CBansheeBulletScript;
	if (L"CBansheeScript" == _strScriptName)
		return new CBansheeScript;
	if (L"CBatBulletScript" == _strScriptName)
		return new CBatBulletScript;
	if (L"CBossBulletScript" == _strScriptName)
		return new CBossBulletScript;
	if (L"CBossHandScript" == _strScriptName)
		return new CBossHandScript;
	if (L"CBossHPScript" == _strScriptName)
		return new CBossHPScript;
	if (L"CBossLaserScript" == _strScriptName)
		return new CBossLaserScript;
	if (L"CBossScript" == _strScriptName)
		return new CBossScript;
	if (L"CBossSwordScript" == _strScriptName)
		return new CBossSwordScript;
	if (L"CCameraScript" == _strScriptName)
		return new CCameraScript;
	if (L"CColliderTileScript" == _strScriptName)
		return new CColliderTileScript;
	if (L"CDamageUIScript" == _strScriptName)
		return new CDamageUIScript;
	if (L"CDashUIScript" == _strScriptName)
		return new CDashUIScript;
	if (L"CDontDestroyOnLoadScript" == _strScriptName)
		return new CDontDestroyOnLoadScript;
	if (L"CDoorScript" == _strScriptName)
		return new CDoorScript;
	if (L"CFadeEffectScript" == _strScriptName)
		return new CFadeEffectScript;
	if (L"CGhostScript" == _strScriptName)
		return new CGhostScript;
	if (L"CGiantBatScript" == _strScriptName)
		return new CGiantBatScript;
	if (L"CGravityScript" == _strScriptName)
		return new CGravityScript;
	if (L"CHPBarScript" == _strScriptName)
		return new CHPBarScript;
	if (L"CInventoryUIScript" == _strScriptName)
		return new CInventoryUIScript;
	if (L"CItemHolderScript" == _strScriptName)
		return new CItemHolderScript;
	if (L"CMinotaursAttackScript" == _strScriptName)
		return new CMinotaursAttackScript;
	if (L"CMinotaursScript" == _strScriptName)
		return new CMinotaursScript;
	if (L"CMonsterHPBarScript" == _strScriptName)
		return new CMonsterHPBarScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CMonsterSummonScript" == _strScriptName)
		return new CMonsterSummonScript;
	if (L"CMouseScript" == _strScriptName)
		return new CMouseScript;
	if (L"CObjectManagerScript" == _strScriptName)
		return new CObjectManagerScript;
	if (L"CPlayerInfoUIScript" == _strScriptName)
		return new CPlayerInfoUIScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CSkelArcherScript" == _strScriptName)
		return new CSkelArcherScript;
	if (L"CSkelArrowScript" == _strScriptName)
		return new CSkelArrowScript;
	if (L"CSkelBowScript" == _strScriptName)
		return new CSkelBowScript;
	if (L"CSkelDogScript" == _strScriptName)
		return new CSkelDogScript;
	if (L"CSkelKnightAttackScript" == _strScriptName)
		return new CSkelKnightAttackScript;
	if (L"CSkelKnightScript" == _strScriptName)
		return new CSkelKnightScript;
	if (L"CSkelScript" == _strScriptName)
		return new CSkelScript;
	if (L"CSkelSwordScript" == _strScriptName)
		return new CSkelSwordScript;
	if (L"CSmallBatScript" == _strScriptName)
		return new CSmallBatScript;
	if (L"CStageScript" == _strScriptName)
		return new CStageScript;
	if (L"CTextScript" == _strScriptName)
		return new CTextScript;
	if (L"CTitleScript" == _strScriptName)
		return new CTitleScript;
	if (L"CUIManagerScript" == _strScriptName)
		return new CUIManagerScript;
	if (L"CUnitScript" == _strScriptName)
		return new CUnitScript;
	if (L"CWarnEffectScript" == _strScriptName)
		return new CWarnEffectScript;
	if (L"CWeaponScript" == _strScriptName)
		return new CWeaponScript;
	if (L"CWeaponUIScript" == _strScriptName)
		return new CWeaponUIScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::AFTERIMAGESCRIPT:
		return new CAfterImageScript;
		break;
	case (UINT)SCRIPT_TYPE::ATTACKSCRIPT:
		return new CAttackScript;
		break;
	case (UINT)SCRIPT_TYPE::BANSHEEBULLETSCRIPT:
		return new CBansheeBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::BANSHEESCRIPT:
		return new CBansheeScript;
		break;
	case (UINT)SCRIPT_TYPE::BATBULLETSCRIPT:
		return new CBatBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSBULLETSCRIPT:
		return new CBossBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSHANDSCRIPT:
		return new CBossHandScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSHPSCRIPT:
		return new CBossHPScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSLASERSCRIPT:
		return new CBossLaserScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSSCRIPT:
		return new CBossScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSSWORDSCRIPT:
		return new CBossSwordScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERASCRIPT:
		return new CCameraScript;
		break;
	case (UINT)SCRIPT_TYPE::COLLIDERTILESCRIPT:
		return new CColliderTileScript;
		break;
	case (UINT)SCRIPT_TYPE::DAMAGEUISCRIPT:
		return new CDamageUIScript;
		break;
	case (UINT)SCRIPT_TYPE::DASHUISCRIPT:
		return new CDashUIScript;
		break;
	case (UINT)SCRIPT_TYPE::DONTDESTROYONLOADSCRIPT:
		return new CDontDestroyOnLoadScript;
		break;
	case (UINT)SCRIPT_TYPE::DOORSCRIPT:
		return new CDoorScript;
		break;
	case (UINT)SCRIPT_TYPE::FADEEFFECTSCRIPT:
		return new CFadeEffectScript;
		break;
	case (UINT)SCRIPT_TYPE::GHOSTSCRIPT:
		return new CGhostScript;
		break;
	case (UINT)SCRIPT_TYPE::GIANTBATSCRIPT:
		return new CGiantBatScript;
		break;
	case (UINT)SCRIPT_TYPE::GRAVITYSCRIPT:
		return new CGravityScript;
		break;
	case (UINT)SCRIPT_TYPE::HPBARSCRIPT:
		return new CHPBarScript;
		break;
	case (UINT)SCRIPT_TYPE::INVENTORYUISCRIPT:
		return new CInventoryUIScript;
		break;
	case (UINT)SCRIPT_TYPE::ITEMHOLDERSCRIPT:
		return new CItemHolderScript;
		break;
	case (UINT)SCRIPT_TYPE::MINOTAURSATTACKSCRIPT:
		return new CMinotaursAttackScript;
		break;
	case (UINT)SCRIPT_TYPE::MINOTAURSSCRIPT:
		return new CMinotaursScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERHPBARSCRIPT:
		return new CMonsterHPBarScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSUMMONSCRIPT:
		return new CMonsterSummonScript;
		break;
	case (UINT)SCRIPT_TYPE::MOUSESCRIPT:
		return new CMouseScript;
		break;
	case (UINT)SCRIPT_TYPE::OBJECTMANAGERSCRIPT:
		return new CObjectManagerScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERINFOUISCRIPT:
		return new CPlayerInfoUIScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELARCHERSCRIPT:
		return new CSkelArcherScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELARROWSCRIPT:
		return new CSkelArrowScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELBOWSCRIPT:
		return new CSkelBowScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELDOGSCRIPT:
		return new CSkelDogScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELKNIGHTATTACKSCRIPT:
		return new CSkelKnightAttackScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELKNIGHTSCRIPT:
		return new CSkelKnightScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELSCRIPT:
		return new CSkelScript;
		break;
	case (UINT)SCRIPT_TYPE::SKELSWORDSCRIPT:
		return new CSkelSwordScript;
		break;
	case (UINT)SCRIPT_TYPE::SMALLBATSCRIPT:
		return new CSmallBatScript;
		break;
	case (UINT)SCRIPT_TYPE::STAGESCRIPT:
		return new CStageScript;
		break;
	case (UINT)SCRIPT_TYPE::TEXTSCRIPT:
		return new CTextScript;
		break;
	case (UINT)SCRIPT_TYPE::TITLESCRIPT:
		return new CTitleScript;
		break;
	case (UINT)SCRIPT_TYPE::UIMANAGERSCRIPT:
		return new CUIManagerScript;
		break;
	case (UINT)SCRIPT_TYPE::UNITSCRIPT:
		return new CUnitScript;
		break;
	case (UINT)SCRIPT_TYPE::WARNEFFECTSCRIPT:
		return new CWarnEffectScript;
		break;
	case (UINT)SCRIPT_TYPE::WEAPONSCRIPT:
		return new CWeaponScript;
		break;
	case (UINT)SCRIPT_TYPE::WEAPONUISCRIPT:
		return new CWeaponUIScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::AFTERIMAGESCRIPT:
		return L"CAfterImageScript";
		break;

	case SCRIPT_TYPE::ATTACKSCRIPT:
		return L"CAttackScript";
		break;

	case SCRIPT_TYPE::BANSHEEBULLETSCRIPT:
		return L"CBansheeBulletScript";
		break;

	case SCRIPT_TYPE::BANSHEESCRIPT:
		return L"CBansheeScript";
		break;

	case SCRIPT_TYPE::BATBULLETSCRIPT:
		return L"CBatBulletScript";
		break;

	case SCRIPT_TYPE::BOSSBULLETSCRIPT:
		return L"CBossBulletScript";
		break;

	case SCRIPT_TYPE::BOSSHANDSCRIPT:
		return L"CBossHandScript";
		break;

	case SCRIPT_TYPE::BOSSHPSCRIPT:
		return L"CBossHPScript";
		break;

	case SCRIPT_TYPE::BOSSLASERSCRIPT:
		return L"CBossLaserScript";
		break;

	case SCRIPT_TYPE::BOSSSCRIPT:
		return L"CBossScript";
		break;

	case SCRIPT_TYPE::BOSSSWORDSCRIPT:
		return L"CBossSwordScript";
		break;

	case SCRIPT_TYPE::CAMERASCRIPT:
		return L"CCameraScript";
		break;

	case SCRIPT_TYPE::COLLIDERTILESCRIPT:
		return L"CColliderTileScript";
		break;

	case SCRIPT_TYPE::DAMAGEUISCRIPT:
		return L"CDamageUIScript";
		break;

	case SCRIPT_TYPE::DASHUISCRIPT:
		return L"CDashUIScript";
		break;

	case SCRIPT_TYPE::DONTDESTROYONLOADSCRIPT:
		return L"CDontDestroyOnLoadScript";
		break;

	case SCRIPT_TYPE::DOORSCRIPT:
		return L"CDoorScript";
		break;

	case SCRIPT_TYPE::FADEEFFECTSCRIPT:
		return L"CFadeEffectScript";
		break;

	case SCRIPT_TYPE::GHOSTSCRIPT:
		return L"CGhostScript";
		break;

	case SCRIPT_TYPE::GIANTBATSCRIPT:
		return L"CGiantBatScript";
		break;

	case SCRIPT_TYPE::GRAVITYSCRIPT:
		return L"CGravityScript";
		break;

	case SCRIPT_TYPE::HPBARSCRIPT:
		return L"CHPBarScript";
		break;

	case SCRIPT_TYPE::INVENTORYUISCRIPT:
		return L"CInventoryUIScript";
		break;

	case SCRIPT_TYPE::ITEMHOLDERSCRIPT:
		return L"CItemHolderScript";
		break;

	case SCRIPT_TYPE::MINOTAURSATTACKSCRIPT:
		return L"CMinotaursAttackScript";
		break;

	case SCRIPT_TYPE::MINOTAURSSCRIPT:
		return L"CMinotaursScript";
		break;

	case SCRIPT_TYPE::MONSTERHPBARSCRIPT:
		return L"CMonsterHPBarScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::MONSTERSUMMONSCRIPT:
		return L"CMonsterSummonScript";
		break;

	case SCRIPT_TYPE::MOUSESCRIPT:
		return L"CMouseScript";
		break;

	case SCRIPT_TYPE::OBJECTMANAGERSCRIPT:
		return L"CObjectManagerScript";
		break;

	case SCRIPT_TYPE::PLAYERINFOUISCRIPT:
		return L"CPlayerInfoUIScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::SKELARCHERSCRIPT:
		return L"CSkelArcherScript";
		break;

	case SCRIPT_TYPE::SKELARROWSCRIPT:
		return L"CSkelArrowScript";
		break;

	case SCRIPT_TYPE::SKELBOWSCRIPT:
		return L"CSkelBowScript";
		break;

	case SCRIPT_TYPE::SKELDOGSCRIPT:
		return L"CSkelDogScript";
		break;

	case SCRIPT_TYPE::SKELKNIGHTATTACKSCRIPT:
		return L"CSkelKnightAttackScript";
		break;

	case SCRIPT_TYPE::SKELKNIGHTSCRIPT:
		return L"CSkelKnightScript";
		break;

	case SCRIPT_TYPE::SKELSCRIPT:
		return L"CSkelScript";
		break;

	case SCRIPT_TYPE::SKELSWORDSCRIPT:
		return L"CSkelSwordScript";
		break;

	case SCRIPT_TYPE::SMALLBATSCRIPT:
		return L"CSmallBatScript";
		break;

	case SCRIPT_TYPE::STAGESCRIPT:
		return L"CStageScript";
		break;

	case SCRIPT_TYPE::TEXTSCRIPT:
		return L"CTextScript";
		break;

	case SCRIPT_TYPE::TITLESCRIPT:
		return L"CTitleScript";
		break;

	case SCRIPT_TYPE::UIMANAGERSCRIPT:
		return L"CUIManagerScript";
		break;

	case SCRIPT_TYPE::UNITSCRIPT:
		return L"CUnitScript";
		break;

	case SCRIPT_TYPE::WARNEFFECTSCRIPT:
		return L"CWarnEffectScript";
		break;

	case SCRIPT_TYPE::WEAPONSCRIPT:
		return L"CWeaponScript";
		break;

	case SCRIPT_TYPE::WEAPONUISCRIPT:
		return L"CWeaponUIScript";
		break;

	}
	return nullptr;
}