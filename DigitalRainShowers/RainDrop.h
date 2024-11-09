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
#include<string>
#include <ctime> 
#include "..\Dependencies\include\GL\glut.h"
#include "../Dependencies/include/glm/vec3.hpp" // glm::vec3

using namespace std;

class RainDrop
{
public:
	int id;
	int degree;
	float nextCharIndex;
	float textWidth;
	float textHeight;

	int maxShownChars;

	int curCyclingCharIndex;

	// Used to determine if this RainDrop should get another character
	std::clock_t lastRenderTime;
	float waitRenderTimeInSeconds;

	std::string theString;
	glm::vec3 topPosition;
	glm::vec3 upVec;
	glm::vec3 rightVec;

	RainDrop();

	void renderGlyphTexture(
		int textIndex,
		float textHeight,
		float textWidth,
		glm::vec3 theLoc,
		glm::vec3 rightVec,
		glm::vec3 upVec,
		GLuint* textureIDs);

	float getRandomWaitTime();
	void randomScrambleString(std::string& theS);
	std::string generateRandomString(const std::string& letters, int length);
	void Update();
	void Render(GLuint* textureIDs);

};
