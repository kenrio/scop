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

	lightingValue = 0.0f;
	lightingMode = false;
	lKeyPressed = false;

	uvModeValue = 0.0f;
	uvMode = false;
	uKeyPressed = false;

	rotationAngle = 0.0f;
	rotating = true;

	rotationMatrix = Mat4::identity();

	mouseLastX = 0.0f;
	mouseLastY = 0.0f;
	mouseDragging = false;

	mousePanning = false;

	zoom = 5.0f;

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

	glfwSetWindowUserPointer(window, this);
	glfwSetCursorPosCallback(window, mousePositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);

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

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void *)(10 * sizeof(float)));
	glEnableVertexAttribArray(4);

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

	if (keyInput->isPressed(GLFW_KEY_L))
	{
		if (!lKeyPressed)
		{
			lightingMode = !lightingMode;
			lKeyPressed = true;
		}
	}
	else
		lKeyPressed = false;

	if (keyInput->isPressed(GLFW_KEY_U))
	{
		if (!uKeyPressed)
		{
			uvMode = !uvMode;
			uKeyPressed = true;
		}
	}
	else
		uKeyPressed = false;

	if (keyInput->isPressed(GLFW_KEY_SPACE))
	{
		if (!spaceKeyPressed)
		{
			rotating = !rotating;
			spaceKeyPressed = true;
		}
	}
	else
		spaceKeyPressed = false;

	return ;
}

void	ModelViewer::render(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Mat4	view = Mat4::identity();
	view = Mat4::translate(view, Vec3(0.0f, 0.0f, -zoom));
	Mat4	projection = Mat4::perspective(FOV, WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_PLANE, FAR_PLANE);

	if (textureMode && mixValue < 1.0f)
		mixValue += MIX_SPEED;
	else if (!textureMode && mixValue > 0.0f)
		mixValue -= MIX_SPEED;

	if (lightingMode && lightingValue < 1.0f)
		lightingValue += MIX_SPEED;
	else if (!lightingMode && lightingValue > 0.0f)
		lightingValue -= MIX_SPEED;

	if (uvMode && uvModeValue < 1.0f)
		uvModeValue += MIX_SPEED;
	else if (!uvMode && uvModeValue > 0.0f)
		uvModeValue -= MIX_SPEED;

	if (uvModeValue > 1.0f) uvModeValue = 1.0f;
	if (uvModeValue < 0.0f) uvModeValue = 0.0f;

	if (rotating)
		rotationAngle += 0.01f;

	shader->use();

	shader->setFloat("mixValue", mixValue);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	texture->bind(0);
	shader->setInt("ourTexture", 0);
	shader->setFloat("lightingValue", lightingValue);
	shader->setFloat("uvMode", uvModeValue);

	glBindVertexArray(VAO);
	Mat4	model = Mat4::identity();

	Mat4	autoRot = Mat4::rotate(Mat4::identity(), rotationAngle, Vec3(0.0f, 1.0f, 0.0f));
	model = Mat4::translate(model, objPos);
	model = model * rotationMatrix * autoRot;
	
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

void	ModelViewer::mousePositionCallback(GLFWwindow * window, double xpos, double ypos)
{
	ModelViewer *	viewer = (ModelViewer *)glfwGetWindowUserPointer(window);

	float	dx = (float)xpos - viewer->mouseLastX;
	float	dy = (float)ypos - viewer->mouseLastY;

	if (viewer->mouseDragging)
	{
		Mat4	rotX = Mat4::rotate(Mat4::identity(), dy * 0.01f, Vec3(1.0f, 0.0f, 0.0f));
		Mat4	rotY = Mat4::rotate(Mat4::identity(), dx * 0.01f, Vec3(0.0f, 1.0f, 0.0f));

		viewer->rotationMatrix = rotX * rotY * viewer->rotationMatrix;
	}
	else if (viewer->mousePanning)
	{
		viewer->objPos.x += dx * 0.01f;
		viewer->objPos.y -= dy * 0.01;
	}

	viewer->mouseLastX = (float)xpos;
	viewer->mouseLastY = (float)ypos;

	return ;
}

void	ModelViewer::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	(void)mods;
	ModelViewer *	viewer = (ModelViewer *)glfwGetWindowUserPointer(window);

	double	x, y;
	glfwGetCursorPos(window, &x, &y);

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			viewer->mouseDragging = true;
			viewer->mouseLastX = (float)x;
			viewer->mouseLastY = (float)y;
		}
		else if (action == GLFW_RELEASE)
			viewer->mouseDragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			viewer->mousePanning = true;
			viewer->mouseLastX = (float)x;
			viewer->mouseLastY = (float)y;
		}
		else if (action == GLFW_RELEASE)
			viewer->mousePanning = false;
	}

	return ;
}

void ModelViewer::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    (void)xoffset;
    ModelViewer *viewer = (ModelViewer *)glfwGetWindowUserPointer(window);

    viewer->zoom -= (float)yoffset * 0.5f;

    if (viewer->zoom < 1.0f) viewer->zoom = 1.0f;
    if (viewer->zoom > 2000.0f) viewer->zoom = 2000.0f;

	return ;
}
