#ifndef HUD_DEATH_NOTICE_H
#define HUD_DEATH_NOTICE_H
#include "base.h"

enum KillRarity
{
	KILLRARITY_HEADSHOT = 0x001, // The killer player kills the victim with a headshot
	KILLRARITY_NOSCOPE = 0x002, // The killer player kills the victim with a sniper rifle with no scope
	KILLRARITY_PENETRATED = 0x004, // The killer player kills the victim through walls
	KILLRARITY_INAIR = 0x008
};

class CHudDeathNotice : public CHudElemBase<CHudDeathNotice>
{
public:
	void Init();
	void InitHudData();
	void VidInit();
	void Draw(float flTime);
	void Think();
	int MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf);

private:
	int m_HUD_d_skull; // sprite index of skull icon
	int m_HUD_d_headshot;
	int m_HUD_d_noscope;
};

#endif
