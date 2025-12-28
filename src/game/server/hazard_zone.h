//
// Created by den4ik on 28.12.25.
//

#ifndef HAZARD_ZONE_H
#define HAZARD_ZONE_H
#include "cbase.h"
#include "mathlib/vector.h"
//#include ""
class CHazZone : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual void Think();
private:
	Vector m_vecCenter;
	float  m_flRadius;
	float  m_flTargetRadius;
	float  m_flStartTime;
	int    m_iSpriteTexture;
	float  m_flNextShrinkTime = 0;
	float  m_flNextDamageTime = 0;
//	float
//	virtual void
	void DmgPlr();
	void CreateBeam(Vector fOrigin);
};

#endif //HAZARD_ZONE_H
