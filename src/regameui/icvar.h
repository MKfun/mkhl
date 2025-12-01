#if !defined( ICVAR_H )
#define ICVAR_H
#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "interface.h"

class ICvar
{
public:
	// Try to register cvar
	virtual void			RegisterConCommandBase ( ConCommand *variable ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual char const*		GetCommandLineValue( char const *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual const ConVar	*FindVar ( const char *var_name ) = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommand	*GetCommands( void ) = 0;
};

#define VENGINE_CVAR_INTERFACE_VERSION "VEngineCvar001"

extern ICvar *cvar;

#endif // ICVAR_H