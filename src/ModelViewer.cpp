#include "ModelViewer.hpp"


ModelViewer::ModelViewer(const std::string &objPath)
:
VAO(0), VBO(0), texture(nullptr), shader(nullptr), keyInput(nullptr),
mixValue(0.0f), textureMode(false), tKeyPressed(false),
lightingValue(0.0f), lightingMode(false), lKeyPressed(false),
uvModeValue(0.0f), uvMode(false), uKeyPressed(false),
rotationAngle(0.0f), rotating(true), spaceKeyPressed(false),
rotationMatrix(Mat4::identity()),
mouseLastX(0.0f), mouseLastY(0.0f),
mouseDragging(false), mousePanning(false),
zoom(DEFAULT_ZOOM), currentObjIndex(0), currentBmpIndex(0)
{
	initWindow();
	initGL();
	
	ObjParser	objParser(objPath);
	if (!objParser.isValid())
	{
		std::cerr << "Failed to load model: " << objPath << std::endl;
		valid = false;
		return ;
	}
	valid = true;

	setBuffers(objParser);

	texture = new Texture(TEXTURE_PATH);
	shader = new Shader(VERTEX_SHADER, FRAGMENT_SHADER);
	shader->use();
	keyInput = new KeyInputHandler(window);

	objFiles = findFiles("resources", ".obj");
	bmpFiles = findFiles("resources", ".bmp");

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

ModelViewer::~ModelViewer()
{
	delete shader;
	delete texture;
	delete keyInput;

	if (VAO)
		glDeleteVertexArrays(1, &VAO);
	if (VBO)
		glDeleteBuffers(1, &VBO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void	ModelViewer::run(void)
{
	if (!valid)
		return ;

	while (!keyInput->shouldClose())
	{
		processInput();
		render();
	}

	return ;
}

bool	ModelViewer::isValid(void) const
{
	return (valid);
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

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
	std::vector<float> const &	vertices = parser.getVertices();
	vertexCount = vertices.size() / VERTEX_STRIDE;

	if (vertexCount == 0)
	{
		std::cerr << "No vertex data to display" << std::endl;
		return ;
	}

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
	if (keyInput->isPressed(GLFW_KEY_R)) resetView();

	handleToggle(GLFW_KEY_T, tKeyPressed, textureMode);
	handleToggle(GLFW_KEY_L, lKeyPressed, lightingMode);
	handleToggle(GLFW_KEY_U, uKeyPressed, uvMode);
	handleToggle(GLFW_KEY_SPACE, spaceKeyPressed, rotating);

	return ;
}

void	ModelViewer::handleToggle(int key, bool &keyPressed, bool &mode)
{
	if (keyInput->isPressed(key))
	{
		if (!keyPressed)
		{
			mode = !mode;
			keyPressed = true;
		}
	}
	else
		keyPressed = false;

	return ;
}

void	ModelViewer::resetView(void)
{
	objPos = Vec3(0.0f, 0.0f, 0.0f);
	rotationMatrix = Mat4::identity();
	rotationAngle = 0.0f;
	zoom = DEFAULT_ZOOM;

	return ;
}

void	ModelViewer::render(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderGUI();
	updateTransitions();
	renderScene();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);

	return ;
}

void	ModelViewer::renderGUI(void)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("SCOP");

	if (ImGui::BeginCombo("Model", objFiles[currentObjIndex].c_str()))
	{
		for (int i = 0; i < (int)objFiles.size(); ++i)
		{
			bool	selected = (i == currentObjIndex);
			if (ImGui::Selectable(objFiles[i].c_str(), selected))
			{
				currentObjIndex = i;
				loadModel(objFiles[i]);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Texture", bmpFiles[currentBmpIndex].c_str()))
	{
		for (int i = 0; i < (int)bmpFiles.size(); ++i)
		{
			bool	selected = (i == currentBmpIndex);
			if (ImGui::Selectable(bmpFiles[i].c_str(), selected))
			{
				currentBmpIndex = i;
				texture->reload("resources/" + bmpFiles[i], true);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::End();
	ImGui::Render();

	return ;
}

void	ModelViewer::updateTransitions(void)
{
	smoothTransition(textureMode, mixValue, TRANSITION_SPEED);
	smoothTransition(lightingMode, lightingValue, TRANSITION_SPEED);
	smoothTransition(uvMode, uvModeValue, TRANSITION_SPEED);

	if (rotating)
		rotationAngle += ROTATION_SPEED;
}

void	ModelViewer::smoothTransition(bool mode, float &value, float speed)
{
	if (mode && value < 1.0f)
		value += speed;
	else if (!mode && value >= 0.0f)
		value -= speed;
	value = std::max(0.0f, std::min(1.0f, value));

	return ;
}

void	ModelViewer::renderScene(void)
{
	int		width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float	aspect = (float)width / (float)height;

	Mat4	view = Mat4::identity();
	view = Mat4::translate(view, Vec3(0.0f, 0.0f, -zoom));
	Mat4	projection = Mat4::perspective(FOV, aspect, NEAR_PLANE, FAR_PLANE);

	Mat4	model = Mat4::identity();
	Mat4	autoRot = Mat4::rotate(Mat4::identity(), rotationAngle, Vec3(0.0f, 1.0f, 0.0f));
	model = Mat4::translate(model, objPos);
	model = model * rotationMatrix * autoRot;

	shader->use();
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	shader->setFloat("mixValue", mixValue);
	shader->setFloat("lightingValue", lightingValue);
	shader->setFloat("uvMode", uvModeValue);

	texture->bind(0);
	shader->setInt("ourTexture", 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

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
	if (ImGui::GetIO().WantCaptureMouse)
		return ;

	ModelViewer *	viewer = (ModelViewer *)glfwGetWindowUserPointer(window);

	float	dx = (float)xpos - viewer->mouseLastX;
	float	dy = (float)ypos - viewer->mouseLastY;

	if (viewer->mouseDragging)
	{
		Mat4	rotX = Mat4::rotate(Mat4::identity(), dy * MOUSE_SENSITIVITY, Vec3(1.0f, 0.0f, 0.0f));
		Mat4	rotY = Mat4::rotate(Mat4::identity(), dx * MOUSE_SENSITIVITY, Vec3(0.0f, 1.0f, 0.0f));

		viewer->rotationMatrix = rotX * rotY * viewer->rotationMatrix;
	}
	else if (viewer->mousePanning)
	{
		viewer->objPos.x += dx * MOUSE_SENSITIVITY;
		viewer->objPos.y -= dy * MOUSE_SENSITIVITY;
	}

	viewer->mouseLastX = (float)xpos;
	viewer->mouseLastY = (float)ypos;

	return ;
}

void	ModelViewer::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return ;

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
	if (ImGui::GetIO().WantCaptureMouse)
		return ;

    (void)xoffset;
    ModelViewer *viewer = (ModelViewer *)glfwGetWindowUserPointer(window);

    viewer->zoom -= (float)yoffset * 0.5f;

    if (viewer->zoom < ZOOM_MIN) viewer->zoom = ZOOM_MIN;
    if (viewer->zoom > ZOOM_MAX) viewer->zoom = ZOOM_MAX;

	return ;
}

std::vector<std::string>	ModelViewer::findFiles(const std::string &directory, const std::string &extension)
{
	std::vector<std::string>	files;

	for (const auto &entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.path().extension() == extension)
			files.push_back(entry.path().filename().string());
	}

	std::sort(files.begin(), files.end());

	return (files);
}

void	ModelViewer::loadModel(const std::string &filename)
{
	std::string	path = "resources/" + filename;

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	ObjParser	parser(path);
	setBuffers(parser);

	rotationMatrix = Mat4::identity();
	rotationAngle = 0.0f;
	objPos = Vec3(0.0f, 0.0f, 0.0f);
	
	return ;
}
