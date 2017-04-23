#include "stdafx.h"
#include "HillsApp.h"
#include "ShapesDemo.h"
#include "SkullDemo.h"
#include "WaveApp.h"
#include "LightingDemo.h"
#include "LitSkull.h"
#include "CrateDemo.h"
#include "TexturedHillsAndWavesDemo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	///Chapter 6
	//HillsApp theApp(hInstance);
	//ShapesDemo theApp(hInstance);
	//SkullDemo theApp(hInstance);
	//WaveApp theApp(hInstance);
	
	///Chapter 7
	//LightingDemo theApp(hInstance);
	//LitSkull theApp(hInstance);
	//CrateDemo theApp(hInstance);
	TexturedHillsAndWavesDemo theApp(hInstance);
	if (!theApp.Init())
		return 0;

	return theApp.Run();
}