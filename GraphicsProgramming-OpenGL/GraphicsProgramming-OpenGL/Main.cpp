#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//
// ─── FUNCTION PROTOTYPES ────────────────────────────────────────────────────────
//
auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void;
auto ProcessInput(GLFWwindow * window) -> void;

//
// ─── GLOBAL PARAMETERS ──────────────────────────────────────────────────────────
//
const auto Screen_Width = 800;
const auto Screen_Height = 600;

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//
	// ─── CREATE WINDOW ──────────────────────────────────────────────────────────────
	
	// Weird errors? Maybe don't make this const
	const auto window = glfwCreateWindow(Screen_Width, Screen_Height, "Graphics Programming", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	// Create the shader program
	auto shaderProgram = Shader("shaders/shader.vs", "shaders/shader.fs");

	float vertices[] = {
		// positions         // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f 
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};


	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind vertex array object first and set vertex buffers then configure vertex attributes
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(nullptr));
	glEnableVertexAttribArray(0);

	// Colour Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	

	// Texturing and shit -------------------------------------------------------------
	unsigned int texture1, texture2;
	// Create and bind the texture
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);


	// Set the wrapping and scaling parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load in the image data and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Free the image data
	stbi_image_free(data);

	// Create and bind the texture
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);


	// Set the wrapping and filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load in the image data and generate the texture

	data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Free the image data
	stbi_image_free(data);

	// Tell OGL for each sampler to which texture unit it belongs to (Done once)
	shaderProgram.Use();

//	glUniform1i(glGetUniformLocation(shaderProgram.ID, "texture1"), 0);
	shaderProgram.SetInt("texture1", 0);
	shaderProgram.SetInt("texture2", 1);

	//
	// ──────────────────────────────────────────────────────────────────────────────── V ──────────
	//   :::::: A P P L I C A T I O N   M A I N L O O P : :  :   :    :     :        :          :
	// ──────────────────────────────────────────────────────────────────────────────────────────
	//
	while(!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// -- Render ---------------------------------------------------------------

		// Bind active textures on corrosponding texure units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		shaderProgram.Use();
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		// -- Swap buffers and poll IO --------------------------------------------- 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Mop the floor
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

auto ProcessInput(GLFWwindow* window) -> void
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
