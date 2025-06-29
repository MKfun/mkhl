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
//=========================================================
// GameRules
//=========================================================
// #define KILLFEED
//#include "weapons.h"
//#include "items.h"
class CBasePlayerItem;
class CBasePlayer;
class CItem;
class CBasePlayerAmmo;

// weapon respawning return codes
enum
{
	GR_NONE = 0,

	GR_WEAPON_RESPAWN_YES,
	GR_WEAPON_RESPAWN_NO,

	GR_AMMO_RESPAWN_YES,
	GR_AMMO_RESPAWN_NO,

	GR_ITEM_RESPAWN_YES,
	GR_ITEM_RESPAWN_NO,

	GR_PLR_DROP_GUN_ALL,
	GR_PLR_DROP_GUN_ACTIVE,
	GR_PLR_DROP_GUN_NO,

	GR_PLR_DROP_AMMO_ALL,
	GR_PLR_DROP_AMMO_ACTIVE,
	GR_PLR_DROP_AMMO_NO,
};

// Player relationship return codes
enum
{
	GR_NOTTEAMMATE = 0,
	GR_TEAMMATE,
	GR_ENEMY,
	GR_ALLY,
	GR_NEUTRAL,
};

const int CS_KILLS_FOR_DOMINATION = 4;

// Flags for specifying extra info about player death
enum DeathMessageFlags
{
	// float[3]
	// Position where the victim was killed by the enemy
	PLAYERDEATH_POSITION          = 0x001,

	// byte
	// Index of the assistant who helped the attacker kill the victim
	PLAYERDEATH_ASSISTANT         = 0x002,

	// short
	// Bitsum classification for the rarity of the kill
	// See enum KillRarity for details
	PLAYERDEATH_KILLRARITY        = 0x004
};

enum KillRarity
{
	KILLRARITY_HEADSHOT      = 0x001, // The killer player kills the victim with a headshot
	KILLRARITY_KILLER_BLIND  = 0x002, // The killer player was blind
	KILLRARITY_NOSCOPE       = 0x004, // The killer player kills the victim with a sniper rifle with no scope
	KILLRARITY_PENETRATED    = 0x008, // The killer player kills the victim through walls
	KILLRARITY_THROUGH_SMOKE = 0x010, // The killer player kills the victim through smoke
	KILLRARITY_ASSIST_FLASH  = 0x020, // The killer player kills the victim with an assistant flashbang grenade
	KILLRARITY_DOMINATION    = 0x040, // The killer player dominates the victim
	KILLRARITY_REVENGE       = 0x080  // The killer player got revenge on the victim
};

class CGameRules
{
public:
	virtual void RefreshSkillData(void); // fill skill data struct with proper values
	virtual void Think(void); // GR_Think - runs every server frame, should handle any timer tasks, periodic events, etc.
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity) = 0; // Can this item spawn (eg monsters don't spawn in deathmatch).

	virtual BOOL FAllowFlashlight(void) = 0; // Are players allowed to switch on their flashlight?
	virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) = 0; // should the player switch to this weapon?
	virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon) = 0; // I can't use this weapon anymore, get me the next best one.

	// Functions to verify the single/multiplayer status of a game
	virtual BOOL IsMultiplayer(void) = 0; // is this a multiplayer game? (either coop or deathmatch)
	virtual BOOL IsDeathmatch(void) = 0; //is this a deathmatch game?
	virtual BOOL IsTeamplay(void) { return FALSE; }; // is this deathmatch game being played with team rules?
	virtual BOOL IsCoOp(void) = 0; // is this a coop game?
	virtual const char *GetGameDescription(void) { return "Half-Life"; } // this is the game name that gets seen in the server browser

	// Client connection/disconnection
	virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]) = 0; // a client just connected to the server (player hasn't spawned yet)
	virtual void InitHUD(CBasePlayer *pl) = 0; // the client dll is ready for updating
	virtual void ClientDisconnected(edict_t *pClient) = 0; // a client just disconnected from the server
	virtual void UpdateGameMode(CBasePlayer *pPlayer) { } // the client needs to be informed of the current game mode

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer) = 0; // this client just hit the ground after a fall. How much damage?
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker) { return TRUE; }; // can this player take damage from this attacker?
	virtual BOOL ShouldAutoAim(CBasePlayer *pPlayer, edict_t *target) { return TRUE; }

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer) = 0; // called by CBasePlayer::Spawn just before releasing player into the game
	virtual void PlayerThink(CBasePlayer *pPlayer) = 0; // called by CBasePlayer::PreThink every frame, before physics are run and after keys are accepted
	virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer) = 0; // is this player allowed to respawn now?
	virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer) = 0; // When in the future will this player be able to spawn?
	virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer); // Place this player on their spawnspot and face them the proper direction.

	virtual BOOL AllowAutoTargetCrosshair(void) { return TRUE; };
	virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd) { return FALSE; }; // handles the user commands;  returns TRUE if command handled properly
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer) { } // the player has changed userinfo;  can change it now

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled) = 0; // how many points do I award whoever kills this player?
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) = 0; // Called each time a player dies
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor, int iDeathMessageFlags, int iRarityOfKill) = 0; // Call this from within a GameRules class to report an obituary.
	// Weapon retrieval
	virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon); // The player is touching an CBasePlayerItem, do I give it to him?
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) = 0; // Called each time a player picks up a weapon from the ground

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon) = 0; // should this weapon respawn?
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon) = 0; // when may this weapon respawn?
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon) = 0; // can i respawn now,  and if not, when should i try again?
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon) = 0; // where in the world should this weapon respawn?

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem) = 0; // is this player allowed to take this item?
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem) = 0; // call each time a player picks up an item (battery, healthkit, longjump)

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem) = 0; // Should this item respawn?
	virtual float FlItemRespawnTime(CItem *pItem) = 0; // when may this item respawn?
	virtual Vector VecItemRespawnSpot(CItem *pItem) = 0; // where in the world should this item respawn?

	// Ammo retrieval
	virtual BOOL CanHaveAmmo(CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry); // can this player take more of this ammo?
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount) = 0; // called each time a player picks up some ammo in the world

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo) = 0; // should this ammo item respawn?
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo) = 0; // when should this ammo item respawn?
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo) = 0; // where in the world should this ammo item respawn?
	    // by default, everything spawns

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void) = 0; // how long until a depleted HealthCharger recharges itself?
	virtual float FlHEVChargerRechargeTime(void) { return 0; } // how long until a depleted HealthCharger recharges itself?

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer) = 0; // what do I do with a player's weapons when he's killed?

	// What happens to a dead player's ammo
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer) = 0; // Do I drop ammo when the player dies? How much?

	// Teamplay stuff
	virtual const char *GetTeamID(CBaseEntity *pEntity) = 0; // what team is this entity on?
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget) = 0; // What is the player's relationship with this entity?
	virtual int GetTeamIndex(const char *pTeamName) { return -1; }
	virtual const char *GetIndexedTeamName(int teamIndex) { return ""; }
	virtual BOOL IsValidTeam(const char *pTeamName) { return TRUE; }
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib) { }
	virtual const char *SetDefaultPlayerTeam(CBasePlayer *pPlayer) { return ""; }

	// Sounds
	virtual BOOL PlayTextureSounds(void) { return TRUE; }
	virtual BOOL PlayFootstepSounds(CBasePlayer *pl, float fvol) { return TRUE; }

	// Monsters
	virtual BOOL FAllowMonsters(void) = 0; //are monsters allowed

	// Immediately end a multiplayer game
	virtual void EndMultiplayerGame(void) { }
};

extern CGameRules *InstallGameRules(void);

//=========================================================
// CHalfLifeRules - rules for the single player Half-Life
// game.
//=========================================================
class CHalfLifeRules : public CGameRules
{
public:
	CHalfLifeRules(void);

	// GR_Think
	virtual void Think(void);
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual BOOL FAllowFlashlight(void) { return TRUE; };

	virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);

	// Functions to verify the single/multiplayer status of a game
	virtual BOOL IsMultiplayer(void);
	virtual BOOL IsDeathmatch(void);
	virtual BOOL IsCoOp(void);

	// Client connection/disconnection
	virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void InitHUD(CBasePlayer *pl); // the client dll is ready for updating
	virtual void ClientDisconnected(edict_t *pClient);

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer);
	virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer);

	virtual BOOL AllowAutoTargetCrosshair(void);

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor, int iDeathMessageFlags, int iRarityOfKill);

	// Weapon retrieval
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

	// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount);

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo);

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void);

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);

	// What happens to a dead player's ammo
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);

	// Monsters
	virtual BOOL FAllowMonsters(void);

	// Teamplay stuff
	virtual const char *GetTeamID(CBaseEntity *pEntity) { return ""; };
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
};

//=========================================================
// CHalfLifeMultiplay - rules for the basic half life multiplayer
// competition
//=========================================================
class CHalfLifeMultiplay : public CGameRules
{
public:
	CHalfLifeMultiplay();

	// GR_Think
	virtual void Think(void);
	virtual void RefreshSkillData(void);
	virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual BOOL FAllowFlashlight(void);

	virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);

	// Functions to verify the single/multiplayer status of a game
	virtual BOOL IsMultiplayer(void);
	virtual BOOL IsDeathmatch(void);
	virtual BOOL IsCoOp(void);

	// Client connection/disconnection
	// If ClientConnected returns FALSE, the connection is rejected and the user is provided the reason specified in
	//  svRejectReason
	// Only the client's name and remote address are provided to the dll for verification.
	virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void InitHUD(CBasePlayer *pl); // the client dll is ready for updating
	virtual void ClientDisconnected(edict_t *pClient);
	virtual void UpdateGameMode(CBasePlayer *pPlayer); // the client needs to be informed of the current game mode

	// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);

	// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer);
	virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer);
	virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer);

	virtual BOOL AllowAutoTargetCrosshair(void);
	virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd);
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);

	// Client kills/scoring
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor, int iDeathMessageFlags, int iRarityOfKill);

	// Weapon retrieval
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon); // The player is touching an CBasePlayerItem, do I give it to him?

	// Weapon spawn/respawn control
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);

	// Item retrieval
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

	// Item spawn/respawn control
	virtual int ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

	// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount);

	// Ammo spawn/respawn control
	virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
	virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo);

	// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void);
	virtual float FlHEVChargerRechargeTime(void);

	// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);

	// What happens to a dead player's ammo
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);

	// Teamplay stuff
	virtual const char *GetTeamID(CBaseEntity *pEntity) { return ""; }
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);

	virtual BOOL PlayTextureSounds(void) { return FALSE; }
	virtual BOOL PlayFootstepSounds(CBasePlayer *pl, float fvol);

	// Monsters
	virtual BOOL FAllowMonsters(void);

	// Immediately end a multiplayer game
	virtual void EndMultiplayerGame(void) { GoToIntermission(); }
	// void SendDeathMessage(CBaseEntity *pKiller, CBasePlayer *pVictim, CBasePlayer *pAssister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
	// int GetRarityOfKill(CBaseEntity *pKiller, CBasePlayer *pVictim);
protected:
	virtual void ChangeLevel(void);
	virtual void GoToIntermission(void);
	float m_flIntermissionEndTime;
	BOOL m_iEndIntermissionButtonHit;

	void SendServerNameToClient(edict_t *client);

public:
	/**
	 * Sends a default MOTD (the one that is sent when the player joins the server).
	 */
	void SendDefaultMOTDToClient(edict_t *client);

	/**
	 * Returns false on error (file not found)
	 * @param file path to the file or nullptr for default
	 */
	bool SendMOTDFileToClient(edict_t *client, const char *file = nullptr);
	void SendMOTDToClient(edict_t *client, char *string);

	/**
	 * Returns false on error (file not found)
	 * @param file path to the file or nullptr for default
	 */
	bool SendUnicodeMOTDFileToClient(edict_t *client, const char *file = nullptr);
	void SendUnicodeMOTDToClient(edict_t *client, char *string);

	/**
	 * Returns false on error (file not found)
	 * @param file path to the file or nullptr for default
	 */
	bool SendHtmlMOTDFileToClient(edict_t *client, const char *file = nullptr);
	void SendHtmlMOTDToClient(edict_t *client, char *string);
};

//=========================================================
// CMultiplayBusters
// Rules for a multiplayer mode that makes you feel good
//=========================================================
class CMultiplayBusters : public CHalfLifeMultiplay
{
public:
	CMultiplayBusters();

	virtual void Think(void);
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);
	virtual void PlayerSpawn(CBasePlayer *pPlayer);

	void SetPlayerModel(CBasePlayer *pPlayer);

protected:
	float m_flEgonBustingCheckTime = -1.0f;
	void CheckForEgons(void);
};

extern DLL_GLOBAL CGameRules *g_pGameRules;
