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

#include <windows.h>
#include "resource.h"
#include "RainStorm.h"
#include<sstream>
#include <iostream>
#include <fstream>
#include <float.h>
#include "dFont.h"
#define FREEGLUT_STATIC
#include "..\Dependencies\include\GL\glut.h"
#include "../Dependencies/include/glm/vec3.hpp" // glm::vec3

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//This is the default constructor
RainStorm::RainStorm()
{
	// Used by RenderControlPanel
	winWidth = 0;
	winHeight = 0;
	winDepth = 0;

	// Used by MouseStillDownMotion
	clickedX = 0.0f;
	clickedY = 0.0f;
	clickedZ = 0.0f;

	mouseState = -1;
	mouseButton = -1;

	theRadius = 100;
}

RainStorm::RainStorm(
	float screenWidth,
	float screenHeight,
	float textWidth,
	float textHeight,
	float fov,
	float nearPlaneDistance)
{
	winWidth = screenWidth; // Set this or 2D rendering won't work properly
	winHeight = screenHeight; // Set this or 2D rendering won't work properly

	graphCameraTwo.tetherLength = 0;

	float aspectRatio = screenWidth / screenHeight; // Example aspect ratio for widescreen

	// Convert FOV to radians
	double fovVertRadians = fov * (M_PI / 180.0);

	// Calculate the half-height and then width at the near plane
	double halfHeight = std::tan(fovVertRadians / 2.0) * nearPlaneDistance;
	double halfWidth = halfHeight * aspectRatio;
	double wholeWidth = halfWidth * 2.0;
	double wholeHeight = wholeWidth * aspectRatio;

	double fovHorzRadians = fov * (M_PI / 180.0);
	double halfWidth2 = std::tan(fovHorzRadians / 2.0) * nearPlaneDistance;
	double wholeWidth2 = halfWidth * 2.0;
	double wholeHeight2 = wholeWidth2 / aspectRatio;

	glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);
	graphCameraTwo.SetUp(upVec);

	// Forward is -Z since moving away from camera is on -Z
	glm::vec3 forwardVec = glm::vec3(0.0f, 0.0f, -1.0f);
	graphCameraTwo.SetForward(forwardVec);

	double angleDegrees = fov / 2.0;
	double angleRadians = (angleDegrees * (M_PI / 180.0));
	double tanOfRandians = std::tan(angleRadians);

	float halfScreenHeight = screenHeight / 2.0;
	float adjacentSideLength = halfScreenHeight / tanOfRandians;


	// Setup the camera position
	theRadius = adjacentSideLength;
	graphCameraTwo.SetOrigin(0.0f, 0.0f, theRadius);


	int xSpacing = 5;
	int maxNumRowsThatFitOnScreen = (screenHeight / textHeight) * 2;
	// The bottom is 1 off so add 1
	maxNumRowsThatFitOnScreen += 1;
	int numColumns = (screenWidth / textWidth) * 2;

	int numberOfTextures = 26;
	bool itWorked = loadMatrixTextures(numberOfTextures); //26 goes from 0 to 26)

	float halfXDist = (textWidth * ((numColumns / 2.0f)));
	glm::vec3 streamUpVector(1.0f, 0.0f, 0.0f);
	glm::vec3 streamRightVector(0.0f, -1.0f, 0.0f);


	for (int curX = 0; curX < numColumns; curX++)
	{
		RainDrop tempRain;

		std:string theLetters = "abcdefghijklmnopqrstuvwxyz";
		tempRain.theString = tempRain.generateRandomString(theLetters, maxNumRowsThatFitOnScreen);

		tempRain.textWidth = textWidth;
		tempRain.textHeight = textHeight;

		tempRain.upVec = streamUpVector;
		tempRain.rightVec = streamRightVector;

		// When curX is 0, this is minus halfXDist so on the left side
		// It slowly moves towards center then goes positive on right side
		tempRain.topPosition.x = ((curX * textWidth)) - halfXDist; // Make sure to center the rain over x-axis
		tempRain.topPosition.y = screenHeight;
		tempRain.lastRenderTime = 0;
		tempRain.waitRenderTimeInSeconds = tempRain.getRandomWaitTime();


		// First time through, show all characters in the column
		// this will randomly be changed each iteration, inside Update
		// Start with random lengths
		tempRain.maxShownChars = (rand() % tempRain.theString.length());
		if (tempRain.maxShownChars < 7) { tempRain.maxShownChars = 7; }

		// Make sure they don't all start on 0, otherwise they all start together.
		tempRain.nextCharIndex = (rand() % maxNumRowsThatFitOnScreen);

		RainDrop_Vector.push_back(tempRain);
	}
}

void RainStorm::RotateLocalY(float incAngle)
{
	RainStorm::graphCameraTwo.RotateLocalY(incAngle);
}

void RainStorm::RotateLocalX(float incAngle)
{
	RainStorm::graphCameraTwo.RotateLocalX(incAngle);
}

void RainStorm::MoveForward(float incAngle)
{
	RainStorm::graphCameraTwo.MoveForward(incAngle);
}

void RainStorm::DrawLabel(float textHeight, float textWidth, glm::vec3 theLoc, glm::vec3 rightVec, glm::vec3 upVec, std::string theLabel)
{
	char theChar;

	glm::vec3 curLoc = theLoc;
	glm::vec3 nextStart;
	nextStart.x = rightVec.x * textWidth;
	nextStart.y = rightVec.y * textWidth;
	nextStart.z = rightVec.z * textWidth;


	glBegin(GL_QUADS);

	for (int charIndex = 0; charIndex < theLabel.size(); charIndex++)
	{
		// Get the character
		theChar = theLabel[charIndex];

		// Check if it is a tab or return key
		drawChar(textHeight, textWidth, curLoc, rightVec, upVec, theChar);
		curLoc.x = curLoc.x + (rightVec.x * textWidth);
		curLoc.y = curLoc.y + (rightVec.y * textWidth);
		curLoc.z = curLoc.z + (rightVec.z * textWidth);
	}

	glEnd();
}

void RainStorm::RenderControlPanel(bool isDebug)
{

	// Step 1, Select the projection matrix so we can change it
	glMatrixMode(GL_PROJECTION);

	// Step 2, Save the current projection matrix wich is the perspective projection matrix so we can restore it later
	// Each mode has its own stack so this will push on the projection stack
	glPushMatrix();

	// Step 3, Reset the current projection matrix so we can create a new Orthographic projection
	glLoadIdentity();
	// Step 4, //Create a new orthographic viewing volume
	gluOrtho2D(0, RainStorm::winWidth, 0, RainStorm::winHeight);


	// Select, save and reset the modelview matrix.
	// Modelview matrix stack store transformation like translation, rotation ...
	glMatrixMode(GL_MODELVIEW);
	//This will push on the modelview stack since each mode has its own stack
	glPushMatrix();
	glLoadIdentity();

	float textH = 5.0f;
	float textW = 5.0f;
	// Draw FPS
	glm::vec3 rightVector(1.0f, 0.0f, 0.0f);
	glm::vec3 upVector(0.0f, 1.0f, 0.0f);

	std::string nextTextString;
	nextTextString = "David S. Shelley";
	glm::vec3 nextTextLocVector(0.0f, 30.0f, 0.0f);
	RainStorm::DrawLabel(textH, textW, nextTextLocVector, rightVector, upVector, nextTextString);

	nextTextString = "OpenGL";
	nextTextLocVector = glm::vec3(0.0f, 20.0f, 0.0f);
	RainStorm::DrawLabel(textH, textW, nextTextLocVector, rightVector, upVector, nextTextString);

	nextTextString = "OpenGL Mathematics (GLM) - Copyright (c) 2005 - G-Truc Creation";
	nextTextLocVector = glm::vec3(0.0f, 10.0f, 0.0f);
	RainStorm::DrawLabel(textH, textW, nextTextLocVector, rightVector, upVector, nextTextString);

	nextTextString = "Freeglut Copyright       - Copyright (c) 1999-2000 Pawel W. Olszta";
	nextTextLocVector = glm::vec3(0.0f, 0.0f, 0.0f);
	RainStorm::DrawLabel(textH, textW, nextTextLocVector, rightVector, upVector, nextTextString);

	if (isDebug) {
		std::string fpsString;
		fpsString = std::to_string(RainStorm::fpsCount);
		fpsString.insert(0, "fps: ");
		glm::vec3 locVector(0.0f, 90.0f, 0.0f);
		RainStorm::DrawLabel(textH, textW, locVector, rightVector, upVector, fpsString);


		int width = glutGet(GLUT_WINDOW_WIDTH);
		std::string widthString;
		widthString = std::to_string(width);
		widthString.insert(0, "width: ");
		glm::vec3 locVectorWidth(0.0f, 120.0f, 0.0f);
		RainStorm::DrawLabel(textH, textW, locVectorWidth, rightVector, upVector, widthString);

		int height = glutGet(GLUT_WINDOW_HEIGHT);
		std::string heightString;
		heightString = std::to_string(height);
		heightString.insert(0, "height: ");
		glm::vec3 locVectorHeight(0.0f, 150.0f, 0.0f);
		RainStorm::DrawLabel(textH, textW, locVectorHeight, rightVector, upVector, heightString);
	}

	float fAspect = (GLfloat)RainStorm::winWidth / (GLfloat)RainStorm::winHeight;
	// This specifies the current operating matrix. That is the matrix that your matrix functions are affecting
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, fAspect, 1.0f, 30000.0f);
	// Think of the ModelView matrix as where you stand with the camera and the direction you point it.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Restore previous projection
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Restore previous model view
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


bool RainStorm::loadMatrixTextures(int numTextures)
{
	bool returnValue = true;
	std::string s;
	std::stringstream out;

	for (int i = 1; i <= numTextures; ++i)
	{
		int resourceID = IDB_BITMAP1 + (i - 1);
		bool itWorked = loadTextureFromResourceID(resourceID, i);
		if (!itWorked) {
			returnValue = false;
		}
	}
	return returnValue;
}

std::string getExecutablePathOnWindows() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string executablePath(buffer);
	size_t pos = executablePath.find_last_of("\\/");
	return executablePath.substr(0, pos); // Returns the directory path
}


unsigned char* LoadImageResource(int resourceID, int& dataSize) {
	HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(resourceID), RT_BITMAP);
	if (!hResource)
	{
		return nullptr;
	}

	HGLOBAL hMemory = LoadResource(nullptr, hResource);
	if (!hMemory) return nullptr;

	dataSize = SizeofResource(nullptr, hResource);
	return static_cast<unsigned char*>(LockResource(hMemory));
}

// Load the bitmap directly and get its data
unsigned char* LoadBitmapResource(int resourceID, int& width, int& height, int& channels)
{
	HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(resourceID), RT_BITMAP);
	if (!hResource) return nullptr;

	HGLOBAL hMemory = LoadResource(nullptr, hResource);
	if (!hMemory) return nullptr;

	BITMAPINFO* bitmapInfo = static_cast<BITMAPINFO*>(LockResource(hMemory));
	if (!bitmapInfo) return nullptr;

	// Get width, height, and color channels
	width = bitmapInfo->bmiHeader.biWidth;
	height = abs(bitmapInfo->bmiHeader.biHeight); // Bitmap height can be negative
	channels = bitmapInfo->bmiHeader.biBitCount / 8;

	// Access raw pixel data offset by BITMAPINFOHEADER size
	unsigned char* data = reinterpret_cast<unsigned char*>(bitmapInfo) + sizeof(BITMAPINFOHEADER);
	return data;
}

bool RainStorm::loadTextureFromResourceID(int resourceID, int textureIndex)
{
	bool returnValue = false;

	// Generate and bind the texture
	glGenTextures(1, &g_textureIDs[textureIndex]);
	glBindTexture(GL_TEXTURE_2D, g_textureIDs[textureIndex]);

	// Load the image
	int resourceSize = 0;
	int width, height, channels;
	unsigned char* data = LoadBitmapResource(resourceID, width, height, channels);
	
	if (data) {
		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload the texture data
		glTexImage2D(GL_TEXTURE_2D, 0, (channels == 4 ? GL_RGBA : GL_RGB), width, height, 0,
			(channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
		returnValue = true;
	}
	else {
		std::cerr << "Failed to load texture wiht ID: " << resourceID << std::endl;
		returnValue = false;
	}

	// No need to free 'data' since LoadResourceand LockResource use memory
	// that is managed by the operating system.

	return returnValue;
}

void RainStorm::Update()
{
	for (int curStream = 0; curStream < RainStorm::RainDrop_Vector.size(); curStream++)
	{
		RainStorm::RainDrop_Vector[curStream].Update();
	}
}

void RainStorm::Render(void)
{
	// Clear last drawn picture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // This makes it tons brighter than the one below
	glEnable(GL_POINT_SMOOTH);
	glShadeModel(GL_FLAT);

	// Loop through each rainStream and render it
	for (int curStream = 0; curStream < RainStorm::RainDrop_Vector.size(); curStream++)
	{
		RainStorm::RainDrop_Vector[curStream].Render(g_textureIDs);
	} 

}

