/*
Copyright (C) 2025 den4iklovelinux

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
    	claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

	den4iklovelinux			denzel8028@hotmail.com

*/
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
