#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

//
// ─── SHADERS ────────────────────────────────────────────────────────────────────
//
const auto vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const auto fragmentShaderSource = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//
	// ─── CREATE WINDOW ──────────────────────────────────────────────────────────────
	auto window = glfwCreateWindow(Screen_Width, Screen_Height, "Graphics Programming", nullptr, nullptr);
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

	//
	// ──────────────────────────────────────────────────────────────────────────────────────────────────── III ──────────
	//   :::::: S H A D E R S   B U I L D   C O M P I L E   A N D   L I N K : :  :   :    :     :        :          :
	// ──────────────────────────────────────────────────────────────────────────────────────────────────────────────
	//
	//
	// ─── BUILD AND COMPILE SHADER PROGRAMS ──────────────────────────────────────────
	auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	// Check for shader compile errors
	// Vertex Shader
	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		fflush(stdout);
	}

	// Fragment Shader
	auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		fflush(stdout);
	}
	
	//
	// ─── LINK SHADERS ───────────────────────────────────────────────────────────────
	const auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	 
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
		fflush(stdout);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//
	// ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── IV ──────────
	//   :::::: V E R T E X   D A T A   |   B U F F E R S   |   V E R T E X   A T T R I B U T E S : :  :   :    :     :        :          :
	// ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
	//
	float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex array object first and set vertex buffers then configure vertex attributes
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
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

		// draw our first triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// glBindVertexArray(0); // no need to unbind it every time 


		// Check and call events then swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Mop the floor

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

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
