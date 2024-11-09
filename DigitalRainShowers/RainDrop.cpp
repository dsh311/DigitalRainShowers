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

#include "RainDrop.h"
#define FREEGLUT_STATIC
#include "..\Dependencies\include\GL\glut.h"
#include "../Dependencies/include/glm/vec3.hpp" // glm::vec3
#include "../Dependencies/include/glm/glm.hpp"
#include "../Dependencies/include/glm/gtc/type_ptr.hpp"

//This is the default constructor
RainDrop::RainDrop()
{
	id = -1;
	nextCharIndex = 0;

	curCyclingCharIndex = 0;

	theString = "";

	topPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	upVec = glm::vec3(0.0f, 0.0f, 0.0f);
	rightVec = glm::vec3(0.0f, 0.0f, 0.0f);
}

void RainDrop::renderGlyphTexture(
	int textIndex,
	float textHeight,
	float textWidth,
	glm::vec3 theLoc,
	glm::vec3 rightVec,
	glm::vec3 upVec,
	GLuint* textureIDs
)
{
	glBindTexture(GL_TEXTURE_2D, textureIDs[textIndex]);
	glEnable(GL_TEXTURE_2D);

	float halfTextWidthSize = textWidth * 0.5f;
	float halfTextHeightSize = textHeight * 0.5f;

	// Multiplicaion changes itself
	glm::vec3 toTheLeft(glm::normalize(rightVec) * -1.0f * halfTextWidthSize);
	glm::vec3 toTheRight(glm::normalize(rightVec) * halfTextWidthSize);
	glm::vec3 toTheUp(glm::normalize(upVec) * halfTextHeightSize);
	glm::vec3 toTheDown(glm::normalize(upVec) * -1.0f * halfTextHeightSize);
	glm::vec3 topLeft = theLoc + toTheLeft + toTheUp;
	glm::vec3 botLeft = theLoc + toTheLeft + toTheDown;
	glm::vec3 topRight = theLoc + toTheRight + toTheUp;
	glm::vec3 botRight = theLoc + toTheRight + toTheDown;

	glMatrixMode(GL_MODELVIEW);

	// A Single face
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3fv(glm::value_ptr(topLeft));
	glTexCoord2f(1.0f, 1.0f); glVertex3fv(glm::value_ptr(topRight));
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(glm::value_ptr(botRight));
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(glm::value_ptr(botLeft));
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


float RainDrop::getRandomWaitTime() {
	float values[] = { 0.15f, 0.10f, 0.05f, 0.02f, 0.01f };
	int numValues = sizeof(values) / sizeof(values[0]);
	int index = rand() % numValues;  // Generate a random index: 0, 1, or 2
	return values[index];
}

void RainDrop::randomScrambleString(std::string& scrambleMe)
{
	int swapLoc2 = 0;
	char tempChar;
	for (int swapLoc = 0; swapLoc < scrambleMe.size(); swapLoc++)
	{
		swapLoc2 = rand() % (scrambleMe.length() - 1);
		tempChar = scrambleMe[swapLoc2];
		scrambleMe[swapLoc2] = scrambleMe[swapLoc];
		scrambleMe[swapLoc] = tempChar;
	}
}

std::string RainDrop::generateRandomString(const std::string& letters, int length) {
	if (letters.empty() || length <= 0) {
		return "";  // Return an empty string if input is invalid
	}

	std::string result;
	result.reserve(length);
	for (int i = 0; i < length; ++i) {
		int randomIndex = std::rand() % letters.size();
		result += letters[randomIndex];
	}
	return result;
}


//This simple updates the x,y location of the ball based on the incrementing values which are set elsewhere
void RainDrop::Update()
{
	// Update this single stream when enough time has passed
	bool shouldUpdate = ((std::clock() - lastRenderTime) / (double)CLOCKS_PER_SEC) > waitRenderTimeInSeconds;
	if (shouldUpdate)
	{
		// If all the characters are drawn then we need to keep looping until the first drawn character goes past the end of the screen
		// If we finished looping through all of the characters
		// If (aGraph::aRainStream_Vector[curStream].nextCharIndex + 1 >= aGraph::aRainStream_Vector[curStream].theString.size())
		// By adding the maxShownChars, we wait for the last characters to pass the end of the screen
		// Simply doing if (nextCharIndex + 1 >= theString.size()) resets when first char goes past screen
		if (nextCharIndex + 1 >= theString.size() + maxShownChars)
		{
			nextCharIndex = 0;
			// Reset the wait render time
			waitRenderTimeInSeconds = getRandomWaitTime();

			// Determine how far forward or back from camera the streams appear
			// Randomize the z value, result between 0 and 1999 
			// topPosition.z = (rand() % 2000);
			topPosition.z = 0;

			//topPosition.z = topPosition.z + 50.0f;

			randomScrambleString(theString);

			// Randomly choose a choose a character to cycle
			curCyclingCharIndex = (rand() % (theString.length() - 1));

			// Reset the maximum characters shown in the column
			maxShownChars = (rand() % theString.length());
			if (maxShownChars < 7) { maxShownChars = 7; }
		}
		else
		{
			//topPosition.z = topPosition.z + 5.0f; // Zooming effect
			nextCharIndex = nextCharIndex + 1;
		}

		// Reset the clock
		lastRenderTime = std::clock();
	}
}

void RainDrop::Render(GLuint* textureIDs)
{
	glm::vec3 theCharLoc(0.0f, 0.0f, 0.0f);

	int startChar = 0;
	float verticalSpacing = 0;
	
	std::string theChar;

	// This is used to vary the shades of green
	float theRedPercent = 0;
	float theGreenPercent = 1;
	float theBluePercent = 0;
	char theActualChar;

	glm::vec3 upVector(0.0f, 1.0f, 0.0f);
	glm::vec3 rightVector(1.0f, 0.0f, 0.0f);

	// Figure out where to start drawing by subtracting the max number of characters to be shown by the next character we want to draw
	// We can show at most maxShownChars, so, what is the next index
	// nextCharIndex = 0 to begin with;
	
	startChar = nextCharIndex - maxShownChars;
	if (startChar < 0) { startChar = 0; }

	// Loop through all characters that are currently visiable
	for (int curCharIndex = startChar; curCharIndex <= nextCharIndex; curCharIndex++)
	{
		// NOTE: The top positions are fixed from initialization
		theCharLoc.x = topPosition.x;
		// Calculate the yLoc based on the current character
		theCharLoc.y = topPosition.y - (curCharIndex * (textHeight + verticalSpacing));

		theCharLoc.z = topPosition.z; //We don't change

		// Get the next character to draw
		// theChar = aGraph::aRainStream_Vector[curStream].theString[aGraph::aRainStream_Vector[curStream].nextCharIndex];
		// Only get this if it isn't past the boundary
		if (curCharIndex < theString.size())
		{
			theChar = theString[curCharIndex];

			// Set the color based on how far it is from the current character
			// If this is the nextCharIndex then make it the brightest
			// theGreenPercent = ((curChar + 1) / (aGraph::aRainStream_Vector[curStream].nextCharIndex + 1)); //Add 1 so the first character won't be black

			theRedPercent = 0.0f;
			theBluePercent = 0.0f;

			float middleMan = 0.0f;
			// If this char is the nextCharIndex then make is whiter than the others
			if (curCharIndex == nextCharIndex)
			{
				theGreenPercent = 1.0f;
				theRedPercent = 0.7f;
				theBluePercent = 0.7f;
				
				// Draw a larger version behind it
				// glColor4f(theRedPercent,theGreenPercent,theBluePercent,0.2f);
				// aGraph::DrawLabel(textHeight + 10,textWidth+10,theCharLoc,rightVector,upVector,theChar);

				theRedPercent = 0.5f;
				theBluePercent = 0.5f;
				middleMan = 1.0f;

			}
			else
			{
				middleMan = 1.0f;
				// Vary the red and blue together
				theRedPercent = 0.0f;
				theBluePercent = 0.0f;

				float thePlaceOnMaxShowScale(maxShownChars - (nextCharIndex - curCharIndex));
				theGreenPercent = thePlaceOnMaxShowScale / maxShownChars;
			}


			// This should be white for the texture
			// glColor4f(1.0f,1.0f,1.0f,theGreenPercent);
			glColor4f(theRedPercent, 1.0f, theBluePercent, theGreenPercent);
			glm::vec3 adjustedLoc = theCharLoc;
			adjustedLoc.y = adjustedLoc.y + (textHeight / 2.0f);

			// If this is the cycling character then change it's character
			if (curCharIndex == curCyclingCharIndex)
			{
				theChar = theString[(rand() % (theString.length() - 1))];
			}

			//Choose the texture from the letter, a = 1, b = 2, etc...
			theActualChar = theChar[0];
			if (theActualChar >= 97)
			{
				theActualChar = theActualChar - 96;
			}
			else
			{
				if (theActualChar >= 65) {
					theActualChar = theActualChar - 38;
				}
			}

			glColor4f(theRedPercent, 1.0f, theBluePercent, (theGreenPercent - 0.08));
			renderGlyphTexture(
				theActualChar,
				textHeight,
				textWidth, 
				adjustedLoc,
				rightVector,
				upVector,
				textureIDs);

		} // End if the current char has an index into theString. Some curChar won't since this allows for the first drawn character to fall off the screen

	} // End looping through characters in the curStream
}
