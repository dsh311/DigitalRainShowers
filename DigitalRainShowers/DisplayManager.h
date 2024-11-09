// DigitalRainShowers Screensaver
// Copyright (c) 2024 David S. Shelley
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#define FREEGLUT_STATIC
#include "RainStorm.h"
#include "DisplayWindowInfo.h"
#include <vector>
#include "Input.h"
#include <sstream>
#include "..\Dependencies\include\GL\glut.h"
#include <GL/gl.h> 

namespace DisplayManager
{

	extern std::vector<DisplayWindowInfo> WindowIDs;
	extern std::vector<RainStorm> RainStorms;
	//static Input UserInput;

	extern std::clock_t fpsTimeDM ;
	extern int fpsCountDM;

	extern std::clock_t appStartTimeDM;
	extern float wakeUpGracePeriodDM;
	extern float showLicensesGracePeriodDM;

	// Variables for window handle and rendering context
	extern HWND hwndPreview;
	extern HDC hdcPreview;
	extern HGLRC hglrcPreview;

	extern void update(int value);
	extern void setupRC();
	extern void CreateWindowsOnMonitors();
	extern bool CreatePreviewOnWindow(HWND hwnd);
	extern void previewMainLoop(HWND hwnd);
	//extern int CreateContextOnPreviewWindow(HWND hwnd, RECT rect);
	extern BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

	extern void renderScene(void);
	extern void renderPreviewScene(void);
	extern void changeSize(int w, int h);
	extern HWND getWindowHandle(const std::string& windowTitle);
	extern void makeWindowTopmost(HWND hWnd);
	extern void closeAllWindows();
	extern void exitScreenSaver();
	extern RainStorm& getDisplayWindowRain(int index);
	extern void addRainStorm(RainStorm addThisRainStorm);
	extern void addDisplayWindow(DisplayWindowInfo addThisDisplayWindow);
	extern int getIndexFromGlutWindowID(int windowID);
	extern DisplayWindowInfo& getDisplayWindowInfo(int index);
	extern size_t getDisplayCount();
};

