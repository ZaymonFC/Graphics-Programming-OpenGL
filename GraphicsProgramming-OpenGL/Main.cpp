
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <PerlinNoise.hpp>

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



Mesh TerrainMaker(const float width, const float length, const float height, int wRes=256, int lRes=256)
{
	const auto Noise = siv::PerlinNoise();
	const auto size = wRes * lRes;
	auto vertices = std::vector<Vertex>(size);
	auto normals = std::vector<glm::vec3>(size);
	auto indices = std::vector<unsigned int>();

	auto i = 0;
	for (auto z = 0; z < lRes; ++z) for (auto x = 0; x < wRes; x++)
	{
		auto v = glm::vec3(x / static_cast<float>(wRes), 0, z/static_cast<float>(lRes));

		v.x *= width;
		v.z *= length;
		v.x -= width / 2;
		v.z -= length / 2;

		// Set the height based on perline noise
		v.y = Noise.noise0_1(v.x, v.z) * height;
		auto vertex = Vertex();
		vertex.Position = v;
		vertex.Normal = glm::vec3(0, 1, 0);
		vertices[i] = vertex;
		
		if ((i + 1 % wRes) != 0 && z + 1 < lRes)
		{
//			auto tri = glm::ivec3(i, i + wRes, i + wRes + 1);
//			auto tri2 = glm::ivec3(i, i + wRes + 1, i + 1);

			indices.emplace_back(i);
			indices.emplace_back(i + wRes);
			indices.emplace_back(i + wRes + 1);
			indices.emplace_back(i);
			indices.emplace_back(i + wRes + 1);
			indices.emplace_back(i + 1);
		}

		++i;
	}

	// Add the texture
	auto textures = std::vector<Texture>();
	auto texture = Texture();
	texture.id = LoadTexture("objects/grass.png");
	texture.type = "texture_diffuse";
	texture.path = "path";
	textures.push_back(texture);
	
	return Mesh(vertices, indices, textures);
}


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
	auto modelShader = Shader("shaders/lightingShader_vertex.shader", "shaders/lightingShader_fragment.shader");
	auto lampShader = Shader("shaders/lampShader.vs", "shaders/lampShader.fs");
	auto simpleDepthShader = Shader("shaders/shadowMap_vertex.shader", "shaders/shadowMap_Fragment.shader");

	// Load house model
	auto modelPath = std::experimental::filesystem::canonical("objects/house/Medieval_House.obj").string();
	auto houseModel = Model(modelPath.c_str());

	// Load lamp model
	auto lampModelPath = std::experimental::filesystem::canonical("objects/cube.obj").string();
	auto lampModel = Model(lampModelPath.c_str());

	// Create terrain mesh (Perlin noise)
	auto terrainMesh = TerrainMaker(15, 15, 2);

	// Configure depth map FBO
	const unsigned int SHADOW_WIDTH = 8000, SHADOW_HEIGHT = 8000;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//
	// ──────────────────────────────────────────────────────────────────────────────── V ──────────
	//   :::::: A P P L I C A T I O N   M A I N L O O P : :  :   :    :     :        :          :
	// ──────────────────────────────────────────────────────────────────────────────────────────
	//
	while(!glfwWindowShouldClose(window))
	{
		// lighting
		glm::vec3 lightPos(sin(glfwGetTime()) * 5, 10, -sin(glfwGetTime()) * 5);

		// Timing
		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(window);

		// -- Render ---------------------------------------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		const auto near_plane = 0.1f;
		const auto far_plane = 100.0f;
		const auto lightProjection = glm::ortho(-15.0f, 15.0f, -10.0f, 10.0f, near_plane, far_plane);
//		const auto lightProjection = glm::perspective(glm::radians(fov), static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, 0.1f, 100.0f);
		const auto lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		const auto lightSpaceMatrix = lightProjection * lightView;

        simpleDepthShader.Use();
        simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glCullFace(GL_FRONT);
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
			modelShader.SetMat4("model", model);
			houseModel.Draw(modelShader);

			// Draw the terrain
			model = glm::translate(model, glm::vec3(0, -5, 0));
			model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
			modelShader.SetMat4("model", model);
			terrainMesh.Draw(modelShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);


		glViewport(0, 0, Screen_Width, Screen_Height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		modelShader.Use();

		// View and projection transformations
		const auto projection = glm::perspective(glm::radians(fov), Screen_Width / Screen_Height, 0.1f, 100.0f);
		const auto view = _camera.GetViewMatrix();
		modelShader.SetMat4("view", view);
		modelShader.SetMat4("projection", projection);
		modelShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		modelShader.SetVec3("light.position", lightPos);
		modelShader.SetVec3("viewPosition", _camera.Position);
		modelShader.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		modelShader.SetVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		modelShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
		modelShader.SetFloat("material.shininess", 32.0f);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		// Render the loaded model
		// World Transformation
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		modelShader.SetMat4("model", model);
		houseModel.Draw(modelShader);


		// Draw the terrain
		model = glm::translate(model, glm::vec3(0, -5, 0));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		modelShader.SetMat4("model", model);
		terrainMesh.Draw(modelShader);

		// Render the lamp object
		lampShader.Use();
		lampShader.SetMat4("projection", projection);
		lampShader.SetMat4("view", view);
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.SetMat4("model", model);
		lampModel.Draw(lampShader);


		// -- Swap buffers and poll IO --------------------------------------------- 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
