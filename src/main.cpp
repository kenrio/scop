#include <iostream>
#include <string>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Math.hpp"
#include "ObjParser.hpp"
#include "KeyInputHandler.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

const float			WINDOW_WIDTH = 800.0f;
const float			WINDOW_HEIGHT = 600.0f;
const std::string	WINDOW_TITLE = "scop";

const float		FOV = 0.785f;
const float		NEAR_PLANE = 0.1f;
const float		FAR_PLANE = 100.0f;
const float		CAMERA_DIST = -5.0f;

const float		MOVE_SPEED = 0.01f;
const float		MIX_SPEED = 0.01f;

const int		VERTEX_STRIDE = 8;

const std::string	TEXTURE_PATH = "resources/myLittlePoneys.bmp";
const std::string	VERTEX_SHADER = "shaders/vertex.glsl";
const std::string	FRAGMENT_SHADER = "shaders/fragment.glsl";

void	framebuffer_size_callback(GLFWwindow *window, int width, int height);

int	main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: ./scop <obj_file_path>" << std::endl;
		return (1);
	}

	std::string	objPath = argv[1];

	glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return (1);
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return (1);
	}

	int	width;
	int	height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	std::cout << "=== OpenGL information ===" << std::endl;
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "==========================" << std::endl;

	ObjParser	objParser(objPath);

	unsigned int	VBO;
	glGenBuffers(1, &VBO);

	unsigned int	VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	std::vector<float> const &vertices = objParser.getVertices();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	Texture	texture(TEXTURE_PATH);

	float	mixValue = 0.0f;
	bool	textureMode = false;
	bool	tKeyPressed = false;

	KeyInputHandler	keyInput(window);
	Vec3			objPos(0.0f, 0.0f, 0.0f);
	float			moveSpeed = MOVE_SPEED;

	Shader	shader(VERTEX_SHADER, FRAGMENT_SHADER);
	shader.use();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	while (!keyInput.shouldClose())
	{
		keyInput.update();

		if (keyInput.isPressed(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, true);
			break ;
		}
		if (keyInput.isPressed(GLFW_KEY_W)) objPos.z -= moveSpeed;
		if (keyInput.isPressed(GLFW_KEY_S)) objPos.z += moveSpeed;
		if (keyInput.isPressed(GLFW_KEY_A)) objPos.x -= moveSpeed;
		if (keyInput.isPressed(GLFW_KEY_D)) objPos.x += moveSpeed;
		if (keyInput.isPressed(GLFW_KEY_Q)) objPos.y -= moveSpeed;
		if (keyInput.isPressed(GLFW_KEY_E)) objPos.y += moveSpeed;
		if (keyInput.isPressed(GLFW_KEY_T))
		{
			if (!tKeyPressed)
			{
				textureMode = !textureMode;
				tKeyPressed = true;
			}
		}
		else
			tKeyPressed = false;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4	view = Mat4::identity();
		view = Mat4::translate(view, Vec3(0.0f, 0.0f, CAMERA_DIST));
		Mat4	projection = Mat4::perspective(FOV, WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_PLANE, FAR_PLANE);

		if (textureMode && mixValue < 1.0f)
			mixValue += MIX_SPEED;
		else if (!textureMode && mixValue > 0.0f)
			mixValue -= MIX_SPEED;

		shader.use();

		shader.setFloat("mixValue", mixValue);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		texture.bind(0);
		shader.setInt("ourTexture", 0);

		glBindVertexArray(VAO);
		Mat4	model = Mat4::identity();

		model = Mat4::translate(model, objPos);
		model = Mat4::rotate(model, (float)glfwGetTime() * -1.0f, Vec3(0.0f, 1.0f, 0.0f));
		
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / VERTEX_STRIDE);

		glfwSwapBuffers(window);

	}

	glfwTerminate();

	return (0);
}

void	framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);

	return ;
}
