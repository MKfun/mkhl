//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

//#include "..\Tracker\Common\winlite.h"
#include "Sys_Utils.h"
#include "EngineInterface.h"

const unsigned int SYS_NO_ERROR = 0L;
const unsigned int SYS_ERROR_INVALID_HANDLE = 0x6;

void Sys_SetLastError(unsigned long error)
{
//	::SetLastError(error);
}

unsigned long Sys_GetLastError()
{
	return 0L;
}


WHANDLE Sys_CreateMutex(const char *mutexName)
{
	return 0; //(WHANDLE)::CreateMutex(NULL, FALSE, TEXT(mutexName));
}

void Sys_ReleaseMutex(WHANDLE mutexHandle)
{
//	::ReleaseMutex((HANDLE)mutexHandle);
}


//const unsigned int SYS_WAIT_OBJECT_0 = WAIT_OBJECT_0;
//const unsigned int SYS_WAIT_ABANDONED = WAIT_ABANDONED;

unsigned int Sys_WaitForSingleObject(WHANDLE mutexHandle, int milliseconds)
{
	return 0; //::WaitForSingleObject((HANDLE)mutexHandle, milliseconds);
}

unsigned int Sys_RegisterWindowMessage(const char *msgName)
{
	return 0;//::RegisterWindowMessage(msgName);
}

WHANDLE Sys_FindWindow(const char *className, const char *windowName)
{
	return 0; //(WHANDLE)::FindWindow(className, windowName);
}

void Sys_EnumWindows(void *callbackFunction, int lparam)
{
	//::EnumWindows((WNDENUMPROC)callbackFunction, lparam);
}

void Sys_GetWindowText(WHANDLE wnd, char *buffer, int bufferSize)
{
//	::GetWindowText((HWND)wnd, buffer, bufferSize - 1);
}

void Sys_PostMessage(WHANDLE wnd, unsigned int msg, unsigned int wParam, unsigned int lParam)
{
	// ::PostMessageA((HWND)wnd, msg, wParam, lParam);
}

void Sys_SetCursorPos(int x, int y)
{
	//::SetCursorPos(x, y);
//		engine->SetCursorPos(x,y); // SRC version
}

WHANDLE Sys_CreateWindowEx(const char *windowName)
{
	/*
	if (!staticWndclassAtom)
	{
		memset( &staticWndclass,0,sizeof(staticWndclass) );
		staticWndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		staticWndclass.lpfnWndProc = staticProc;
		staticWndclass.hInstance = GetModuleHandle(NULL);
		staticWndclass.hIcon = 0;
		staticWndclass.lpszClassName = windowName;
		staticWndclassAtom = ::RegisterClass( &staticWndclass );

		DWORD error = ::GetLastError();
	}

	return (WHANDLE)::CreateWindow(windowName, windowName, 0, 0, 0, 0, 0, 0, 0, GetModuleHandle(NULL), 0);
	*/
	return (WHANDLE)1;
}

void Sys_DestroyWindow(WHANDLE wnd)
{
	//::DestroyWindow((HWND)wnd);
}
