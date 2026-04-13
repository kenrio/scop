#include <unistd.h>
#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Math.hpp"
#include "ObjParser.hpp"
#include "KeyInputHandler.hpp"
#include "Shader.hpp"

void	framebuffer_size_callback(GLFWwindow *window, int width, int height);

int	main(void)
{
	glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow	*window = glfwCreateWindow(800, 600, "scop", NULL, NULL);
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
	std::cout << "=========================" << std::endl;

	ObjParser	objParser("resources/42.obj");

	unsigned int	VBO;
	glGenBuffers(1, &VBO);

	// unsigned int	EBO;
	// glGenBuffers(1, &EBO);

	unsigned int	VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	std::vector<float> const &vertices = objParser.getVertices();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// std::vector<unsigned int> const &indices = objParser.getIndices();
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	KeyInputHandler	keyInput(window);
	Vec3			objPos(0.0f, 0.0f, 0.0f);
	float			moveSpeed = 0.01f;

	Shader	shader("shaders/vertex.glsl", "shaders/fragment.glsl");
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

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4	view = Mat4::identity();
		view = Mat4::translate(view, Vec3(0.0f, 0.0f, -5.0f));
		Mat4	projection = Mat4::perspective(0.785f, 800.0f/600.f, 0.1f, 100.0f);

		shader.use();

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glBindVertexArray(VAO);
		Mat4	model = Mat4::identity();

		model = Mat4::translate(model, objPos);
		model = Mat4::rotate(model, (float)glfwGetTime() * -1.0f, Vec3(0.0f, 1.0f, 0.0f));
		// model = Mat4::rotate(model, -1.6f, Vec3(0.0f, 1.0f, 0.0f));
		
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
		// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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
