/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// Created by den4ik on 22.11.25.
//
#if !defined(OEM_BUILD) && !defined(HLDEMO_BUILD)

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

enum bump_state
{
	BUMP_IDLE = 0,
	BUMP_READY,
	BUMP_RELOAD,
};

enum bump_e
{
	BUMP_IDLE1 = 0,
	BUMP_FIDGET1,
	BUMP_DRAW,
	BUMP_DROP
};

class CBumpMine : public CGrenade
{
	Vector m_lastBounceOrigin; // Used to fix a bug in engine: when object isn't moving, but its speed isn't 0 and on ground isn't set
	void Spawn(void);
	void Precache(void);
	void BounceSound(void);

	void EXPORT BumpmineSetup(CBaseEntity *pOther);
	void EXPORT BumpThink(void);

public:
	void Deactivate(CBasePlayer *pOwner);
};
LINK_ENTITY_TO_CLASS(monster_bump, CBumpMine);

//=========================================================
// Deactivate - do whatever it is we do to an orphaned
// bumps when we don't want it in the world anymore.
//=========================================================
void CBumpMine::Deactivate(CBasePlayer *pOwner)
{
	pev->solid = SOLID_NOT;
	UTIL_Remove(this);
}

void CBumpMine::Spawn(void)
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	SET_MODEL(ENT(pev), "models/w_bumpmine.mdl");
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4)); // Uses point-sized, and can be stepped over
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(&CBumpMine::BumpmineSetup);
	SetThink(&CBumpMine::BumpThink);
	pev->nextthink = gpGlobals->time + 0.1;

	pev->gravity = 0.5;
	pev->friction = 0;

	pev->dmg = 0;
	pev->sequence = 1;
}

void CBumpMine::BumpmineSetup(CBaseEntity *pOther)
{
	entvars_t *pevOther = pOther->pev;
	pev->gravity = 1; // normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin - Vector(0, 0, 10), ignore_monsters, edict(), &tr);

	if (tr.flFraction < 1.0)
	{
		// fullstop
		pev->velocity = pev->velocity*0;
		pev->avelocity = pev->avelocity*0;
		// play sliding sound, volume based on velocity
	}
	if (pOther->IsPlayer())
	{
		float flBoostSpeed = 800.0f;

		Vector vecNewVelocity = pevOther->velocity;
		Vector vecAim;
		UTIL_MakeVectors( pevOther->v_angle );
		vecAim = gpGlobals->v_forward;
		vecNewVelocity = vecNewVelocity + vecAim * flBoostSpeed;
		vecNewVelocity.z = vecNewVelocity.z + 800.0f;
		pevOther->velocity = vecNewVelocity;
		pOther->pev->flags &= ~FL_ONGROUND;
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "common/bodysplat.wav", 1, ATTN_NORM);
		UTIL_Remove(this);
	}
	m_lastBounceOrigin = pev->origin;
}

void CBumpMine ::BumpThink(void)
{
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->waterlevel == 3)
	{
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = pev->velocity * 0.8;
		pev->avelocity = pev->avelocity * 0.9;
		pev->velocity.z += 8;
	}
	else if (pev->waterlevel == 0)
	{
		pev->movetype = MOVETYPE_TOSS;
	}
	else
	{
		pev->velocity.z = 0;
	}
}

void CBumpMine ::Precache(void)
{
	PRECACHE_MODEL("models/w_bumpmine.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
}

void CBumpMine ::BounceSound(void)
{
	return;
}

LINK_ENTITY_TO_CLASS(weapon_bumpmine, CHandBump);

//=========================================================
// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
//=========================================================
int CHandBump::AddDuplicate(CBasePlayerItem *pOriginal)
{
	CHandBump *pBump;

#ifdef CLIENT_DLL
	if (bIsMultiplayer())
#else
	if (g_pGameRules->IsMultiplayer())
#endif
	{
		pBump = (CHandBump *)pOriginal;

		if (!pOriginal->m_pPlayer)
			return TRUE;

		int nBumpsInPocket = pBump->m_pPlayer->m_rgAmmo[pBump->PrimaryAmmoIndex()];
		int nNumBumps = 0;
		CBaseEntity *pLiveBump = NULL;

		while ((pLiveBump = UTIL_FindEntityInSphere(pLiveBump, pOriginal->m_pPlayer->pev->origin, 4096)) != NULL)
		{
			if (FClassnameIs(pLiveBump->pev, "monster_bump"))
			{
				if (pLiveBump->pev->owner == pOriginal->m_pPlayer->edict())
				{
					nNumBumps++;
				}
			}
		}

		if (pBump->m_chargeReady != BUMP_IDLE && (nBumpsInPocket + nNumBumps) >= BUMPMINE_MAX_CARRY)
		{
			// player has some bumps deployed. Refuse to add more.
			return FALSE;
		}
	}

	return CBasePlayerWeapon::AddDuplicate(pOriginal);
}

//=========================================================
//=========================================================
int CHandBump::AddToPlayer(CBasePlayer *pPlayer)
{
	int bResult = CBasePlayerItem::AddToPlayer(pPlayer);
	if (bResult)
	{
		return AddWeapon();
	}
	return FALSE;
}

void CHandBump::Spawn()
{
	Precache();
	m_iId = WEAPON_BUMPMINE;
	SET_MODEL(ENT(pev), "models/w_bumpmine.mdl");

	m_iDefaultAmmo = BUMPMINE_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}

void CHandBump::Precache(void)
{
	PRECACHE_MODEL("models/v_bumpmine.mdl");
	PRECACHE_MODEL("models/w_bumpmine.mdl");
	PRECACHE_MODEL("models/p_satchel.mdl");

	UTIL_PrecacheOther("monster_bump");
}

int CHandBump::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Bump Mines";
	p->iMaxAmmo1 = BUMPMINE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = m_iId = WEAPON_BUMPMINE;
	p->iWeight = BUMPMINE_WEIGHT;

	return 1;
}

//=========================================================
//=========================================================
BOOL CHandBump::IsUseable(void)
{
	return CanDeploy();
}

BOOL CHandBump::CanDeploy(void)
{
	return TRUE;
}

BOOL CHandBump::Deploy()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
		return DefaultDeploy("models/v_bumpmine.mdl", "models/p_satchel.mdl", BUMP_DRAW, "trip");

	return TRUE;
}

void CHandBump::Holster(int skiplocal /* = 0 */)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= 0)
	{
		SendWeaponAnim(BUMP_DROP);
	}
	else
	{
		DestroyItem();
	}
}

void CHandBump::PrimaryAttack(void)
{
	if (m_chargeReady != BUMP_RELOAD)
	{
		Throw();
	}
}

void CHandBump::SecondaryAttack()
{
}

void CHandBump::Throw(void)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
#ifndef CLIENT_DLL
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		Vector vecSrc = m_pPlayer->pev->origin+Vector(30*gpGlobals->v_forward.x,30*gpGlobals->v_forward.y,0);

		Vector vecThrow = gpGlobals->v_forward * 274 + m_pPlayer->pev->velocity;

		CBaseEntity *pBump = Create("monster_bump", vecSrc, Vector(0,0,0), m_pPlayer->edict());
		pBump->pev->velocity = vecThrow;
		pBump->pev->avelocity.y = 400;
		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
#endif
		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		m_chargeReady = BUMP_READY;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) {
			Holster();
			DestroyItem();
		}
	}
}

void CHandBump::WeaponIdle(void)
{
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	switch (m_chargeReady)
	{
	case BUMP_IDLE:
		SendWeaponAnim(BUMP_FIDGET1);
		// use tripmine animations
		UTIL_strcpy(m_pPlayer->m_szAnimExtention, "trip");
		break;
	case BUMP_READY:
		// use hivehand animations
		UTIL_strcpy(m_pPlayer->m_szAnimExtention, "hive");
		break;
	case BUMP_RELOAD:
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		{
			Holster();
			return;
		}

#ifndef CLIENT_DLL
		m_pPlayer->pev->viewmodel = MAKE_STRING("models/v_bumpmine.mdl");
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/p_satchel.mdl");
#else
		LoadVModel("models/v_bumpmine.mdl", m_pPlayer);
#endif

		SendWeaponAnim(BUMP_DRAW);

		// use tripmine animations
		UTIL_strcpy(m_pPlayer->m_szAnimExtention, "trip");

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_chargeReady = BUMP_IDLE;
		break;
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}

//=========================================================
// DeactivateBumps - removes all bumps owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateBumps(CBasePlayer *pOwner)
{
	edict_t *pFind;

	pFind = FIND_ENTITY_BY_CLASSNAME(NULL, "monster_bump");

	while (!FNullEnt(pFind))
	{
		CBaseEntity *pEnt = CBaseEntity::Instance(pFind);
		CBumpMine *pBump = (CBumpMine *)pEnt;

		if (pBump)
		{
			if (pBump->pev->owner == pOwner->edict())
			{
				pBump->Deactivate(pOwner);
			}
		}

		pFind = FIND_ENTITY_BY_CLASSNAME(pFind, "monster_bump");
	}
}

#endif
