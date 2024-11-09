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

#include "DisplayManager.h"

namespace DisplayManager
{
    std::vector<DisplayWindowInfo> WindowIDs; // Definition
    std::vector<RainStorm> RainStorms;

	std::clock_t fpsTimeDM = std::clock();
	int fpsCountDM = 0;

	std::clock_t appStartTimeDM;
	float wakeUpGracePeriodDM = 2.0;
	float showLicensesGracePeriodDM = 1.0;

	HWND hwndPreview = nullptr;
	HDC hdcPreview = nullptr;
	HGLRC hglrcPreview = nullptr;


	void update(int value)
	{
		for (int curWindowIDIndex = 0; curWindowIDIndex < DisplayManager::getDisplayCount(); curWindowIDIndex++)
		{
			int curWindowID = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).GLUTWindowID;
			glutSetWindow(curWindowID);
			int currentWindowIndexIntoRainStorms = DisplayManager::getIndexFromGlutWindowID(curWindowID);
			if (currentWindowIndexIntoRainStorms == -1) { return; }
			// For debugging, exit when escape pressed.
			if (debugging) {
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { DisplayManager::exitScreenSaver(); }
			}
			DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).Update();
			// Trigger a redraw of everything
			glutPostRedisplay();
		}
		// Call self so the loop continues
		glutTimerFunc(25, update, 1);
	}

	size_t getDisplayCount() {
		return WindowIDs.size();
	}

	DisplayWindowInfo& getDisplayWindowInfo(int index) {
		return WindowIDs[index];
	}

	int getIndexFromGlutWindowID(int windowID) {
		for (int curWindowIDIndex = 0; curWindowIDIndex < DisplayManager::getDisplayCount(); curWindowIDIndex++)
		{
			int curWindowID = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).GLUTWindowID;
			if (windowID == curWindowID) {
				return curWindowIDIndex;
			}
		}
		return -1;
	}

	void addDisplayWindow(DisplayWindowInfo addThisDisplayWindow)
	{
		WindowIDs.push_back(addThisDisplayWindow);
	}

	void addRainStorm(RainStorm addThisRainStorm)
	{
		RainStorms.push_back(addThisRainStorm);
	}

	RainStorm& getDisplayWindowRain(int index) {
		return RainStorms[index];
	}

	void closeAllWindows() {
		for (int curWindowIDIndex = 0; curWindowIDIndex < getDisplayCount(); curWindowIDIndex++)
		{
			glutDestroyWindow(getDisplayWindowInfo(curWindowIDIndex).GLUTWindowID);
		}
	}

	void exitScreenSaver() {
		closeAllWindows();
		exit(0);
	}

	void makeWindowTopmost(HWND hWnd) {
		// Make the window topmost and remove decorations
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE); // WS_POPUP removes borders
	}

	HWND getWindowHandle(const std::string& windowTitle)
	{
		return FindWindowA(nullptr, windowTitle.c_str());
	}

	void changeSize(int w, int h)
	{
		int currentWindow = glutGetWindow();
		int curWindowIDIndex = getIndexFromGlutWindowID(currentWindow);
		if (curWindowIDIndex == -1) { return; }

		getDisplayWindowInfo(curWindowIDIndex).width = w;
		getDisplayWindowInfo(curWindowIDIndex).height = h;

		GLfloat fAspect;

		// Prevent a divide by zero, since you cant make a window of zero width.
		if (h == 0) { h = 1; }

		glViewport(0, 0, w, h);

		fAspect = (GLfloat)w / (GLfloat)h;
		// This specifies the current operating matrix.
		// That is the matrix that your matrix functions are affecting
		glMatrixMode(GL_PROJECTION);
		// This loads the current matrix, which is the GL_PROJECTION, with the identity matrix.
		// It basically means no transformations are performed
		glLoadIdentity();

		// Creates a viewing frustum
		float fov = getDisplayWindowInfo(curWindowIDIndex).fov;
		float nearPlane = getDisplayWindowInfo(curWindowIDIndex).nearPlane;
		float farPlane = getDisplayWindowInfo(curWindowIDIndex).farPlane;
		gluPerspective(fov, fAspect, 1.0f, farPlane);

		// Think of the ModelView matrix as where you stand with the camera and the direction you point it.
		glMatrixMode(GL_MODELVIEW);
		// This loads the current matrix, which is the GL_MODELVIEW, with the identity matrix. It basically means no transformations are performed.
		// In essence, we are resetting the modelview matrix to the origin
		glLoadIdentity();
	}

	void renderScene(void)
	{
		int currentWindow = glutGetWindow();
		int currentWindowIndexIntoRainStorms = DisplayManager::getIndexFromGlutWindowID(currentWindow);
		if (currentWindowIndexIntoRainStorms == -1) { return; }

		glPushMatrix();
		// Camera
		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.ApplyCameraTransform();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render
		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).Render();
		glPopMatrix();

		// Only show licenses for specific time
		bool shouldRenderLicenses = (std::clock() - appStartTimeDM) / static_cast<double>(CLOCKS_PER_SEC) < showLicensesGracePeriodDM;
		if (shouldRenderLicenses || debugging) {
			DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).RenderControlPanel(debugging);
		}

		glutSwapBuffers();
		// Finish so we can count the frames
		glFinish();

		double countASecTimeDif;
		countASecTimeDif = (std::clock() - fpsTimeDM) / (double)CLOCKS_PER_SEC;
		if (countASecTimeDif >= 1)
		{
			DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).fpsCount = fpsCountDM;
			// Reset the clock
			fpsTimeDM = std::clock();
			fpsCountDM = -1;
		}
		fpsCountDM++;
	}

	// Callback function for each monitor
	BOOL CALLBACK MonitorEnumProc(
		HMONITOR hMonitor,
		HDC hdcMonitor,
		LPRECT lprcMonitor,
		LPARAM dwData)
	{
		int* monitorCount = (int*)dwData;
		int nextIndex = getDisplayCount();
		std::stringstream windowTitleStream;
		windowTitleStream << "DigitalRainShowers_" << nextIndex;
		std::string windowTitle = windowTitleStream.str();

		// Create a FreeGLUT window for each monitor
		int windowID = glutCreateWindow(windowTitle.c_str());

		DisplayWindowInfo rememberThisWindow;
		rememberThisWindow.WindowTitle = windowTitle;
		rememberThisWindow.WindowHandle = getWindowHandle(windowTitle);
		rememberThisWindow.hMonitor = hMonitor;
		rememberThisWindow.GLUTWindowID = windowID;

		rememberThisWindow.width = lprcMonitor->right - lprcMonitor->left;
		rememberThisWindow.height = lprcMonitor->bottom - lprcMonitor->top;

		rememberThisWindow.fov = 45.0f;
		rememberThisWindow.nearPlane = 1.0f;
		rememberThisWindow.farPlane = 30000;

		// Save the window ID
		addDisplayWindow(rememberThisWindow);

		// Set window position and size based on the monitor's dimensions
		glutPositionWindow(lprcMonitor->left, lprcMonitor->top);
		glutReshapeWindow(lprcMonitor->right - lprcMonitor->left, lprcMonitor->bottom - lprcMonitor->top);

		// This ensures window sits above everything, and no titlebar appears
		// Comment below to debug easier
		makeWindowTopmost(rememberThisWindow.WindowHandle);

		// Resize
		glutReshapeFunc(changeSize);

		// Keyboard
		glutSpecialFunc(Input::handleSpecialKeys);
		glutKeyboardFunc(Input::handleKeypress);

		// Mousedown and move
		glutMotionFunc(Input::mouseStillDownMotion);
		glutMouseFunc(Input::MouseClicksnWheelButton);
		glutPassiveMotionFunc(Input::onCursorMove); //detect cursor movement

		// RenderScene will determine which window called it by calling glutGetWindow()
		glutDisplayFunc(renderScene);

		// Hide the cursor
		glutSetCursor(GLUT_CURSOR_NONE);

		(*monitorCount)++;
		return TRUE;
	}

	void setupRC()
	{
		// Set the display mode for all subsequent windows created
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glShadeModel(GL_FLAT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set clear color to black
		// Draw filled
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glFrontFace(GL_CW);
	}

	void CreateWindowsOnMonitors()
	{
		setupRC();

		float textWidth = 32;
		float textHeight = 32;
		int monitorCount = 0;
		//Create Windows for each monitor
		EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitorCount));

		// Loop through each created window and setup a RainStorm for it
		for (int curWindowIDIndex = 0; curWindowIDIndex < DisplayManager::getDisplayCount(); curWindowIDIndex++) {
			int curWindowID = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).GLUTWindowID;

			glutSetWindow(curWindowID);

			float screenWidth = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).width;
			float screenHeight = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).height;
			float fov = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).fov;
			float nearPlane = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).nearPlane;

			// Create and Add RainStorm for the current window
			RainStorm aMonitorStorm =
				RainStorm(
					screenWidth,
					screenHeight,
					textWidth,
					textHeight,
					fov, nearPlane);
			DisplayManager::addRainStorm(aMonitorStorm);
		}

		return;
	}

	bool CreatePreviewOnWindow(HWND hwnd)
	{
		DisplayManager::hwndPreview = hwnd;
		DisplayManager::hdcPreview = GetDC(DisplayManager::hwndPreview);

		// Define pixel format for OpenGL rendering
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,
			8,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		int pixelFormat = ChoosePixelFormat(DisplayManager::hdcPreview, &pfd);
		SetPixelFormat(DisplayManager::hdcPreview, pixelFormat, &pfd);

		// Create and make current OpenGL rendering context
		DisplayManager::hglrcPreview = wglCreateContext(DisplayManager::hdcPreview);
		wglMakeCurrent(DisplayManager::hdcPreview, DisplayManager::hglrcPreview);


		RECT rect;
		GetClientRect(DisplayManager::hwndPreview, &rect);

		setupRC();

		int monitorCount = 0;

		std::stringstream windowTitleStream;
		windowTitleStream << "DigitalRainShowers_Preview";
		std::string windowTitle = windowTitleStream.str();

		DisplayWindowInfo rememberThisWindow;
		rememberThisWindow.WindowTitle = windowTitle;
		rememberThisWindow.WindowHandle = hwnd;

		rememberThisWindow.width = rect.right - rect.left;
		rememberThisWindow.height = rect.bottom - rect.top;

		rememberThisWindow.fov = 45.0f;
		rememberThisWindow.nearPlane = 1.0f;
		rememberThisWindow.farPlane = 30000;

		// Save the window ID
		addDisplayWindow(rememberThisWindow);

		// Show 60 raindrops
		float textWidth = rememberThisWindow.width / 10.0;
		float textHeight = textWidth;

		// Loop through each created window and setup a RainStorm for it
		for (int curWindowIDIndex = 0; curWindowIDIndex < DisplayManager::getDisplayCount(); curWindowIDIndex++) {
			float screenWidth = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).width;
			float screenHeight = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).height;
			float fov = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).fov;
			float nearPlane = DisplayManager::getDisplayWindowInfo(curWindowIDIndex).nearPlane;

			// Create and Add RainStorm for the current window
			RainStorm aMonitorStorm =
				RainStorm(
					screenWidth,
					screenHeight,
					textWidth,
					textHeight,
					fov,
					nearPlane);
			DisplayManager::addRainStorm(aMonitorStorm);
		}

		long width = rect.right - rect.left;
		long height = rect.bottom - rect.top;
		glViewport(0, 0, width, height);

		float fAspect = (GLfloat)width / (GLfloat)height;
		// This specifies the current operating matrix.
		// That is the matrix that your matrix functions are affecting
		glMatrixMode(GL_PROJECTION);
		// This loads the current matrix, which is the GL_PROJECTION, with the identity matrix.
		// It basically means no transformations are performed
		glLoadIdentity();

		// Creates a viewing frustum
		float fov = DisplayManager::getDisplayWindowInfo(0).fov;
		float nearPlane = DisplayManager::getDisplayWindowInfo(0).nearPlane;
		float farPlane = DisplayManager::getDisplayWindowInfo(0).farPlane;
		gluPerspective(fov, fAspect, 1.0f, farPlane);

		// Think of the ModelView matrix as where you stand with the camera and the direction you point it.
		glMatrixMode(GL_MODELVIEW);
		// This loads the current matrix, which is the GL_MODELVIEW, with the identity matrix. It basically means no transformations are performed.
		// In essence, we are resetting the modelview matrix to the origin
		glLoadIdentity();

		return true;
	}

	void renderPreviewScene(void)
	{
		glPushMatrix();
		// Camera, get the first one
		DisplayManager::getDisplayWindowRain(0).graphCameraTwo.ApplyCameraTransform();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render, get the first one
		DisplayManager::getDisplayWindowRain(0).Render();
		glPopMatrix();

		// Only show licenses for specific time
		bool shouldRenderLicenses = (std::clock() - appStartTimeDM) / static_cast<double>(CLOCKS_PER_SEC) < showLicensesGracePeriodDM;
		if (shouldRenderLicenses || debugging) {
			DisplayManager::getDisplayWindowRain(0).RenderControlPanel(debugging);
		}

		// Swap the buffers to display the result
		SwapBuffers(DisplayManager::hdcPreview);

		// Finish so we can count the frames
		glFinish();

		double countASecTimeDif;
		countASecTimeDif = (std::clock() - fpsTimeDM) / (double)CLOCKS_PER_SEC;
		if (countASecTimeDif >= 1)
		{
			DisplayManager::getDisplayWindowRain(0).fpsCount = fpsCountDM;
			// Reset the clock
			fpsTimeDM = std::clock();
			fpsCountDM = -1;
		}
		fpsCountDM++;
	}

	void previewMainLoop(HWND hwnd)
	{
		// Continuously render the scene in preview mode
		while (true) {

			// Ensure the preview window is still present
			if (!IsWindow(hwnd)) {
				break;
			}
			else
			{
				// Check if window is not visible
				// This can happen if another screen saver is chosen
				if (!IsWindowVisible(hwnd))
				{
					PostQuitMessage(0);
					break;
				}
			}

			// Check for any messages to handle (optional, but useful in a Windows environment)
			MSG msg;
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				// Handle system command messages, such as close commands
				if (msg.message == WM_SYSCOMMAND)
				{
					// I never saw this get sent
					if ((msg.wParam & 0xFFF0) == SC_CLOSE || (msg.wParam & 0xFFF0) == SC_SCREENSAVE)
					{
						PostQuitMessage(0);
						break;
					}
				}
				// Exit on standard close, quit, or destroy messages
				if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY) {
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			DisplayManager::getDisplayWindowRain(0).Update();

			// Render the scene
			renderPreviewScene();
		}

		// Cleanup resources
		if (DisplayManager::hdcPreview) {
			ReleaseDC(DisplayManager::hwndPreview, DisplayManager::hdcPreview);
			DisplayManager::hdcPreview = NULL;
		}
		if (DisplayManager::hglrcPreview) {
			wglDeleteContext(DisplayManager::hglrcPreview);
			DisplayManager::hglrcPreview = NULL;
		}
	}

}