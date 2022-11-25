#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


enum CameraDirectionCommand 
{
	CameraForwardDirection,
	CameraBackwardDirection,
	CameraLeftDirection,
	CameraRightDirection
};

using glm::vec3;
using glm::vec2;
using glm::lookAt;
using glm::radians;

class Camera
{
	public:

		Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f) : cameraFront(vec3(0.0f, 0.0f, -5.0f)), cameraSpeed(2000.0f), cameraSensitivity(0.1f), cameraZoom(60.0f)
		{
			cameraPosition = position;
			worldUp = up;
			cameraYaw = yaw;
			cameraPitch = pitch;
			updateCameraVectors();
		}
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : cameraFront(vec3(0.0f, 0.0f, -1.0f)), cameraSpeed(2000.0f), cameraSensitivity(0.1f), cameraZoom(60.0f)
		{
			cameraPosition = vec3(posX, posY, posZ);
			worldUp = vec3(upX, upY, upZ);
			cameraYaw = yaw;
			cameraPitch = pitch;
			updateCameraVectors();
		}



		
		void ProcessKeyboard(CameraDirectionCommand direction, float deltaTime)
		{
			float velocity = cameraSpeed * deltaTime;
			if (direction == CameraForwardDirection)
			{
				cameraPosition += cameraFront * velocity;
			}
			if (direction == CameraBackwardDirection)
			{
				cameraPosition -= cameraFront * velocity;
			}
			if (direction == CameraLeftDirection)
			{
				cameraPosition -= cameraRight * velocity;
			}	
			if (direction == CameraRightDirection)
			{
				cameraPosition += cameraRight * velocity;
			}
		}
		float Random2D(vec2 st) // a standard way in shaders of generating "random" values
		{
			return glm::fract(glm::sin(glm::dot(st, vec2(12.9898f, 78.233f))) * 43758.5453123f);
		}
		glm::mat4 GetViewMatrix()
		{
			updateCameraVectors();
			return lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		}
		float InterpolatedNoise(int ind, float x, float y) 
		{
			float integer_X = floor(x);
			float fractional_X = glm::fract(x);
			float integer_Y = floor(y);
			float fractional_Y = glm::fract(y);
			vec2 randomInput = vec2(integer_X, integer_Y);
			float a = Random2D(randomInput);
			float b = Random2D(randomInput + vec2(1.0, 0.0));
			float c = Random2D(randomInput + vec2(0.0, 1.0));
			float d = Random2D(randomInput + vec2(1.0, 1.0));

			vec2 w = vec2(fractional_X, fractional_Y);

			w = w * w * w * (10.0f + w * (-15.0f + 6.0f*w));
			return a + (b - a) * w.x + (c - a) * w.y + (d - c - b + a) * w.x * w.y; // Don't ask me what this means - David.
		}
		// orientating vectors
		vec3 cameraPosition;
		vec3 cameraFront;
		vec3 cameraUp;
		vec3 cameraRight;
		vec3 worldUp;

		float cameraYaw; // Angles
		float cameraPitch; // Angles

	
		float cameraSpeed;
		float cameraSensitivity;
		float cameraZoom;


		float perlin(vec2 st, int planeOctaves, float freq, float gDispFactor) 
		{
			glm::mat2 m;
			m[0][0] = 0.8;
			m[0][1] = -0.6;
			m[1][0] = 0.6;
			m[1][1] = 0.8;
			float persistence = 0.5;
			float total = 0.0;
			float planeFrequencyFactor = 0.005 * freq;
			float amplitude = gDispFactor; // gravity disposition
			for (int i = 0; i < planeOctaves; ++i) 
			{
				planeFrequencyFactor *= 2.0;
				amplitude *= persistence;

				vec2 v = planeFrequencyFactor * st;

				total += InterpolatedNoise(0, v.x, v.y) * amplitude;
			}
			
			return glm::pow(total, 3.0f);
		}

		void projectCameraPosition() 
		{
			vec2 st = vec2(cameraPosition.x,cameraPosition.z);
			cameraPosition.y = perlin(st, 13, 0.01, 20.);
		}

		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
		{
			xoffset *= cameraSensitivity;
			yoffset *= cameraSensitivity;

			cameraYaw += xoffset;
			cameraPitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (cameraPitch > 89.0f)
				{
					cameraPitch = 89.0f;
				}
				if (cameraPitch < -89.0f)
				{
					cameraPitch = -89.0f;
				}	
			}
			updateCameraVectors();
		}

		void ProcessMouseScroll(float yoffset)
		{
			if (cameraZoom >= 1.0f && cameraZoom <= 100.0f)
			{
				cameraZoom -= yoffset;
			}
			cameraZoom = glm::clamp(cameraZoom, 1.0f, 100.0f);	
		}

		void invertPitch() 
		{
			this->cameraPitch = -cameraPitch;
			updateCameraVectors();
		}

	private:

		void updateCameraVectors()
		{
			using glm::normalize;
			using glm::cross;
			vec3 front;
			front.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
			front.y = sin(radians(cameraPitch));
			front.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
			cameraFront = normalize(front);
			cameraRight = normalize(cross(cameraFront, worldUp));
			cameraUp = normalize(cross(cameraRight, cameraFront));
		}
};
#endif