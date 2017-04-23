#pragma once

struct WindowSettings
{
	int Width = 800;
	int Height = 600;
	HWND WindowHandle = 0;
	bool bEnableMSAA = false;
	int MSAASampleNum = 4;
	UINT MSAAQuality = 0;

	bool IsInitialized() { return WindowHandle != 0; }
};

extern WindowSettings GSettings;