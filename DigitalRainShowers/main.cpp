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

//This hides the console window
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#define FREEGLUT_STATIC
#include "..\Dependencies\include\GL\glut.h"
#include "../Dependencies/include/glm/vec3.hpp" // glm::vec3
#include "../Dependencies/include/glm/glm.hpp"
#include <windows.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include<sstream>
#include <windows.h>
#include <ctime>
#include "RainStorm.h"
#include <vector>
#include "DisplayManager.h"
#include "globals.h"
#include "dFont.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Globals
bool debugging = false;
HANDLE hMutex;

void startScreenSaver(
	int argc,
	char* argv[],
	HWND hwnd,
	bool previewOnly)
{
	if (previewOnly)
	{
		// Only allow one instance of the preview
		hMutex = CreateMutex(NULL, TRUE, L"Global\\DigitalRainShowersScreenSaverPreviewMutex");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			if (hMutex != NULL) {
				CloseHandle(hMutex);
			}
			return;
		}

		// Run screensaver on the preview window
		bool initializeWorkd = DisplayManager::CreatePreviewOnWindow(hwnd);
		if (initializeWorkd)
		{
			DisplayManager::previewMainLoop(hwnd);
		}
	}
	else {
		glutInit(&argc, argv);
		DisplayManager::CreateWindowsOnMonitors();
		glutTimerFunc(1, DisplayManager::update, 1);
		glutMainLoop(); // Handle all events for all windows
	}
}

int main(int argc, char* argv[])
{
	// Seed the random number generator
	std::srand(static_cast<unsigned int>(std::time(0)));

	HWND hwnd = NULL;
	bool previewOnly = false;
	
	// Check command line inscase a preview is needed
	if (argc > 1) {
		std::string arg = argv[1];
		if (arg == "/p" || arg == "/P")
		{
			hwnd = (HWND)atoi(argv[2]);
			previewOnly = true;
		}
		else if (arg.find("/c:") == 0 || arg.find("/C:") == 0)
		{
			MessageBox(nullptr, TEXT("No options \nThis screen saver has no options that you can set."), TEXT("DigitalRainShowers Screen Saver"), MB_OK | MB_ICONINFORMATION);
			return 0;
		}
	}

	startScreenSaver(argc, argv, hwnd, previewOnly);
	return 0;
}



