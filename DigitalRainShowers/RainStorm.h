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
#include<vector>
#include "RainDrop.h"
#include "Camera.h"
#include<string>
#include <ctime> 


class RainStorm
{
public:

	GLuint g_textureIDs[50];

	int fpsCount;

	// Used by RenderControlPanel
	int winWidth;
	int winHeight;
	int winDepth;

	int graphWidth;
	int graphHeight;
	int graphDepth;


	float theRadius;

	Camera graphCameraTwo;

	glm::vec3 picPointOnSphere;

	// Used by MouseStillDownMotion
	float clickedX;
	float clickedY;
	float clickedZ;

	int mouseState;
	int mouseButton;

	void RotateLocalY(float angle);
	void RotateLocalX(float angle);
	void MoveForward(float angle);

	std::vector <RainDrop> RainDrop_Vector;

	// Color use with glClearColor
	glm::vec4 bkgndColor;

	RainStorm();
	RainStorm(float screenWidth, float screenHeight, float textWidth, float textHeight, float fov, float nearField);

	bool loadMatrixTextures(int numTextures);
	bool loadTextureFromResourceID(int resourceID, int textureIndex);	

	void RenderControlPanel(bool);

	void Update();
	void Render(void);

	void DrawLabel(float textHeight, float textWidth, glm::vec3 theLoc, glm::vec3 rightVec, glm::vec3 upVec, std::string theLabel);
};
