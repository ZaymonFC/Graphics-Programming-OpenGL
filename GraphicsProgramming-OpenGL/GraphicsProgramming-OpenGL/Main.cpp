
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"
#include "Model.h"
#include <filesystem>


//
// ─── FUNCTION PROTOTYPES ────────────────────────────────────────────────────────
//
auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void;
auto mouse_callback(GLFWwindow* window, double xpos, double ypos) -> void;
auto ProcessInput(GLFWwindow * window) -> void;
auto LoadTexture(const char * path) -> unsigned int;

//
// ─── GLOBAL PARAMETERS ──────────────────────────────────────────────────────────
//
const auto Screen_Width = 1280.0f;
const auto Screen_Height = 720.0f;
const auto fov = 55.0f;


// Camera Base
auto _camera = Camera();

// Frame Timing
auto deltaTime = 0.0f;	// Time between current frame and last frame
auto lastFrame = 0.0f; // Time of last frame

// Mouse Position
auto mouseLastX = Screen_Width / 2;
auto mouseLastY = Screen_Height / 2;
auto firstMousePoll = false;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//
	// ─── CREATE WINDOW ──────────────────────────────────────────────────────────────
	const auto window = glfwCreateWindow(static_cast<int>(Screen_Width), static_cast<int>(Screen_Height), "Graphics Programming", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	// Global OpenGL Settings
	glEnable(GL_DEPTH_TEST);

	// SHADER PROGRAMS
//	auto lightingShader = Shader("shaders/lightingShader_vertex.shader", "shaders/lightingShader_fragment.shader");
//	auto lampShader = Shader("shaders/lampShader.vs", "shaders/lampShader.fs");

	auto modelShader = Shader("shaders/modelShader_vertex.shader", "shaders/modelShader_fragment.shader");
	auto modelPath = std::experimental::filesystem::canonical("objects/dummy/dummy_obj.obj").string();
	auto loadedModel = Model(modelPath.c_str());

	//
	// ──────────────────────────────────────────────────────────────────────────────── V ──────────
	//   :::::: A P P L I C A T I O N   M A I N L O O P : :  :   :    :     :        :          :
	// ──────────────────────────────────────────────────────────────────────────────────────────
	//

	while(!glfwWindowShouldClose(window))
	{
		// Timing
		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(window);

		// -- Render ---------------------------------------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		modelShader.Use();

		// View and projection transformations
		const auto projection = glm::perspective(glm::radians(fov), Screen_Width / Screen_Height, 0.1f, 100.0f);
		const auto view = _camera.GetViewMatrix();
		modelShader.SetMat4("view", view);
		modelShader.SetMat4("projection", projection);

		// Render the loaded model
		// World Transformation
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		modelShader.SetMat4("model", model);
		loadedModel.Draw(modelShader);
		
//		lightingShader.Use();
//		lightingShader.SetVec3("light.position", lightPos);
//		lightingShader.SetVec3("viewPosition", _camera.Position);
//
//		lightingShader.SetFloat("material.shininess", 32.0f);
//
//		lightingShader.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
//		lightingShader.SetVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
//		lightingShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
//
//		lightingShader.SetFloat("emissionIntensity", sin(glfwGetTime() + 0.5));
//		lightingShader.SetFloat("time", glfwGetTime());
//		
//		// Camera and view calculations -----
//		const auto projection = glm::perspective(glm::radians(fov), Screen_Width / Screen_Height, 0.1f, 100.0f);
//		const auto view = _camera.GetViewMatrix();
//
//		// Get matrix's uniform location and set matrix
//		lightingShader.SetMat4("view", view);
//		lightingShader.SetMat4("projection", projection);
//
//
//		// World Transformation
//		glm::mat4 model;
//		lightingShader.SetMat4("model", model);
//
//		// Bind defuse map
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, diffuseMap);
//
//		// Bind specular map
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, specularMap);
//
//		// Bind Emission Map
//		glActiveTexture(GL_TEXTURE2);
//		glBindTexture(GL_TEXTURE_2D, emissionMap);
//
//		// Render the cube
//		glBindVertexArray(cubeVAO);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		// Render the lamp object
//		lampShader.Use();
//		lampShader.SetMat4("projection", projection);
//		lampShader.SetMat4("view", view);
//		model = glm::mat4();
//		model = glm::translate(model, lightPos);
//		model = glm::scale(model, glm::vec3(0.2f));
//		lampShader.SetMat4("model", model);
//
//		glBindVertexArray(lightVAO);
//		glDrawArrays(GL_TRIANGLES, 0, 36);


		// -- Swap buffers and poll IO --------------------------------------------- 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Mop the floor
//	glDeleteVertexArrays(1, &cubeVAO);
//	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

auto ProcessInput(GLFWwindow* window) -> void
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Camera Movement
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		_camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		_camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		_camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		_camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		_camera.ProcessKeyboard(DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		_camera.ProcessKeyboard(UP, deltaTime);
	}

}

auto mouse_callback(GLFWwindow* window, double xpos, double ypos) -> void
{
	if (firstMousePoll)
	{
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMousePoll = false;
	}
	const float xoffset = xpos - mouseLastX;
	const float yoffset = mouseLastY - ypos; // reversed since y-coordinates range from bottom to top
	mouseLastX = xpos;
	mouseLastY = ypos;

	_camera.ProcessMouseLook(xoffset, yoffset);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


auto LoadTexture(const char * path) -> unsigned int
{
	unsigned int textureID;

	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	// Load the image data from disk
	auto imageData = stbi_load(path, &width, &height, &nrComponents, 0);

	if (imageData)
	{
		// Determine the image format
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(imageData);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(imageData);
	}

	return textureID;
}
