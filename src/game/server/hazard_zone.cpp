//
// Created by den4ik on 28.12.25.
//
//==========================================================
// Purpose: Actual hazard zone that shrinks and dmgs players
//==========================================================
#include "hazard_zone.h"
#include "convar.h"
#include "util.h"
#include "player.h"
#include "shake.h"
#include "progdefs.h"
ConVar sv_hz_dmg("sv_hz_dmg", "4", FCVAR_SERVER, "zone damage per sec");
ConVar sv_hz_shrinkinter("sv_hz_shrinkinter", "60", FCVAR_SERVER, "zone shrinking velocity");
ConVar sv_hz_shrinkfraction("sv_hz_shrinkfraction", "1.5", FCVAR_SERVER, "zone shrinking fraction");

Vector GetRandomOrigin()
{
	edict_t* pSpot = NULL;
	edict_t* pSpawnList[64];
	int count = 0;

	while (!FNullEnt((pSpot = g_engfuncs.pfnFindEntityByString(pSpot, "classname", "info_player_deathmatch"))))
	{
		if (count < 64)
		{
			pSpawnList[count] = pSpot;
			count++;
		}
	}

	if (count > 0)
	{
		int randomIndex = RANDOM_LONG(0, count - 1);

		return pSpawnList[randomIndex]->v.origin;
	}

	return Vector(0, 0, 0);
}

void CHazZone::Spawn() {
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	m_flRadius = 8192.0f;

	UTIL_SetOrigin(pev, GetRandomOrigin());
	m_flNextShrinkTime = gpGlobals->time; //+ 30.0f; // wait 30s
	m_flNextDamageTime = gpGlobals->time + 1.0f;
	m_flStartTime = gpGlobals->time;
	pev->nextthink = gpGlobals->time + 0.1f;
//	UTIL_ClientPrintAll(HUD_PRINTCENTER, "ZONE_CREATED");
}
LINK_ENTITY_TO_CLASS(hazard_zone, CHazZone);

void CHazZone::Think( void )
{
	float flTimePassed = gpGlobals->time - m_flStartTime;
	float flFraction = flTimePassed / sv_hz_shrinkinter.GetFloat();

	if (flFraction > 1.0f)
		flFraction = 1.0f;

//	m_flRadius = 512.0f + (512.0f - 8192.0f) * flFraction;
	if (gpGlobals->time >= m_flNextShrinkTime) {
		m_flNextShrinkTime = gpGlobals->time + sv_hz_shrinkinter.GetFloat();
		m_flRadius /= sv_hz_shrinkfraction.GetFloat();
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "ZONE SHRUNK!");
	}
	int iDivides = 128;
	float flAngleMulScalar = (360.0f/iDivides);
	Vector fOrigin, fAngle, fVector;
	Vector vForward, vRight, vUp;
	for (int i = 0; i < iDivides; i++)
	{
		g_engfuncs.pfnAngleVectors(pev->angles, vForward, vRight, vUp);
		fVector = vRight * m_flRadius;
		fOrigin = pev->origin + fVector;
		CreateBeam(fOrigin);
		pev->angles.y += flAngleMulScalar;
	}
	if (gpGlobals->time >= pev->dmgtime)
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBaseEntity *pEnt = UTIL_PlayerByIndex(i);
			if (pEnt && pEnt->IsAlive())
			{
				float flDist = (pEnt->pev->origin - pev->origin).Length2D();
				if (flDist > m_flRadius)
				{
					pEnt->TakeDamage(pev, pev, sv_hz_dmg.GetFloat(), DMG_RADIATION);
					UTIL_ScreenFade(pEnt, Vector(0, 200, 10), 1.0f, 0.5f, 40, FFADE_MODULATE);
					ClientPrint(pEnt->pev, HUD_PRINTCENTER, "Return to safe zone.");
				}
			}
		}
		pev->dmgtime = gpGlobals->time + 1.0f;
	}
	pev->nextthink = gpGlobals->time + 0.1f;
}
void CHazZone::Precache() {
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/laserbeam.spr" );
}
void CHazZone::CreateBeam( Vector fOrigin ) {
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, pev->origin);

	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(fOrigin.x);
	WRITE_COORD(fOrigin.y);
	WRITE_COORD(fOrigin.z -500.0f);
	WRITE_COORD(fOrigin.x);
	WRITE_COORD(fOrigin.y);
	WRITE_COORD(4000.0f); // point up, radius 1000 units
	WRITE_SHORT(m_iSpriteTexture);
	WRITE_BYTE(1);				// starting frame
	WRITE_BYTE(5);			// 25 fps
	WRITE_BYTE(10);	// 2.3 then 2.6 seconds of life
	WRITE_BYTE(50);				// 5 units wide
	WRITE_BYTE(0);				// no noise (doesn't work)
	WRITE_BYTE(0);	// colour R
	WRITE_BYTE(200); // colour G
	WRITE_BYTE(10); // colour B
	WRITE_BYTE(128);			// brightness
	WRITE_BYTE(0);				// don't scroll (doesn't work)
	MESSAGE_END();
}

