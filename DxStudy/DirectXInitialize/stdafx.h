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
//#include <d3d11.h>
//#include <DxErr.h>
//#include <stdio.h>
//#include <xnamath.h>
#include "Settings.h"
#include "GameTimer.h"
#include "d3dUtil.h"
#include "D3DApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "Effects.h"
#include "Vertex.h"
//#include "RHID3D11Device.h"


//extern WindowSettings GSettings;
extern GameTimer gGameTimer;

#define SAFE_RELEASE(p) {if(p){p->Release();p=nullptr;}}

// 정점 구조체의 정의
struct FVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct VertexWithNormal
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};
