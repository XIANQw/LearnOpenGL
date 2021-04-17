#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera {
	
	
	glm::vec3 cameraUp;
	glm::vec3 worldUp;
	float cameraSpeed;
	/*yaw is initialized to -90.0 degrees since a yaw of 0.0 results 
	in a direction vector pointing to the right 
	so we initially rotate a bit to the left.*/
	float yaw;	
	float pitch;
	

public:
	glm::vec3 cameraFront;
	glm::vec3 cameraPos;
	float zoom;

	Camera() : 
		cameraPos(glm::vec3(0, 0.5, 3)),
		worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		cameraSpeed(2.5f), yaw(YAW), pitch(PITCH), zoom(ZOOM) {
		updateCameraFront();
	}

	void setCameraSpeed(float speed) {
		cameraSpeed = speed;
	}

	float getZoom() {
		return zoom;
	}

	void move(Camera_movement direct) {
		switch (direct)
		{
		case FORWARD:
			cameraPos += cameraSpeed * cameraFront;
			break;
		case BACKWARD:
			cameraPos -= cameraSpeed * cameraFront;
			break;
		case LEFT:
			cameraPos -= glm::normalize(glm::cross(cameraFront, worldUp)) * cameraSpeed;
			break;
		case RIGHT:
			cameraPos += glm::normalize(glm::cross(cameraFront, worldUp)) * cameraSpeed;
			break;
		default:
			break;
		}
	}

	glm::mat4 getLookAt() {
		//return glm::lookAt(cameraPos, cameraPos + cameraFront, worldUp);
		return computeViewMat(cameraPos, cameraPos + cameraFront, worldUp);
	}

	void updateYawPitch(float xoffset, float yoffset) {
		
		xoffset *= SENSITIVITY;
		yoffset *= SENSITIVITY;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		updateCameraFront();
	}

	void updateZoom(float yoffset) {
		zoom -= yoffset;
		zoom = std::max(zoom, 1.0f);
		zoom = std::min(zoom, 45.0f);
	}

private:
	void updateCameraFront() {
		cameraFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraFront.y = sin(glm::radians(pitch));
		cameraFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraFront = glm::normalize(cameraFront);
	}


	/*
	LookAt = [ xAxis,   0] * [ 1, 0, 0, -Px]
			 [ yAxis,   0]   [ 0, 1, 0, -Py]
			 [ zAxis,   0]	 [ 0, 0, 1, -Pz]
			 [ 0, 0, 0, 1]	 [ 0, 0, 0, 1  ]
	*/
	glm::mat4 computeViewMat(glm::vec3& cameraPos, glm::vec3&& targetPos, glm::vec3 worldUp) {
		glm::vec3 zAxis = glm::normalize(cameraPos - targetPos);
		glm::vec3 xAxis = glm::normalize(glm::cross(worldUp, zAxis));
		glm::vec3 yAxis = glm::normalize(glm::cross(zAxis, xAxis));
		// glm的4x4矩阵构造, 每个vec参数都是一个col, 所以要转置, 把col变成row
		glm::mat4 rotation(glm::vec4(xAxis, 0.0f), glm::vec4(yAxis, 0.0f), glm::vec4(zAxis, 0.0f), glm::vec4(glm::vec3(0.0f), 1.0f));
		rotation = glm::transpose(rotation);
		glm::mat4 translation = glm::identity<glm::mat4>();
		translation[3] = glm::vec4(-cameraPos, 1.0f);
		return rotation * translation;
	}
};
