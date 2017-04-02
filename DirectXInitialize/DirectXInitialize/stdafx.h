// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WindowsX.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <string>
#include <sstream>

// TODO: reference additional headers your program requires here
#include <d3d11.h>
#include <DxErr.h>
#include <stdio.h>
#include <xnamath.h>
#include "Settings.h"
#include "GameTimer.h"
#include "D3DApp.h"

//#include "RHID3D11Device.h"


//extern WindowSettings GSettings;
//extern GameTimer gGameTimer;

#define SAFE_RELEASE(p) {if(p){p->Release();p=nullptr;}}

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
						{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
						}                                                      \
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 
