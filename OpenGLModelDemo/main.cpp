// OpenGL/GLFW Include
#include "glad/glad.h"
#include "glfw3.h"
#include "KHR/khrplatform.h"

// GLM Include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

// ASSIMP Include
#include "Importer.hpp"

// Foreign Library Include
#include "stb_image.h"

// C++ Include
#include <iostream>
#include <string>
#include <fstream>

// Personal Include
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Callback Declaration
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Functions
	// General
unsigned int loadTexture(const char* texPath);

// Input processing
void processInput(GLFWwindow* window);
void processMovement(GLFWwindow* window);

// Zezanje functions
void changeColors(int& colorstate);

// Main Settings
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// Frametime
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// Camera
Camera mainCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), CAM_FPS);
bool firstMouseMovement = true;
float lastX = 0.0f;
float lastY = 0.0f;

// Key input checking for toggles
bool pressingR = false;
bool pressingF = false;

// Global Variables
int colorState = 1;
int currentMap = 1;

bool flashlightOn = false;
bool debugView = false;

typedef unsigned int uint;

int main() {

	// GLFW Initialization
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Model Viewer DEMO", NULL, NULL);
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

	//STBI za ucitavanja tekstura ucitava pravilno, kako OPENGLu odgovara
	stbi_set_flip_vertically_on_load(true);

	// Input mode radi kamere kako ne bi mis izlazio van ekrana.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ODAVDE KRECE PROGRAM


	// Defining cube - definisanje kocke
	float kockaTacke[] = {
		// koordinate			normale			tex. koordinate

		// prednja strana kocke
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,

		//zadnja strana kocke
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		0.5f, -0.5f,-0.5f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,

		//gornja
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,

		//donja
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f,

		//leva
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,

		//desno
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 		1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f
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
		20, 22, 23
	};

	uint kockaVAO, lightsourceVAO, kockaVBO, kockaEBO;

	glGenVertexArrays(1, &kockaVAO);
	glBindVertexArray(kockaVAO);
	glGenBuffers(1, &kockaVBO);
	glGenBuffers(1, &kockaEBO);
	glBindBuffer(GL_ARRAY_BUFFER, kockaVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kockaTacke), kockaTacke, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kockaEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kockaRedosled), kockaRedosled, GL_STATIC_DRAW);

	glGenVertexArrays(1, &lightsourceVAO);
	glBindVertexArray(lightsourceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, kockaVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kockaEBO);

	glBindVertexArray(0);

	// SHADER SETUP

	Shader* cubeShader = new Shader("shaders/lighting.vs", "shaders/lighting.fs");
	Shader* map2CubeShader = new Shader("shaders/lighting.vs", "shaders/lightingNoPoints.fs");
	Shader* lightsourceShader = new Shader("shaders/lightsource.vs", "shaders/lightsource.fs");

	cubeShader->use();
	cubeShader->setInt("material.texture_diffuse1", 0);
	cubeShader->setInt("material.texture_specular1", 1);
	cubeShader->setFloat("material.shininess", 32.0f);

	map2CubeShader->use();
	map2CubeShader->setInt("material.texture_diffuse1", 0);
	map2CubeShader->setInt("material.texture_specular1", 1);
	map2CubeShader->setFloat("material.shininess", 32.0f);

	// Ucitavanje tekstura
	uint boxDiffuse, dnkGreenDiff, dnkRedDiff, dnkSpec;

	boxDiffuse = loadTexture("textures/dosadnakutija.png");
	dnkGreenDiff = loadTexture("textures/dnkgreen.png");
	dnkRedDiff = loadTexture("textures/dnkred.png");
	dnkSpec = loadTexture("textures/dnkSPEC.png");

	Model torusConeModel("models/toruscone/torus.obj");
	Model backpackModel("models/backpack2/backpack.obj");

	while (!glfwWindowShouldClose(window)) {

		// SETUP , DELTATIME ITD

		processInput(window);
		changeColors(colorState);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float vreme = currentFrame;

		float fps = 1.0f / deltaTime;

		//std::cout << "FPS: " << fps << std::endl;

		// RENDEROVANJE

		glm::mat4 viewMatrix = mainCamera.buildViewMatrix();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(mainCamera.fov), (float)window_width / (float)window_height, 0.1f, 100.0f);

		
		// SWITCHING BETWEEN MAPS

		// DNK helix model with flashing lights circling
		if (currentMap == 1) 
		{

			lightsourceShader->use();
			lightsourceShader->setMat4("model", glm::mat4(1.0f));
			lightsourceShader->setMat4("view", viewMatrix);
			lightsourceShader->setMat4("projection", projectionMatrix);
			lightsourceShader->setVec3("lightColor", glm::vec3(1.0f));

			glm::mat4 modelMatrix = glm::mat4(1.0f);

			// CRTANJE I DEFINISANJE "NEONKI"

			std::string broj;
			std::string naziv;
			float distanceFactor = 0.25f;
			int counter = 0;
			int heights[] = {
				0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39
			};

			for (int i = 0; i < 28; i += 2) {
				counter = i / 2;

				glm::vec3 lokacija1 = glm::vec3(4.0f * sin(vreme + (heights[counter] * distanceFactor)), -30.0f + 1.25f * heights[counter], 4.0f * cos(vreme + (heights[counter] * distanceFactor)));
				glm::vec3 lokacija2 = glm::vec3(-4.0f * sin(vreme + (heights[counter] * distanceFactor)), -30.0f + 1.25f * heights[counter], -4.0f * cos(vreme + (heights[counter] * distanceFactor)));


				cubeShader->use();

				broj = std::to_string(i);
				naziv = "pointLights[" + broj + "]";
				cubeShader->setVec3((naziv + ".position").c_str(), glm::vec3(viewMatrix * glm::vec4(lokacija1, 1.0f)));
				cubeShader->setVec3((naziv + ".ambient").c_str(), 0.05f, 0.0f, 0.0f);
				cubeShader->setVec3((naziv + ".diffuse").c_str(), 0.8f, 0.0f, 0.0f);
				cubeShader->setVec3((naziv + ".specular").c_str(), 1.0f, 1.0f, 1.0f);
				cubeShader->setFloat((naziv + ".constant").c_str(), 1.0f);
				cubeShader->setFloat((naziv + ".linear").c_str(), 0.09f);
				cubeShader->setFloat((naziv + ".quadratic").c_str(), 0.032f);

				broj = std::to_string(i + 1);
				naziv = "pointLights[" + broj + "]";
				cubeShader->setVec3((naziv + ".position").c_str(), glm::vec3(viewMatrix * glm::vec4(lokacija2, 1.0f)));
				cubeShader->setVec3((naziv + ".ambient").c_str(), 0.0f, 0.05f, 0.0f);
				cubeShader->setVec3((naziv + ".diffuse").c_str(), 0.0f, 0.8f, 0.0f);
				cubeShader->setVec3((naziv + ".specular").c_str(), 1.0f, 1.0f, 1.0f);
				cubeShader->setFloat((naziv + ".constant").c_str(), 1.0f);
				cubeShader->setFloat((naziv + ".linear").c_str(), 0.09f);
				cubeShader->setFloat((naziv + ".quadratic").c_str(), 0.032f);


				// CRTANJE "NEONKI". Opcioni korak

				if (debugView) {
					lightsourceShader->use();

					glBindVertexArray(lightsourceVAO);

					lightsourceShader->setVec3("lightColor", glm::vec3(1.0f, 0.0f, 0.0f));

					modelMatrix = glm::translate(glm::mat4(1.0f), lokacija1);
					lightsourceShader->setMat4("model", modelMatrix);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

					lightsourceShader->setVec3("lightColor", glm::vec3(0.0f, 1.0f, 0.0f));

					modelMatrix = glm::translate(glm::mat4(1.0f), lokacija2);
					lightsourceShader->setMat4("model", modelMatrix);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


				}
			}

			//vracanje ostalih izvora svetlosti u belu boju
			lightsourceShader->use();
			lightsourceShader->setVec3("lightColor", glm::vec3(1.0f));

			// CRTANJE KOCKI

			cubeShader->use();
			cubeShader->setMat4("view", viewMatrix);
			cubeShader->setMat4("projection", projectionMatrix);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, boxDiffuse);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, dnkSpec);

			// directional light
			cubeShader->setVec3("directionLight.direction", -0.2f, -1.0f, -0.3f);
			cubeShader->setVec3("directionLight.ambient", 0.05f, 0.05f, 0.05f);
			cubeShader->setVec3("directionLight.diffuse", 0.4f, 0.4f, 0.4f);
			cubeShader->setVec3("directionLight.specular", 0.5f, 0.5f, 0.5f);
			// spotLight
			cubeShader->setVec3("spotLight.position", 0.0f, 0.0f, 0.0f);
			cubeShader->setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
			cubeShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			cubeShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			cubeShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			cubeShader->setFloat("spotLight.constant", 1.0f);
			cubeShader->setFloat("spotLight.linear", 0.09f);
			cubeShader->setFloat("spotLight.quadratic", 0.032f);
			cubeShader->setFloat("spotLight.innerCosAngle", glm::cos(glm::radians(12.5f)));
			cubeShader->setFloat("spotLight.outerCosAngle", glm::cos(glm::radians(15.0f)));
			cubeShader->setBool("spotLight.on", flashlightOn);

			glBindVertexArray(kockaVAO);

			float radius = 6.0f;
			glm::vec3 position1, position2;


			// rendering helix cubes (kocke)
			for (int i = 0; i < 40; i++) {
				glBindVertexArray(kockaVAO);
				cubeShader->use();

				if (i % 2) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, dnkRedDiff);

				}
				else {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, dnkGreenDiff);

				}

				position1 = glm::vec3(radius * sin(vreme + (i * distanceFactor)), -30.0f + 1.25f * i, radius * cos(vreme + (i * distanceFactor)));
				modelMatrix = glm::translate(glm::mat4(1.0f), position1);
				glm::quat rotationQuaternion;
				glm::mat4 rotationMatrix;
				glm::vec3 rotationAxis = normalize(glm::vec3(pow(-1, i) * i * 1.3f, 0.6f, -1.0f * pow(-1, i) * i * i * 0.3f));
				rotationQuaternion = glm::angleAxis(glm::radians(i * vreme * 2.8f), rotationAxis);
				rotationMatrix = glm::toMat4(rotationQuaternion);
				modelMatrix *= rotationMatrix;
				cubeShader->setMat4("model", modelMatrix);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

				position2 = glm::vec3(-radius * sin(vreme + (i * distanceFactor)), -30.0f + 1.25f * i, -radius * cos(vreme + (i * distanceFactor)));
				modelMatrix = glm::translate(glm::mat4(1.0f), position2);
				rotationAxis = normalize(glm::vec3(pow(-1, i) * i * 1.3f, 0.6f, -1.0f * pow(-1, i) * i * i * 0.3f));
				rotationQuaternion = glm::angleAxis(glm::radians(i * vreme * 2.8f), rotationAxis);
				rotationMatrix = glm::toMat4(rotationQuaternion);
				modelMatrix *= rotationMatrix;
				cubeShader->setMat4("model", modelMatrix);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

				float distanceBetweenSquares = 2 * radius;

				if (i % 3 == 0) {
					modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -30.0f + 1.25f * i, 0.0f));
					rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
					rotationQuaternion = glm::angleAxis(vreme + (i * distanceFactor) + glm::pi<float>() / 2, rotationAxis);
					rotationMatrix = glm::toMat4(rotationQuaternion);
					modelMatrix *= rotationMatrix;
					modelMatrix = glm::scale(modelMatrix, glm::vec3(distanceBetweenSquares, 0.5f, 0.5f));

					lightsourceShader->use();
					lightsourceShader->setMat4("model", modelMatrix);

					glBindVertexArray(lightsourceVAO);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				}
				else {
					continue;
				}
			}

		}
		else if (currentMap == 2) 
		{

			// CRTANJE KRUZNOG IZVORA SVETLA
			float radius = 5.0f;
			glm::vec3 lightcubePos = glm::vec3(radius * sin(vreme), 0.0f, radius * cos(vreme)) + glm::vec3(-1.0f, 2.0f, 2.0f);

			glm::vec3 lightColor = glm::vec3(1.0f);
			lightColor.x = sin(vreme);
			lightColor.y = sin(vreme + glm::pi<float>() * 4 / 3);
			lightColor.z = sin(vreme + glm::pi<float>() * 2 / 3);

			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), lightcubePos);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

			lightsourceShader->use();
			lightsourceShader->setMat4("model", modelMatrix);
			lightsourceShader->setMat4("view", viewMatrix);
			lightsourceShader->setMat4("projection", projectionMatrix);
			lightsourceShader->setVec3("lightColor", lightColor);

			glBindVertexArray(lightsourceVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			map2CubeShader->use();
			// directional light
			map2CubeShader->setVec3("directionLight.direction", -0.2f, -1.0f, -0.3f);
			map2CubeShader->setVec3("directionLight.ambient", 0.05f, 0.05f, 0.05f);
			map2CubeShader->setVec3("directionLight.diffuse", 0.4f, 0.4f, 0.4f);
			map2CubeShader->setVec3("directionLight.specular", 0.5f, 0.5f, 0.5f);
			// point light kruzni
			map2CubeShader->setVec3("pointLights[0].position", glm::vec3(viewMatrix * glm::vec4(lightcubePos, 1.0f)));
			map2CubeShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			map2CubeShader->setVec3("pointLights[0].diffuse", lightColor);
			map2CubeShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			map2CubeShader->setFloat("pointLights[0].constant", 1.0f);
			map2CubeShader->setFloat("pointLights[0].linear", 0.09f);
			map2CubeShader->setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			map2CubeShader->setVec3("spotLight.position", 0.0f, 0.0f, 0.0f);
			map2CubeShader->setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
			map2CubeShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			map2CubeShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			map2CubeShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			map2CubeShader->setFloat("spotLight.constant", 1.0f);
			map2CubeShader->setFloat("spotLight.linear", 0.09f);
			map2CubeShader->setFloat("spotLight.quadratic", 0.032f);
			map2CubeShader->setFloat("spotLight.innerCosAngle", glm::cos(glm::radians(12.5f)));
			map2CubeShader->setFloat("spotLight.outerCosAngle", glm::cos(glm::radians(15.0f)));
			map2CubeShader->setBool("spotLight.on", flashlightOn);

			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 2.0f, 8.0f));
			map2CubeShader->setMat4("model", modelMatrix);
			map2CubeShader->setMat4("view", viewMatrix);
			map2CubeShader->setMat4("projection", projectionMatrix);
			torusConeModel.draw(*map2CubeShader);
		}
		else if (currentMap == 3) 
		{
			// CRTANJE KRUZNOG IZVORA SVETLA
			float radius = 5.0f;
			glm::vec3 lightcubePos = glm::vec3(radius * sin(vreme), 0.0f, radius * cos(vreme)) + glm::vec3(-1.0f, 2.0f, 2.0f);

			glm::vec3 lightColor = glm::vec3(1.0f);
			lightColor.x = sin(vreme);
			lightColor.y = sin(vreme + glm::pi<float>() * 4 / 3);
			lightColor.z = sin(vreme + glm::pi<float>() * 2 / 3);

			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), lightcubePos);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

			lightsourceShader->use();
			lightsourceShader->setMat4("model", modelMatrix);
			lightsourceShader->setMat4("view", viewMatrix);
			lightsourceShader->setMat4("projection", projectionMatrix);
			lightsourceShader->setVec3("lightColor", lightColor);

			glBindVertexArray(lightsourceVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			map2CubeShader->use();
			// directional light
			map2CubeShader->setVec3("directionLight.direction", -0.2f, -1.0f, -0.3f);
			map2CubeShader->setVec3("directionLight.ambient", 0.05f, 0.05f, 0.05f);
			map2CubeShader->setVec3("directionLight.diffuse", 0.4f, 0.4f, 0.4f);
			map2CubeShader->setVec3("directionLight.specular", 0.5f, 0.5f, 0.5f);
			// point light kruzni
			map2CubeShader->setVec3("pointLights[0].position", glm::vec3(viewMatrix* glm::vec4(lightcubePos, 1.0f)));
			map2CubeShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			map2CubeShader->setVec3("pointLights[0].diffuse", lightColor);
			map2CubeShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			map2CubeShader->setFloat("pointLights[0].constant", 1.0f);
			map2CubeShader->setFloat("pointLights[0].linear", 0.09f);
			map2CubeShader->setFloat("pointLights[0].quadratic", 0.032f);
			// spotLight
			map2CubeShader->setVec3("spotLight.position", 0.0f, 0.0f, 0.0f);
			map2CubeShader->setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
			map2CubeShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			map2CubeShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			map2CubeShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			map2CubeShader->setFloat("spotLight.constant", 1.0f);
			map2CubeShader->setFloat("spotLight.linear", 0.09f);
			map2CubeShader->setFloat("spotLight.quadratic", 0.032f);
			map2CubeShader->setFloat("spotLight.innerCosAngle", glm::cos(glm::radians(12.5f)));
			map2CubeShader->setFloat("spotLight.outerCosAngle", glm::cos(glm::radians(15.0f)));
			map2CubeShader->setBool("spotLight.on", flashlightOn);

			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 2.0f, 8.0f));
			map2CubeShader->setMat4("model", modelMatrix);
			map2CubeShader->setMat4("view", viewMatrix);
			map2CubeShader->setMat4("projection", projectionMatrix);
			backpackModel.draw(*map2CubeShader);
		}
		

		// KRAJ RENDEROVANJA

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

	// Za promenu mape
	if ((glfwGetKey(window, GLFW_KEY_1)) == GLFW_PRESS) {
		currentMap = 1;
	}
	else if ((glfwGetKey(window, GLFW_KEY_2)) == GLFW_PRESS) {
		currentMap = 2;
	}
	else if ((glfwGetKey(window, GLFW_KEY_3)) == GLFW_PRESS) {
		currentMap = 3;
	}


	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		if (pressingR == false) {
			debugView = !debugView;
		}
		pressingR = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
		pressingR = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (pressingF == false) {
			flashlightOn = !flashlightOn;
		}
		pressingF = true;

	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		pressingF = false;
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
}

//Prati lokaciju misa
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouseMovement) {
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouseMovement = false;
	}

	float offsetX = static_cast<float>(xpos) - lastX;
	float offsetY = lastY - static_cast<float>(ypos);

	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

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

unsigned int loadTexture(const char* texPath) {
	uint textureID;
	glGenTextures(1, &textureID);

	int texWidth, texHeight, texNumberOfChannels;
	unsigned char* texData = stbi_load(texPath, &texWidth, &texHeight, &texNumberOfChannels, 0);

	if (texData) {

		GLenum imageformat = GL_RGB;
		if (texNumberOfChannels == 1) {
			imageformat = GL_RED;
		}
		if (texNumberOfChannels == 3) {
			imageformat = GL_RGB;
		}
		if (texNumberOfChannels == 4) {
			imageformat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, imageformat, texWidth, texHeight, 0, imageformat, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cerr << "Failed to load texture!" << std::endl;
	}

	stbi_image_free(texData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbinding je opcionalan uvek

	return textureID;
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

