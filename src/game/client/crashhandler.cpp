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
#include <stdio.h>
#include <stdlib.h>
#include "sdl_rt.h"
extern "C" {
#include <backtrace.h>
}
char buffer[512];
char outbuf[2048];

static struct backtrace_state *bt_state = nullptr;

static void bt_error(void *data, const char *msg, int errnum)
{
	GetSDL()->ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CRASH3D", msg);
	fprintf(stderr, "BacktraceD at: %s (%d)\n", msg, errnum);
}

static int bt_full(
    void *data,
    uintptr_t pc,
    const char *filename,
    int lineno,
    const char *function)
{
	fprintf(stderr,
	    "BacktraceD at: %s (%s:%d) [0x%lx]\n",
	    function ? function : "??",
	    filename ? filename : "??",
	    lineno,
	    (unsigned long)pc);
	sprintf(buffer, "BacktraceD at: %s (%s:%d) [0x%lx]\n",  	    function ? function : "??",
	    filename ? filename : "??",
	    lineno,
	    (unsigned long)pc);
	strcat(outbuf, buffer);
	return 0;
}

void HL_DumpBacktrace()
{
	if (!bt_state)
	{
		bt_state = backtrace_create_state(
		    "client.so",
		    1,
		    bt_error,
		    NULL
		);
	}

	backtrace_full(
	    bt_state,
	    0,
	    bt_full,
	    bt_error,
	    NULL
	);
	GetSDL()->ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CRASH3D", buffer);
}
