//
// Created by den4ik on 15.12.25.
//
#include "cbase.h"

#include "player.h"
#include "gamerules.h"
#include "util.h"
#include "edict.h"
#include "convar.h"
#include "utlvector.h"
#include "hazard_zone.h"
extern DLL_GLOBAL CGameRules *g_pGameRules;
extern ConVar sv_hazardz;
char last_alive[256];
extern DLL_GLOBAL BOOL g_fGameOver;
#define WARMUP_TIME 15.0f
ConVar mp_warmuptime("mp_warmuptime", "20", FCVAR_SERVER);

float g_flWarmupStartTime = 0.0f;



bool IsHazardZ() {
	return sv_hazardz.GetBool();
}
CHazardZone::CHazardZone() {
	StartWarmup();
}
CHazardZone::~CHazardZone()
{
}

void CHazardZone::StartWarmup()
{
	g_iGameState = GAME_WARMUP;
	m_flWarmupEndTime = gpGlobals->time + mp_warmuptime.GetFloat();
	g_flWarmupStartTime = gpGlobals->time;

	UTIL_ClientPrintAll(HUD_PRINTCENTER, "WARMUP STARTED\n");
}
void CHazardZone::EndWarmup()
{
	g_iGameState = GAME_LIVE;

	UTIL_ClientPrintAll(HUD_PRINTCENTER, "MATCH STARTED!\n");
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);
		if (pPlayer)
		{
			pPlayer->Spawn();
			pPlayer->GiveNamedItem("weapon_crowbar");
		}
	}
	hazzone = CBaseEntity::Create("hazard_zone", Vector(0, 0, 0), Vector(0, 0, 0), NULL);
}

void CHazardZone::PlayerSpawn(CBasePlayer *pPlayer)
{
	pPlayer->pev->weapons |= (1 << WEAPON_SUIT);

}
void CHazardZone::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) {
	// TODO: move player to observers like in regamedll_cs
	CheckWinConditions();
	CHalfLifeMultiplay::PlayerKilled(pVictim, pKiller, pInflictor);
	pVictim->StartObserver();
}

void CHazardZone::DelayedMVP(float tmDelay) {
	// TODO -- wait a delay
	char sentence[2048] = "%s won the game!";
	CUtlVector<CBasePlayer *> candidates;
	char buff[512];
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);
		if (pPlayer != nullptr) {
			if (!UTIL_IsValidEntity(pPlayer->edict()))
				continue;

			if (!pPlayer->IsAlive())
				continue;

			candidates.AddToTail(pPlayer);
		}
	}
	// assuming br winner is only who alive now
	if (!candidates.IsEmpty())
	{
		const char *pMVP = STRING(candidates[0]->pev->netname);
		sprintf( buff, sentence, pMVP);
	}
	else
	{
		sprintf(buff, sentence, "NOBODY");
	}
	UTIL_ClientPrintAll(HUD_PRINTCENTER, buff);
	UTIL_ClientPrintAll(HUD_PRINTCONSOLE, buff);
	UTIL_LogPrintf( sentence);
}
void CHazardZone::GoToIntermission(void)
{
	if (g_fGameOver)
		return; // intermission has already been triggered, so ignore.
	DelayedMVP(0);
	CHalfLifeMultiplay::GoToIntermission();
	g_fGameOver = true;
}


void CHazardZone::CheckWinConditions() {

	//	CUtlVector<CBasePlayer *> alive_people;
	int alive_people = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = (CBasePlayer*)UTIL_PlayerByIndex(i);
		if (pPlayer != nullptr) {
			if (!UTIL_IsValidEntity(pPlayer->edict()))
				continue;

			if (!pPlayer->IsAlive())
				continue;
			alive_people++;
		}
	}
	if (alive_people <= 1) {
		UTIL_Remove(hazzone);
		GoToIntermission();
	}
}
void CHazardZone::Think(void)
{
	// TODO: create hazard zone that shrinks and kills players for balancing in center
	if (g_iGameState == GAME_WARMUP)
	{
		float timeLeft = m_flWarmupEndTime - gpGlobals->time;

		if (timeLeft <= 0.0f)
		{
			EndWarmup();
		}
	}
	CHalfLifeMultiplay::Think();
	//	CHalfLifeMultiplay::Think();
}
void CHazardZone::ClientDisconnected(edict_t *pClient)
{
	CheckWinConditions();
	CHalfLifeMultiplay::ClientDisconnected(pClient);
}
BOOL CHazardZone::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char *szRejectReason) {
	return CHalfLifeMultiplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}
void CHazardZone::InitHUD(CBasePlayer *pl)
{
	CHalfLifeMultiplay::InitHUD(pl);
	if (g_iGameState == GAME_LIVE) {
		pl->StartObserver();
	}
}