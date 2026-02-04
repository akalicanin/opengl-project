#ifndef _MOJA_CAMERA_H_
#define _MOJA_CAMERA_H_

#include <string>
#include "glad/glad.h"
#include "glm/glm.hpp"

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum CameraType {
	CAM_FPS,
	CAM_TRUE_FPS
};

//default values
const float YAW		= -90.0f;
const float PITCH	= 0.0f;
const float FOV		= 45.0f;
const float SPEED	= 15.0f;
const float ZOOMS	= 1.5f;
const float SENSX	= 0.2f;
const float SENSY   = 0.2f;

class Camera {
public:

	CameraType type;

	glm::vec3 cameraPos;
	glm::vec3 cameraForwardDirection;
	glm::vec3 cameraUp;
	glm::vec3 worldUp;
	glm::vec3 cameraRight;

	float yaw;
	float pitch;
	float fov;

	float cameraSpeed;
	float zoomSpeed;
	float mouseSensitivityX;
	float mouseSensitivityY;


	Camera(glm::vec3 cameraPosition, CameraType type, glm::vec3 worldUp = glm::vec3(0, 1, 0),
		float yaw = YAW, float pitch = PITCH, float fov = FOV, float speed = SPEED, float zoomspeed = ZOOMS, float mouseSensX = SENSX, float mouseSensY = SENSY);

	// returns a view matrix based on all camera data
	glm::mat4 buildViewMatrix() const;

	// process movement of the mouse
	void processMouseMovement(float xoffset, float yoffset);

	// process mouse scrolling
	void processMouseScroll(float yoffset);

	// actions depending on what key is pressed. use processkeyboardCAMmovement instead
	void processKeyboardMovement(CameraMovement Direction, float deltaTime);

	// pomeranje koje je iste brzine i po dijagonalama. koristiti ovo umesto processkeyboardmovement
	void processKeyboardCamMovement(bool forward, bool backward, bool left, bool right, float deltaTime);

private:
	
	void updateCameraVectors();

};


#endif
