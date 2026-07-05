//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Includes all the headers/declarations necessary to access the
//			engine interface
//
// $NoKeywords: $
//=============================================================================

#ifndef ENGINEINTERFACE_H
#define ENGINEINTERFACE_H

#ifdef _WIN32
#pragma once
#endif

// these stupid set of includes are required to use the cdll_int interface
#include "mathlib/vector.h"
#include "wrect.h"
#define IN_BUTTONS_H

// engine interface
#include "cdll_int.h"
#include "cvardef.h"

// engine interface singleton accessor
extern cl_enginefunc_t gEngfuncs;
//#define engine &gEngfuncs;
extern class IGameUIFuncs *gameuifuncs;
extern class IEngineSound *enginesound;

#endif // ENGINEINTERFACE_H