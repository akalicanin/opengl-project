// OpenGL/GLFW Include
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"

// GLM Include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

// Foreign Library Include
#include "stb_image.h"

// C++ Include
#include <iostream>
#include <string>

// Personal Include
#include "Shader.h"
#include "Camera.h"

// Callback Declaration
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Function Forward Declaration
void processInput(GLFWwindow* window);
void processMovement(GLFWwindow* window);
void changeColors(int& colorstate);

// Global Variables
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// Frametime
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// Camera
Camera mainCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), CAM_FPS);
bool firstMouseMovement = true;
float lastX = 0.0f;
float lastY = 0.0f;

// Function-specific Global Variables
float mixAlpha;
int colorState;
bool spacePressed;

typedef unsigned int uint;

int main() {

	// GLFW Initialization
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Title", NULL, NULL);
	if (!window) {
		std::cerr << "Window je null!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // <- uzima iz OS-a sve opengl f-je

	// set callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetScrollCallback(window, scrollCallback);

	int window_width, window_height;
	glfwGetFramebufferSize(window, &window_width, &window_height);
	glViewport(0, 0, window_width, window_height);
	glfwSwapInterval(1);

	// omogucava koriscenje transparentnih tekstura
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// omogucava dubinsko testiranje
	glEnable(GL_DEPTH_TEST);

	// Input mode radi kamere kako ne bi mis izlazio van ekrana.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float kockaTacke[] = {
		// koordinate			tex. koordinate

		// prednja strana kocke
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,

		//zadnja strana kocke
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, -0.5f,-0.5f,		1.0f, 0.0f,

		//gornja
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,

		//donja
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,

		//leva
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 	0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f,

		//desno
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 		0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f

	};

	uint kockaRedosled[] = {
		//prednja
		0, 1, 2,
		0, 2, 3,
		//zadnja
		4, 5, 6,
		4, 6, 7,
		//gornja
		8, 9, 10,
		8, 10, 11,
		//donja
		12, 13, 14,
		12, 14, 15,
		//leva
		16, 17, 18,
		16, 18, 19,
		//desna
		20, 21, 22,
		20, 22, 23,
	};

	uint kockaVAO, kockaVBO, kockaEBO;

	glGenVertexArrays(1, &kockaVAO);
	glBindVertexArray(kockaVAO);
	glGenBuffers(1, &kockaVBO);
	glGenBuffers(1, &kockaEBO);
	glBindBuffer(GL_ARRAY_BUFFER, kockaVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kockaTacke), kockaTacke, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kockaEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kockaRedosled), kockaRedosled, GL_STATIC_DRAW);

	// Ucitavanje tekstura
	int texWidth, texHeight, texNumberOfChannels;
	unsigned char* texData;
	stbi_set_flip_vertically_on_load(true);

	uint kutijaTekstura, asTekstura;
	glGenTextures(1, &kutijaTekstura);
	glBindTexture(GL_TEXTURE_2D, kutijaTekstura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texData = stbi_load("textures/csbox.png", &texWidth, &texHeight, &texNumberOfChannels, 0);
	if (!texData) {
		std::cerr << "Failed to load texture!" << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texData);

	glGenTextures(1, &asTekstura);
	glBindTexture(GL_TEXTURE_2D, asTekstura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texData = stbi_load("textures/ace.png", &texWidth, &texHeight, &texNumberOfChannels, 0);
	if (!texData) {
		std::cerr << "Failed to load texture!" << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbinding je opcionalan uvek

	Shader* kockaShader = new Shader("shaders/kocka.vs", "shaders/kocka.fs");
	kockaShader->use();
	kockaShader->setInt("u_tex_Kutija", 0);
	kockaShader->setInt("u_tex_As", 1);

	glm::quat startQ = glm::angleAxis(glm::radians(0.0f), glm::vec3(0, sqrt(3) / 2, 0.5f));
	glm::quat endQ = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, sqrt(3) / 2, 0.5f));
	glm::quat rotateQ = startQ;

	float startTime = -1.0f;
	float animDuration = 0.50f; // 1 sekunda
	// vrednost 0,1,2,3
	// na 1 i 3 swap start i end quaternion
	int animStateCounter = 0;

	glm::vec3 locationList[] = {
		glm::vec3(-3.5f, 0.3f, 0.1f),
		glm::vec3(3.6f, 0.8f, -2.3f),
		glm::vec3(0.1f, 3.1f, -1.0f),
		glm::vec3(0.3f, -3.9f, -6.0f),
		glm::vec3(-2.6f, -2.0f, -9.0f)
	};

	colorState = 1;
	mixAlpha = 0.0f;

	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		changeColors(colorState);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		float vreme = (float)glfwGetTime();
		float vrednost = sin(2 * vreme) * glm::pi<float>() / 2;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, kutijaTekstura);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, asTekstura);

		glm::mat4 translationMatrix;

		// za naginjanje kocke napred.
		glm::quat rotationQuaternion = glm::angleAxis(glm::radians(-60.0f), glm::vec3(1, 0, 0));
		glm::vec3 newZaxis = rotationQuaternion * glm::vec3(0, 0, 1);

		// animacija kocke
		if (spacePressed && startTime < 0.0f) { // da li nema animacije i kliknuli smo space?
			startTime = vreme; // pokreni animaciju na trenutno vreme
		}
		if (startTime >= 0.0f) { // ako je pokrenuta animacija
			float interpolateT = (vreme - startTime) / animDuration; // izracunaj faktor interpolacije, koliko je ostalo do kraja
			if (interpolateT >= 1.0f) { // ako smo 1 ili presli 1, znaci dosli smo do kraja, animacija je gotova
				interpolateT = 0.0f;
				startTime = -1.0f; // prekidamo sa animacijom
				animStateCounter++;
				animStateCounter %= 8;
				switch (animStateCounter) {  // priprema startQ i endQ za sledecu animaciju.
				case 0:
					startQ = glm::angleAxis(glm::radians(0.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(90.0f), newZaxis);
					break;
				case 1:
					startQ = glm::angleAxis(glm::radians(90.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(180.0f), newZaxis);
					break;
				case 2:
					startQ = glm::angleAxis(glm::radians(180.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(90.0f), newZaxis);
					break;
				case 3:
					startQ = glm::angleAxis(glm::radians(90.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(0.0f), newZaxis);
					break;
				case 4:
					startQ = glm::angleAxis(glm::radians(0.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(-90.0f), newZaxis);
					break;
				case 5:
					startQ = glm::angleAxis(glm::radians(-90.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(-180.0f), newZaxis);
					break;
				case 6:
					startQ = glm::angleAxis(glm::radians(-180.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(-90.0f), newZaxis);
					break;
				case 7:
					startQ = glm::angleAxis(glm::radians(-90.0f), newZaxis);
					endQ = glm::angleAxis(glm::radians(0.0f), newZaxis);
					break;
				}
			}
			rotateQ = glm::slerp(startQ, endQ, interpolateT);
		}
		glm::quat finalRotateQuaternion = rotateQ * rotationQuaternion;
		glm::mat4 rotationMatrix = glm::toMat4(finalRotateQuaternion);

		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1), glm::vec3(0.7f, 0.7f, 0.7f));

		glm::mat4 transformMatrix = rotationMatrix * scalingMatrix;

		glm::mat4 viewMatrix = mainCamera.buildViewMatrix();


		glm::mat4 projectionMatrix = glm::perspective(glm::radians(mainCamera.fov), (float)window_width / (float)window_height, 0.1f, 100.0f);


		kockaShader->use();
		kockaShader->setMat4("u_Model", transformMatrix);
		kockaShader->setMat4("u_View", viewMatrix);
		kockaShader->setMat4("u_Projection", projectionMatrix);
		kockaShader->setFloat("u_MixAmount", mixAlpha);

		glBindVertexArray(kockaVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


		translationMatrix = glm::mat4(1);

		glm::quat offsetRotateX;
		glm::quat offsetRotateY;
		glm::quat offsetRotateZ;
		glm::quat offsetRotate;
		glm::quat spinRotate;
		glm::quat finalRotate;

		for (int i = 0; i <= 4; i++) {
			translationMatrix = glm::translate(glm::mat4(1), locationList[i]);
			offsetRotateX = glm::angleAxis((float)glm::radians(pow(-1, i) * i * 3), glm::vec3(1, 0, 0));
			offsetRotateY = glm::angleAxis((float)glm::radians(pow(-1, i + 1) * i * 3.6f), glm::vec3(0, 1, 0));
			offsetRotateZ = glm::angleAxis((float)glm::radians(pow(-1, i) * i * 2.4f), glm::vec3(0, 0, 1));
			offsetRotate = offsetRotateX * offsetRotateY * offsetRotateZ;
			glm::vec3 newYaxis = offsetRotate * glm::vec3(0, 1, 0);
			spinRotate = glm::angleAxis(vrednost, newYaxis);
			finalRotate = spinRotate * offsetRotate;
			rotationMatrix = glm::toMat4(finalRotate);
			transformMatrix = translationMatrix * rotationMatrix * scalingMatrix;
			kockaShader->setMat4("u_Model", transformMatrix);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {

	// Za zatvaranje prozora
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	processMovement(window);

	// Za animaciju
	if ((glfwGetKey(window, GLFW_KEY_SPACE)) == GLFW_PRESS) {
		spacePressed = true;
	}

	if ((glfwGetKey(window, GLFW_KEY_SPACE)) == GLFW_RELEASE) {
		spacePressed = false;
	}

	// Za mesanje alpha vrednosti
	if ((glfwGetKey(window, GLFW_KEY_UP)) == GLFW_PRESS) {
		mixAlpha += 0.1f;
	}
	else if ((glfwGetKey(window, GLFW_KEY_DOWN)) == GLFW_PRESS) {
		mixAlpha -= 0.1f;
	}

	if (mixAlpha >= 1.0f) {
		mixAlpha = 1.0f;
	}
	else if (mixAlpha <= -1.0f) {
		mixAlpha = -1.0f;
	}

	// Za promenu boje pozadine
	if ((glfwGetKey(window, GLFW_KEY_U)) == GLFW_PRESS) {
		colorState = 1;
	}
	else if ((glfwGetKey(window, GLFW_KEY_I)) == GLFW_PRESS) {
		colorState = 2;
	}
	else if ((glfwGetKey(window, GLFW_KEY_O)) == GLFW_PRESS) {
		colorState = 3;
	}
	else if ((glfwGetKey(window, GLFW_KEY_P)) == GLFW_PRESS) {
		colorState = 4;
	}
}

// kretanje, pokriva i dijagonalni slucaj
void processMovement(GLFWwindow* window) {

	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;

	if ((glfwGetKey(window, GLFW_KEY_W)) == GLFW_PRESS) {
		forward = true;
	}
	else {
		forward = false;
	}
	if ((glfwGetKey(window, GLFW_KEY_S)) == GLFW_PRESS) {
		backward = true;
	}
	else {
		backward = false;
	}
	if ((glfwGetKey(window, GLFW_KEY_A)) == GLFW_PRESS) {
		left = true;
	}
	else {
		left = false;
	}
	if ((glfwGetKey(window, GLFW_KEY_D)) == GLFW_PRESS) {
		right = true;
	}
	else {
		right = false;
	}

	mainCamera.processKeyboardCamMovement(forward, backward, left, right, deltaTime);

	/*
	if ((glfwGetKey(window, GLFW_KEY_W)) == GLFW_PRESS) {
		mainCamera.processKeyboardMovement(FORWARD, deltaTime);
	}
	if ((glfwGetKey(window, GLFW_KEY_S)) == GLFW_PRESS) {
		mainCamera.processKeyboardMovement(BACKWARD, deltaTime);
	}
	if ((glfwGetKey(window, GLFW_KEY_A)) == GLFW_PRESS) {
		mainCamera.processKeyboardMovement(LEFT, deltaTime);
	}
	if ((glfwGetKey(window, GLFW_KEY_D)) == GLFW_PRESS) {
		mainCamera.processKeyboardMovement(RIGHT, deltaTime);
	}
	*/
}

//Prati lokaciju misa
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouseMovement) {
		lastX = xpos;
		lastY = ypos;
		firstMouseMovement = false;
	}

	float offsetX = xpos - lastX;
	float offsetY = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	mainCamera.processMouseMovement(offsetX, offsetY);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	mainCamera.processMouseScroll(static_cast<float>(yoffset));
}

// Menja viewport velicinu
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	std::cout << "[CB] Framebuffer velicina promenjena na: " << width << ":"
		<< height << std::endl;
	glViewport(0, 0, width, height);
}

// Menja boju ekrana po pritisku Q,W,E,R
void changeColors(int& colorState) {

	switch (colorState) {
	case 1:
		glClearColor(0.2f, 0, 0, 1);
		break;
	case 2:
		glClearColor(0, 0.2f, 0, 1);
		break;
	case 3:
		glClearColor(0, 0, 0.2f, 1);
		break;
	case 4:
		glClearColor(1, 1, 1, 1);
		break;
	default:
		colorState = 1;
		glClearColor(0.2f, 0, 0, 1);
		break;
	}

}

