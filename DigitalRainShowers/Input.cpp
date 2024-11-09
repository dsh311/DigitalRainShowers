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

#include "../Dependencies/include/glm/glm.hpp"
#include <ctime>
#include "Input.h"
#include "DisplayManager.h"

namespace Input
{
	std::clock_t appStartTimeInput = std::clock();
	float wakeUpGracePeriod = 2.0;
	// Used to exit when mouse moves
	glm::vec3 lastMouseLoc(-1.0f, -1.0f, -1.0f);
	bool lastMouseLocRecorded = false;

	void onCursorMove(int x, int y)
	{
		float timePassed = (std::clock() - appStartTimeInput) / static_cast<double>(CLOCKS_PER_SEC);
		if (timePassed < wakeUpGracePeriod) {
			return;
		}

		if (lastMouseLocRecorded == false)
		{
			lastMouseLoc.x = x;
			lastMouseLoc.y = y;
			lastMouseLocRecorded = true;
		}
		else
		{
			if (lastMouseLoc.x != x && lastMouseLoc.y != y) {
				if (debugging != true) {
					DisplayManager::exitScreenSaver();
					return;
				}
			}
			lastMouseLoc.x = x;
			lastMouseLoc.y = y;
		}
	}


	// For the mousewheel, this gets fired with the down and up state immediatly for every movement of the wheel, so it happens twice
	void MouseClicksnWheelButton(int button, int state, int x, int y)
	{
		if (debugging != true) {
			DisplayManager::exitScreenSaver();
			return;
		}
		int currentWindow = glutGetWindow();
		int currentWindowIndexIntoRainStorms = DisplayManager::getIndexFromGlutWindowID(currentWindow);
		if (currentWindowIndexIntoRainStorms == -1) { return; }

		if (state == 0)
		{
			//If it is the left mouse button
			if (button == 0 || button == 2)
			{
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).mouseState = 0; //0 means mouse down
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).mouseButton = 0; //0 means left mouse button
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedX = x;
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedX = x;
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedZ = 1;
			} //If left mouse button

			float zoomDistance = DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).theRadius / 20;

			//If the mouse wheel pushed forward
			if (button == 3)
			{
				//The reason we don't call MoveForward is can't tell what type of graph this is and it resets the camera loc to the origin
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.MoveForward(zoomDistance);
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.MoveForward(zoomDistance);
			}
			//If the mouse wheel pulled back
			if (button == 4)
			{
				//The reason we don't call MoveForward is can't tell what type of graph this is and it resets the camera loc to the origin
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.MoveForward(-zoomDistance);
				DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.MoveForward(-zoomDistance);
			}
		}

		//Once the mouse button is up then reset the global button state
		if (state == 1 && DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).mouseState == 0)
		{
			DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).mouseState = 1; //1 means mouse up
		}
	}


	void mouseStillDownMotion(int x, int y)
	{
		if (debugging != true) {
			DisplayManager::exitScreenSaver();
			return;
		}

		int currentWindow = glutGetWindow();

		int currentWindowIndexIntoRainStorms = DisplayManager::getIndexFromGlutWindowID(currentWindow);
		if (currentWindowIndexIntoRainStorms == -1) { return; }


		float fieldOfView = 45;
		float nearPlane = -1000;

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		float aspectRatio = viewport[2] / viewport[3];

		int xDif = x - DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedX;
		int yDif = y - DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedY;

		double normal_xDif = double(xDif) / (double(viewport[3]) / 2);
		double normal_yDif = double(yDif) / (double(viewport[3]) / 2);
		float pi = 3.141592;
		// Trying to calculate how much they moved their mouse
		float halfNearPlaneHeight = nearPlane * tan(((fieldOfView / 2) * pi / 180.0)); //Convert to radians
		int xDistIsRadian = (halfNearPlaneHeight * aspectRatio) * normal_xDif;
		int yDistIsRadian = halfNearPlaneHeight * normal_yDif;

		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.MoveUp(-yDistIsRadian);
		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).graphCameraTwo.MoveRight(xDistIsRadian);
		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedX = x;
		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedY = y;
		DisplayManager::getDisplayWindowRain(currentWindowIndexIntoRainStorms).clickedZ = 1;
		
		glutPostRedisplay();
	}

	void handleKeypress(unsigned char key, int x, int y)
	{
		if ((std::clock() - appStartTimeInput) / static_cast<double>(CLOCKS_PER_SEC) < wakeUpGracePeriod) {
			return;
		}

		if (debugging != true) {
			DisplayManager::exitScreenSaver();
			return;
		}
	}

	// Respond to arrow keys by moving the camera frame of reference
	void handleSpecialKeys(int key, int x, int y)
	{
		if ((std::clock() - appStartTimeInput) / static_cast<double>(CLOCKS_PER_SEC) < wakeUpGracePeriod) {
			return;
		}

		if (debugging != true) {
			DisplayManager::exitScreenSaver();
			return;
		}
	}
}