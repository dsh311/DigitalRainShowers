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
#include <windows.h>
#include <GL/gl.h>
#include "../Dependencies/include/glm/vec3.hpp" // glm::vec3
#include "../Dependencies/include/glm/matrix.hpp"
#include "../Dependencies/include/glm/glm.hpp"
#include "../Dependencies/include/glm/gtc/matrix_transform.hpp"
#include "../Dependencies/include/glm/gtx/rotate_vector.hpp"
#include "../Dependencies/include/glm/gtc/type_ptr.hpp"

class Camera {
private:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    float pitch, yaw, roll;

    bool bRotOnly;

public:
    float tetherLength;

    // Apply the camera transformation, similar to GLFrame's method
    void ApplyCameraTransform() {
        // Create the orientation matrix
        glm::mat4 orientation = glm::lookAt(position, position + forward, up);

        // Convert the glm matrix to a format OpenGL can use (column-major order)
        GLfloat m[16];
        const float* mat = (const float*)glm::value_ptr(orientation);
        for (int i = 0; i < 16; ++i) {
            m[i] = mat[i];
        }

        // Apply the orientation using OpenGL
        glMultMatrixf(m);

        // If not rotation only, apply translation
        if (!bRotOnly) {
            glTranslatef(-position.x, -position.y, -position.z);
        }
    }

    void SetRotationOnly(bool flag) {
        bRotOnly = flag;
    }

    // Get the right (X) axis vector
    void GetXAxis(glm::vec3& xAxis) const {
        xAxis = right;
    }

    glm::vec3& GetPosition() {
        return position;
    }

    // Get the up (Y) axis vector
    void GetUpVector(glm::vec3& upVector) const {
        upVector = up;
    }

    // Set the origin (camera position)
    void SetOrigin(float x, float y, float z) {
        position = glm::vec3(x, y, z);
    }

    void SetOrigin(const glm::vec3& origin) {
        position = origin;
    }

    void SetForward(const glm::vec3& useThisForward) {
        forward = glm::normalize(useThisForward);
        right = glm::normalize(glm::cross(forward, up));
        up = glm::cross(right, forward);  // Ensure orthogonality
    }

    void SetUp(const glm::vec3& useThisUp) {
        up = glm::normalize(useThisUp);
    }

    // Rotate camera around local X-axis (pitch)
    void RotateLocalX(float angle) {
        pitch += glm::radians(angle);
        forward = glm::rotate(forward, glm::radians(angle), right);
        up = glm::cross(right, forward); // Recalculate up
    }

    // Rotate camera around local Y-axis (yaw)
    void RotateLocalY(float angle) {
        yaw += glm::radians(angle);
        forward = glm::rotateY(forward, glm::radians(angle));
        right = glm::cross(forward, up); // Recalculate right
    }

    // Rotate camera around local Z-axis (roll)
    void RotateLocalZ(float angle) {
        roll += glm::radians(angle);
        right = glm::rotate(right, glm::radians(angle), forward);
        up = glm::cross(right, forward); // Recalculate up
    }

    // Move the camera along the right (local X-axis)
    void MoveRight(float distance) {
        position += right * distance;
    }

    // Move the camera along the forward (local Z-axis)
    void MoveForward(float distance) {
        position += forward * distance;
    }

    // Move the camera along the up (local Y-axis)
    void MoveUp(float distance) {
        position += up * distance;
    }

    // Get the camera's current position
    void GetOrigin(glm::vec3& origin) const {
        origin = position;
    }
};

