//=========== (C) Copyright 1996-2002, Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Quake world items
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "shake.h"
#include "../engine/studio.h"
#include "weapons.h"
// #include "quake_gun.h"
#include "hltv.h"

extern unsigned short g_usPowerUp;

class CQuakeItem : public CBaseEntity
{
public:
	void	Spawn( void );

	// Respawning
	void	EXPORT Materialize( void );
	void	Respawn( float flTime );

	virtual void SetObjectCollisionBox ( void );

	// Touch
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	virtual	BOOL MyTouch( CBasePlayer *pOther ) { return FALSE; };

	float	m_flRespawnTime;
};

//-----------------------------------------------------------------------------
// Purpose: Spawn and drop to the floor
//-----------------------------------------------------------------------------

void CQuakeItem :: SetObjectCollisionBox( void )
{
	pev->absmin = pev->origin + Vector(-32, -32, 0);
	pev->absmax = pev->origin + Vector(32, 32, 56); 
}

void CQuakeItem::Spawn()
{ 
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	
	SetTouch(&CQuakeItem::ItemTouch);

	if (DROP_TO_FLOOR(ENT(pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z);
		UTIL_Remove( this );
		return;
	}

	//UTIL_SetOrigin( pev, pev->origin + Vector(0,0,16) );

	if (!m_flRespawnTime)
		m_flRespawnTime = 20;
}

//-----------------------------------------------------------------------------
// Purpose: Bring the item back
//-----------------------------------------------------------------------------
void CQuakeItem::Materialize()
{
	// Become visible and touchable
	pev->effects &= ~EF_NODRAW;
	SetTouch( &CQuakeItem::ItemTouch );

	// Play respawn sound
	EMIT_SOUND( ENT(pev), CHAN_WEAPON, "items/itembk2.wav", 1, ATTN_NORM );
}

//-----------------------------------------------------------------------------
// Purpose: Setup the item's respawn in the time set
//-----------------------------------------------------------------------------
void CQuakeItem::Respawn( float flTime )
{
	pev->effects |= EF_NODRAW;
	SetTouch( NULL );

	// Come back in time
	SetThink ( &CQuakeItem::Materialize );
	pev->nextthink = gpGlobals->time + flTime;
}


//-----------------------------------------------------------------------------
// Purpose: Touch function that calls the virtual touch function
//-----------------------------------------------------------------------------
void CQuakeItem::ItemTouch( CBaseEntity *pOther )
{
	// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
		return;

	//Dead?
	if (pOther->pev->health <= 0)
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	// Call the virtual touch function
	if ( MyTouch( pPlayer ) )
	{
		SUB_UseTargets( pOther, USE_TOGGLE, 0 );

		// Respawn if it's not DM==2
		if (gpGlobals->deathmatch != 2)
		{
			Respawn( m_flRespawnTime );
		}
		else
		{
			UTIL_Remove( this );
		}
	}
}
//===============================================================================
// POWERUPS
//===============================================================================
class CItemPowerup : public CQuakeItem
{
public:
	BOOL MyTouch( CBasePlayer *pPlayer );

	int		m_iPowerupBit;
	float	invincible_finished;
	float	radsuit_finished;
	float	invisible_finished;
	float	super_damage_finished;
};

// Powerup Touch
BOOL CItemPowerup::MyTouch( CBasePlayer *pPlayer )
{
	if (pPlayer->pev->health <= 0)
		return FALSE;
	// if (pPlayer->m_pActiveItem)
	// {
	if ( pPlayer->HasPlayerItemFromID(WEAPON_EGON) || pPlayer->HasPlayerItemFromID(WEAPON_GAUSS) )
		return false;
	// }
	EMIT_SOUND( ENT(pev), CHAN_ITEM, STRING(pev->noise), 1, ATTN_NORM );

	pPlayer->m_iQuakeItems |= m_iPowerupBit;
	
	int iPowerUp = 0;

	// Invincibility
	if (invincible_finished)
	{
		// Make them glow red

		if ( pPlayer->m_iQuakeItems & IT_QUAD )
		{
			pPlayer->pev->renderfx = kRenderFxGlowShell;
			pPlayer->pev->rendercolor = Vector( 255, 125, 255 );	// RGB
			pPlayer->pev->renderamt = 100;	// Shell size
			
			iPowerUp = 3;
		}
		else
		{
			pPlayer->pev->renderfx = kRenderFxGlowShell;
			pPlayer->pev->rendercolor = Vector( 255, 128, 0 );	// RGB
			pPlayer->pev->renderamt = 100;	// Shell size

			iPowerUp = 2;
		}

		if ( pPlayer->m_iQuakeItems & IT_INVISIBILITY )
		{
			pPlayer->pev->rendermode = kRenderTransColor;
			pPlayer->pev->renderamt = 1;
		}
		pPlayer->m_flInvincibleFinished = gpGlobals->time + invincible_finished;

	}
	
	// Quad Damage
	if (super_damage_finished)
	{
		// Make them glow blue

		if ( pPlayer->m_iQuakeItems & IT_INVULNERABILITY )
		{
			pPlayer->pev->renderfx = kRenderFxGlowShell;
			pPlayer->pev->rendercolor = Vector( 255, 125, 255 );	// RGB
			pPlayer->pev->renderamt = 100;	// Shell size

			iPowerUp = 3;
		}
		else
		{
			pPlayer->pev->renderfx = kRenderFxGlowShell;
			pPlayer->pev->rendercolor = Vector( 128, 128, 255 );	// RGB
			pPlayer->pev->renderamt = 100;	// Shell size

			iPowerUp = 1;
		}

		if ( pPlayer->m_iQuakeItems & IT_INVISIBILITY )
		{
			pPlayer->pev->rendermode = kRenderTransColor;
			pPlayer->pev->renderamt = 1;
		}


		pPlayer->m_flSuperDamageFinished = gpGlobals->time + super_damage_finished;

		// Remove armor and cells if DM==4
		if (gpGlobals->deathmatch == 4)
		{
			pPlayer->pev->armortype = 0;
			pPlayer->pev->armorvalue = 0;
			// pPlayer->m_iAmmoCells = 0;
		}
	}

	// Radiation suit
	if (radsuit_finished)
		pPlayer->m_flRadsuitFinished = gpGlobals->time + radsuit_finished;

	// Invisibility
	if (invisible_finished)
	{
		pPlayer->m_flInvisibleFinished = gpGlobals->time + invisible_finished;

		pPlayer->pev->renderfx = kRenderFxGlowShell;
		pPlayer->pev->rendercolor = Vector( 128, 128, 128 );	// RGB
		pPlayer->pev->renderamt = 5;	// Shell size
	
	}
	
	// tell director about it
	MESSAGE_BEGIN( MSG_SPEC, SVC_DIRECTOR );
		WRITE_BYTE ( 9 );	// command length in bytes
		WRITE_BYTE ( DRC_CMD_EVENT );	// powerup pickup
		WRITE_SHORT( ENTINDEX(pPlayer->edict()) );	// player is primary target
		WRITE_SHORT( ENTINDEX(this->edict()) );	// powerup as second target
		WRITE_LONG( 9 );   // highst prio in game
	MESSAGE_END();

	// pPlayer->W_SetCurrentAmmo();

	PLAYBACK_EVENT_FULL( FEV_GLOBAL | FEV_RELIABLE, 
	pPlayer->edict(), g_usPowerUp, 0, (float *)&g_vecZero, (float *)&g_vecZero, 
	(float)iPowerUp, 0.0, pPlayer->entindex(), pPlayer->pev->team, 0, 0 );

	return TRUE;
}


//===============
// Pentagram
class CItemPowerupInvincible : public CItemPowerup
{
public:
	void Spawn( void );
	void Precache( void );
};
LINK_ENTITY_TO_CLASS(item_artifact_invulnerability, CItemPowerupInvincible);

// Spawn
void CItemPowerupInvincible::Spawn( void )
{
	Precache();
	CQuakeItem::Spawn();

	m_flRespawnTime = 300;
	invincible_finished = 30;

	SET_MODEL(ENT(pev), "models/pow_invuln.mdl");
	pev->netname = MAKE_STRING("Pentagram of Protection");
	pev->noise = MAKE_STRING("items/protect.wav");
	m_iPowerupBit = IT_INVULNERABILITY;

	// Make it glow red
	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor = Vector( 255, 128, 0 );	// RGB
	pev->renderamt = 100; // Shellsize
}

// Precache
void CItemPowerupInvincible::Precache( void )
{
	PRECACHE_MODEL("models/pow_invuln.mdl");
	PRECACHE_SOUND("items/protect.wav");
	PRECACHE_SOUND("items/protect2.wav");
	PRECACHE_SOUND("items/protect3.wav");
}

//===============
// Radiation Suit
class CItemPowerupRadsuit : public CItemPowerup
{
public:
	void Spawn( void );
	void Precache( void );
};
LINK_ENTITY_TO_CLASS(item_artifact_envirosuit, CItemPowerupRadsuit);

// Spawn
void CItemPowerupRadsuit::Spawn( void )
{
	Precache();
	CQuakeItem::Spawn();

	m_flRespawnTime = 60;
	radsuit_finished = 30;

	SET_MODEL(ENT(pev), "models/suit.mdl");
	pev->netname = MAKE_STRING("Biosuit");
	pev->noise = MAKE_STRING("items/suit.wav");
	m_iPowerupBit = IT_SUIT;
}

// Precache
void CItemPowerupRadsuit::Precache( void )
{
	PRECACHE_MODEL("models/suit.mdl");
	PRECACHE_SOUND("items/suit.wav");
	PRECACHE_SOUND("items/suit2.wav");
}

//===============
// Ring of Invisibility
class CItemPowerupInvisibility : public CItemPowerup
{
public:
	void Spawn( void );
	void Precache( void );
};
LINK_ENTITY_TO_CLASS(item_artifact_invisibility, CItemPowerupInvisibility);

// Spawn
void CItemPowerupInvisibility::Spawn( void )
{
	Precache();
	CQuakeItem::Spawn();

	m_flRespawnTime = 300;
	invisible_finished = 30;

	SET_MODEL(ENT(pev), "models/pow_invis.mdl");
	pev->netname = MAKE_STRING("Ring of Shadows");
	pev->noise = MAKE_STRING("items/inv1.wav");
	m_iPowerupBit = IT_INVISIBILITY;

	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor = Vector( 128, 128, 128 );	// RGB
	pev->renderamt = 25;	// Shell size

	pev->rendermode = kRenderTransColor;
	pev->renderamt = 30;
}

// Precache
void CItemPowerupInvisibility::Precache( void )
{
	PRECACHE_MODEL("models/pow_invis.mdl");
	PRECACHE_SOUND("items/inv1.wav");
	PRECACHE_SOUND("items/inv2.wav");
	PRECACHE_SOUND("items/inv3.wav");
}

//===============
// Quad Damage
class CItemPowerupQuad : public CItemPowerup
{
public:
	void Spawn( void );
	void Precache( void );
};
LINK_ENTITY_TO_CLASS(item_artifact_super_damage, CItemPowerupQuad);

// Spawn
void CItemPowerupQuad::Spawn( void )
{
	Precache();
	CQuakeItem::Spawn();

	m_flRespawnTime = 60;
	super_damage_finished = 30;

	SET_MODEL(ENT(pev), "models/pow_quad.mdl");
	pev->netname = MAKE_STRING("Quad Damage");
	pev->noise = MAKE_STRING("items/damage.wav");
	m_iPowerupBit = IT_QUAD;

	// Make it glow blue
	pev->renderfx = kRenderFxGlowShell;
	pev->rendercolor = Vector( 128, 128, 255 );	// RGB
	pev->renderamt = 100;	// Shell size
}

// Precache
void CItemPowerupQuad::Precache( void )
{
	PRECACHE_MODEL("models/pow_quad.mdl");
	PRECACHE_SOUND("items/damage.wav");
	PRECACHE_SOUND("items/damage2.wav");
	PRECACHE_SOUND("items/damage3.wav");
}
