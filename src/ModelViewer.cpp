#include "ModelViewer.hpp"


ModelViewer::ModelViewer(const std::string &objPath)
{
	initWindow();
	initGL();

	ObjParser	objParser(objPath);
	setBuffers(objParser);

	texture = new Texture(TEXTURE_PATH);
	mixValue = 0.0f;
	textureMode = false;
	tKeyPressed = false;

	shader = new Shader(VERTEX_SHADER, FRAGMENT_SHADER);
	shader->use();

	keyInput = new KeyInputHandler(window);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

ModelViewer::~ModelViewer()
{
	delete shader;
	delete texture;
	delete keyInput;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}

void	ModelViewer::run(void)
{
	while (!keyInput->shouldClose())
	{
		processInput();
		render();
	}

	return ;
}

void	ModelViewer::initWindow(void)
{
	glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return ;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	return ;
}

void	ModelViewer::initGL(void)
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return ;
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

	return ;
}

void	ModelViewer::setBuffers(const ObjParser &parser)
{
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	std::vector<float> const &vertices = parser.getVertices();
	vertexCount = vertices.size() / VERTEX_STRIDE;
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return ;
}

void	ModelViewer::processInput(void)
{
	keyInput->update();

	if (keyInput->isPressed(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	if (keyInput->isPressed(GLFW_KEY_W)) objPos.z -= MOVE_SPEED;
	if (keyInput->isPressed(GLFW_KEY_S)) objPos.z += MOVE_SPEED;
	if (keyInput->isPressed(GLFW_KEY_A)) objPos.x -= MOVE_SPEED;
	if (keyInput->isPressed(GLFW_KEY_D)) objPos.x += MOVE_SPEED;
	if (keyInput->isPressed(GLFW_KEY_Q)) objPos.y -= MOVE_SPEED;
	if (keyInput->isPressed(GLFW_KEY_E)) objPos.y += MOVE_SPEED;

	if (keyInput->isPressed(GLFW_KEY_T))
	{
		if (!tKeyPressed)
		{
			textureMode = !textureMode;
			tKeyPressed = true;
		}
	}
	else
		tKeyPressed = false;

	return ;
}

void	ModelViewer::render(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Mat4	view = Mat4::identity();
	view = Mat4::translate(view, Vec3(0.0f, 0.0f, CAMERA_DIST));
	Mat4	projection = Mat4::perspective(FOV, WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_PLANE, FAR_PLANE);

	if (textureMode && mixValue < 1.0f)
		mixValue += MIX_SPEED;
	else if (!textureMode && mixValue > 0.0f)
		mixValue -= MIX_SPEED;

	shader->use();

	shader->setFloat("mixValue", mixValue);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	texture->bind(0);
	shader->setInt("ourTexture", 0);

	glBindVertexArray(VAO);
	Mat4	model = Mat4::identity();

	model = Mat4::translate(model, objPos);
	model = Mat4::rotate(model, (float)glfwGetTime() * -1.0f, Vec3(0.0f, 1.0f, 0.0f));
	
	shader->setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glfwSwapBuffers(window);

	return ;
}

void	ModelViewer::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);

	return ;
}
