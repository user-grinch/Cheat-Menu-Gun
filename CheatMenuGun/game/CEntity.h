#pragma once

class CEntityList
{
public:
	class CEntity *Player; //0x0000

	static CEntity* FindPlayer()
	{
		return reinterpret_cast<CEntityList*>((int)GetModuleHandle(NULL) + 0x2C965C)->Player;
	}
}; //Size: 0x0004
static_assert(sizeof(CEntityList) == 0x4);

class CEntity
{
public:
	char pad_0000[48]; //0x0000
	float m_fHealth; //0x0030
	float m_fHealth2; //0x0034
	char pad_0038[248]; //0x0038
	uint32_t m_nPistolMag; //0x0130
	char pad_0134[8]; //0x0134
	uint32_t m_nAmmo; //0x013C
	char pad_0140[20]; //0x0140
	uint32_t m_nRifleMag; //0x0154
	char pad_0158[4]; //0x0158
	uint32_t m_nSniperMag; //0x015C
	uint32_t m_nBowMag; //0x0160
	uint32_t m_nDynamyteMag; //0x0164
	char pad_0168[392]; //0x0168
}; //Size: 0x02F0
static_assert(sizeof(CEntity) == 0x2F0);
