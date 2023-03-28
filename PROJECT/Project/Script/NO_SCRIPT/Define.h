#pragma once

#define DEFINE_NAME(className) (L#className)

enum class COSTUME_TYPE
{
	BASIC,
	ALICE,
	METAL,
	PIKAX,
	RED_ROTUS,

	END
};

enum class ACTION_TYPE
{
	MOVE_UP,        // ���� �̵�
	MOVE_DOWN,      // �Ʒ��� �̵�
	MOVE_LEFT,      // �������� �̵�
	MOVE_RIGHT,     // ���������� �̵�
	JUMP,           // ����
	ATTACK,         // ���� 
	MAIN_SKILL,     // ��ų
	SUB_SKILL,      // ������ų
	DASH,           // �뽬
	RELOAD,         // ������
	INVENTORY,      // ĳ���� �κ��丮
	MAP,            // ���� ����
	STAT,           // ĳ���� �ɷ�ġ
	INTERACTION,    // ��ȣ�ۿ�
	CHANGE_WEAPON,  // ���� ��ü
	END
};

enum class UNIT_DIRECTION
{
	LEFT,
	RIGHT
};

enum class TILE_TYPE
{
	BLOCK,
	HALF_BLOCK
};

enum class COLLISION_SIDE
{
	TOP,
	OTHER,
	NONE
};

enum class UNIT_STATE
{
	IDLE,
	MOVE,
	JUMP,
	DEAD,
	DASH,
	ATTACK,
	SKILL,
	END
};

enum class MONSTER_TYPE
{
	SKELETON,
	SKELETON_KNIGHT,
	SKELETON_ARCHER,
	SKELETON_DOG,
	BANSHEE,
	GHOST,
	GIANT_BAT,
	SMALL_BAT,
	MINOTAURS,
	BOSS,
	NONE,
	END
};

enum class BAT_TYPE
{
	NORMAL,
	RED
};

enum class UNIT_HP
{
	PLAYRER = 100,
	SKELETON = 50,
	SKELETON_KNIGHT=80,
	SKELETON_ARCHER = 70,
	SKELETON_DOG=40,
	BANSHEE = 120,
	GIANT_BAT = 150,
	SMALL_BAT=50,
	GHOST=20,
	MINOTAURS=200,
	BOSS = 500,
	END
};

enum class PARTICLE_DIR
{
	FROM_LEFT,
	FROM_RIGHT,
	FROM_BOTTOM,
	FROM_TOP
};

inline constexpr int DEFAULT_LAYER        = 0;
inline constexpr int TILE_LAYER           = 1;
inline constexpr int PLAYER_LAYER         = 2;
inline constexpr int PLAYER_ATTACK_LAYER  = 3;
inline constexpr int MONSTER_LAYER        = 4;
inline constexpr int MONSTER_ATTACK_LAYER = 5;
inline constexpr int ENTER_SCENE_LAYER    = 6;
inline constexpr int MONSTER_SPAWN_LAYER  = 7;

inline constexpr int UI_LAYER = 31;

///////
//SOUND
///////
