#include "scop.h" 

void	glfw_error_callback(int error, const char * description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void	key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		std::cout << "ESC pressed, closing..." << std::endl;
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (action == GLFW_PRESS)
	{
		std::cout << "Key pressed: " << key << std::endl;
	}
}

void	framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

bool	init_opengl(t_gl_context *ctx)
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
		return (false);
	}

	std::cout << "=== OpenGL Information ===" << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "==========================" << std::endl;

	std::cout << "Loading shaders from files..." << std::endl;
	ctx->shader_program = load_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	if (ctx->shader_program == 0)
	{
		std::cerr << "Failed to load shaders" << std::endl;
		return (false);
	}

	std::cout << "Shaders loaded successfully (Program ID: " << ctx->shader_program << ")" << std::endl;

	float vertices[] =
	{
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	std::cout << "Creating VBO/VAO..." << std::endl;

	glGenVertexArrays(1, &ctx->vao);
	glBindVertexArray(ctx->vao);

	glGenBuffers(1, &ctx->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	glBindVertexArray(0);

	GLenum gl_error = glGetError();
	if (gl_error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL Error: " << gl_error << std::endl;
		return (false);
	}

	std::cout << "OpenGL initialized succesfully" <<std::endl;
	return (true);
}

int	main(void)
{
	t_gl_context	ctx = {};

	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return (1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::cout << "Attempting to create window with OpenGL 3.3..." << std::endl;

	ctx.window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (!ctx.window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return (1);
	}

	glfwMakeContextCurrent(ctx.window);

	glfwSwapInterval(1);

	glfwSetKeyCallback(ctx.window, key_callback);
	glfwSetFramebufferSizeCallback(ctx.window, framebuffer_size_callback);

	if (!init_opengl(&ctx))
	{
		glfwTerminate();
		return (1);
	}

	std::cout << "\n=== Controls ===" << std::endl;
	std::cout << "ESC: Exit" << std::endl;
	std::cout << "==================" << std::endl;

	while (!glfwWindowShouldClose(ctx.window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(ctx.shader_program);
		glBindVertexArray(ctx.vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(ctx.window);
		glfwPollEvents();
	}

	std::cout << "Cleaning up..." << std::endl;
	glDeleteVertexArrays(1, &ctx.vao);
	glDeleteBuffers(1, &ctx.vbo);
	glDeleteProgram(ctx.shader_program);

	glfwDestroyWindow(ctx.window);
	glfwTerminate();

	std::cout << "Program terminated successfully" << std::endl;
	return (0);
}

