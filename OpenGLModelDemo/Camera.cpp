#include "Camera.h"

#include <iostream>
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 cameraPosition, CameraType type, glm::vec3 worldUp, float yaw, float pitch, float fov, float speed, float zoomspeed, float mouseSensX, float mouseSensY) {
	this->cameraPos = cameraPosition;
	this->worldUp = worldUp;
	this->fov = fov;
	this->yaw = yaw;
	this->pitch = pitch;
	this->cameraSpeed = speed;
	this->zoomSpeed = zoomspeed;
	this->mouseSensitivityX = mouseSensX;
	this->mouseSensitivityY = mouseSensY;
	this->type = type;
	updateCameraVectors();
}

glm::mat4 Camera::buildViewMatrix() const {
	glm::vec3 cameraTarget = this->cameraPos + this->cameraForwardDirection;
	return glm::lookAt(this->cameraPos, cameraTarget, glm::vec3(0, 1, 0));
}

void Camera::processMouseMovement(float xoffset, float yoffset) {

	xoffset *= this->mouseSensitivityX;
	yoffset *= this->mouseSensitivityY;

	this->yaw += xoffset;
	this->pitch += yoffset;

	if (this->pitch > 89.0f) {
		this->pitch = 89.0f;
	}
	else if (this->pitch < -89.0f) {
		this->pitch = -89.0f;
	}

	if (this->yaw > 360.0f || this->yaw < -360.0f) {
		this->yaw = 0.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
	this->fov -= zoomSpeed * yoffset;
}

void Camera::processKeyboardMovement(CameraMovement Direction, float deltaTime) {

	float velocity = this->cameraSpeed * deltaTime;

	if (this->type == CAM_FPS) {
		if (Direction == FORWARD) {
			this->cameraPos += velocity * this->cameraForwardDirection;
		}
		if (Direction == BACKWARD) {
			this->cameraPos -= velocity * this->cameraForwardDirection;
		}
		if (Direction == LEFT) {
			this->cameraPos -= velocity * this->cameraRight;
		}
		if (Direction == RIGHT) {
			this->cameraPos += velocity * this->cameraRight;
		}
	}

	if (this->type == CAM_TRUE_FPS) {

		glm::vec3 trueFpsForward = glm::normalize(glm::vec3(this->cameraForwardDirection.x, 0.0f, this->cameraForwardDirection.z));
		glm::vec3 trueFpsRight = glm::normalize(glm::cross(trueFpsForward, this->worldUp));

		if (Direction == FORWARD) {
			this->cameraPos += velocity * trueFpsForward;
		}
		if (Direction == BACKWARD) {
			this->cameraPos -= velocity * trueFpsForward;
		}
		if (Direction == LEFT) {
			this->cameraPos -= velocity * trueFpsRight;
		}
		if (Direction == RIGHT) {
			this->cameraPos += velocity * trueFpsRight;
		}
	}
	
}

void Camera::processKeyboardCamMovement(bool forward, bool backward, bool left, bool right, float deltaTime) {

	float velocity = this->cameraSpeed * deltaTime;
	glm::vec3 direction = glm::vec3(0);

	if (this->type == CAM_FPS) {
		if (forward) {
			direction += this->cameraForwardDirection;
		}
		if (backward) {
			direction -= this->cameraForwardDirection;
		}
		if (left) {
			direction -= this->cameraRight;
		}
		if (right) {
			direction += this->cameraRight;
		}
	}

	if (this->type == CAM_TRUE_FPS) {

		glm::vec3 trueFpsForward = glm::normalize(glm::vec3(this->cameraForwardDirection.x, 0.0f, this->cameraForwardDirection.z));
		glm::vec3 trueFpsRight = glm::normalize(glm::cross(trueFpsForward, this->worldUp));

		if (forward) {
			direction += trueFpsForward;
		}
		if (backward) {
			direction -= trueFpsForward;
		}
		if (left) {
			direction -= trueFpsRight;
		}
		if (right) {
			direction += trueFpsRight;
		}
	}

	if (direction != glm::vec3(0)) {
		direction = glm::normalize(direction);
	}
	this->cameraPos += velocity * direction;
}


void Camera::updateCameraVectors() {
	glm::vec3 frontDirection = glm::vec3(1);
	frontDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontDirection.y = sin(glm::radians(pitch));
	frontDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->cameraForwardDirection = glm::normalize(frontDirection);
	this->cameraRight = glm::normalize(glm::cross(this->cameraForwardDirection, this->worldUp));
	this->cameraUp = glm::normalize(glm::cross(this->cameraRight, this->cameraForwardDirection));
}
