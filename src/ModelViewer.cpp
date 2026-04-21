#include "ModelViewer.hpp"


ModelViewer::ModelViewer(const std::string &objPath)
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
	setNormalBuffers(objParser);
	setAxisBuffers();

	texture = new Texture(TEXTURE_PATH);
	shader = new Shader(VERTEX_SHADER, FRAGMENT_SHADER);
	shader->use();
	keyInput = new KeyInputHandler(window);

	normalShader = new Shader(NORMAL_VERTEX_SHADER, NORMAL_FRAGMENT_SHADER);
	axisShader = new Shader(AXIS_VERTEX_SHADER, AXIS_FRAGMENT_SHADER);

	objFiles = findFiles("resources", ".obj");
	currentObjIndex = findFileIndex(objFiles, std::filesystem::path(objPath).filename().string());
	
	bmpFiles = findFiles("resources", ".bmp");
	currentBmpIndex = findFileIndex(bmpFiles, std::filesystem::path(TEXTURE_PATH).filename().string());

	modelPositionCount = objParser.getPositionCount();
	modelFaceCount = objParser.getFaceCount();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

ModelViewer::~ModelViewer()
{
	delete	shader;
	delete	texture;
	delete	keyInput;

	delete	normalShader;
	delete	axisShader;

	deleteBuffers(modelBuf);
	deleteBuffers(normalBuf);
	deleteBuffers(axisBuf);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void	ModelViewer::deleteBuffers(GLBuffers &buf)
{
	if (buf.VAO)
		glDeleteVertexArrays(1, &buf.VAO);
	if (buf.VBO)
		glDeleteBuffers(1, &buf.VBO);

	return ;
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
	modelBuf.vertexCount = vertices.size() / VERTEX_STRIDE;

	if (modelBuf.vertexCount == 0)
	{
		std::cerr << "No vertex data to display" << std::endl;
		return ;
	}

	glGenBuffers(1, &modelBuf.VBO);
	glGenVertexArrays(1, &modelBuf.VAO);

	glBindVertexArray(modelBuf.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, modelBuf.VBO);
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

void	ModelViewer::setNormalBuffers(const ObjParser &parser)
{
	std::vector<float> const &	lines = parser.getNormalLines();
	normalBuf.vertexCount = lines.size() / 3;

	if (normalBuf.vertexCount == 0)
		return ;

	glGenBuffers(1, &normalBuf.VBO);
	glGenVertexArrays(1, &normalBuf.VAO);

	glBindVertexArray(normalBuf.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuf.VBO);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), lines.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	return ;
}

void	ModelViewer::setAxisBuffers(void)
{
	float axisData[] =
	{
		// X軸（赤）
		0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		// Y軸（緑）
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		// Z軸（青）
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    };

	axisBuf.vertexCount = 6;

	glGenVertexArrays(1, &axisBuf.VAO);
	glGenBuffers(1, &axisBuf.VBO);

	glBindVertexArray(axisBuf.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, axisBuf.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisData), axisData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

	handleToggle(GLFW_KEY_T, textureToggle.keyPressed, textureToggle.mode);
	handleToggle(GLFW_KEY_L, lightingToggle.keyPressed, lightingToggle.mode);
	handleToggle(GLFW_KEY_U, uvToggle.keyPressed, uvToggle.mode);
	handleToggle(GLFW_KEY_SPACE, spaceKeyPressed, rotating);
	handleToggle(GLFW_KEY_F, fKeyPressed, wireframe);
	handleToggle(GLFW_KEY_N, nKeyPressed, showNormals);

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
	glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderGUI();
	updateTransitions();
	updateMatrices();
	renderAxis();
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

	std::string	texLabel = textureToggle.mode ? bmpFiles[currentBmpIndex].c_str() : "None";
	if (ImGui::BeginCombo("Texture", texLabel.c_str()))
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

	ImGui::Separator();

	ImGui::Text("Vertices: %d", modelPositionCount);
	ImGui::Text("Faces: %d", modelFaceCount);

	ImGui::Separator();

	ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);

	ImGui::Separator();

	ImGui::Text("Texture: %s", textureToggle.mode ? "ON": "OFF");
	ImGui::Text("Lighting: %s", lightingToggle.mode ? "ON": "OFF");
	ImGui::Text("OBJ UV: %s", uvToggle.mode ? "ON": "OFF");
	ImGui::Text("Wireframe: %s", wireframe ? "ON": "OFF");
	ImGui::Text("Normals: %s", showNormals ? "ON": "OFF");
	ImGui::Text("Rotation: %s", rotating ? "ON": "OFF");

	ImGui::Separator();

	ImGui::Text("Model position: (%.1f, %.1f, %.1f)", objPos.x, objPos.y, objPos.z);
	ImGui::Text("Zoom: %.1f", zoom);

	ImGui::Separator();

    if (ImGui::CollapsingHeader("Controls"))
    {
        ImGui::Text("WASD/QE - Move model");
        ImGui::Text("T - Toggle texture");
        ImGui::Text("L - Toggle lighting");
        ImGui::Text("U - Toggle OBJ UV");
        ImGui::Text("F - Toggle wireframe");
        ImGui::Text("N - Toggle normals");
        ImGui::Text("Space - Toggle rotation");
        ImGui::Text("R - Reset view");
        ImGui::Text("Left drag - Rotate");
        ImGui::Text("Right drag - Pan");
        ImGui::Text("Scroll - Zoom");
        ImGui::Text("Esc - Quit");
    }

	ImGui::End();
	ImGui::Render();

	return ;
}

void	ModelViewer::renderAxis(void)
{
	int	width, height;
	glfwGetFramebufferSize(window, &width, &height);

    // 右上の小さな領域にだけ描画
	int axisSize = 100;
	glViewport(width - axisSize, height - axisSize, axisSize, axisSize);
	glClear(GL_DEPTH_BUFFER_BIT);

    Mat4 axisView = Mat4::translate(Mat4::identity(), Vec3(0.0f, 0.0f, -3.0f));
    Mat4 axisProjection = Mat4::perspective(FOV, 1.0f, 0.1f, 100.0f);
    Mat4 autoRot = Mat4::rotate(Mat4::identity(), rotationAngle, Vec3(0.0f, 1.0f, 0.0f));
    Mat4 axisModel = rotationMatrix * autoRot;

	axisShader->use();
	axisShader->setMat4("model", axisModel);
	axisShader->setMat4("view", axisView);
	axisShader->setMat4("projection", axisProjection);

	glLineWidth(2.0f);
	glBindVertexArray(axisBuf.VAO);
	glDrawArrays(GL_LINES, 0, axisBuf.vertexCount);

    // ビューポートを元に戻す
	glViewport(0, 0, width, height);

	shader->use();

	return ;
}

void	ModelViewer::updateTransitions(void)
{
	smoothTransition(textureToggle.mode, textureToggle.value, TRANSITION_SPEED);
	smoothTransition(lightingToggle.mode, lightingToggle.value, TRANSITION_SPEED);
	smoothTransition(uvToggle.mode, uvToggle.value, TRANSITION_SPEED);

	if (rotating)
	{
		rotationAngle += ROTATION_SPEED;
		rotationAngle = std::fmod(rotationAngle, 2.0f * M_PI);
	}

	return ;
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

void	ModelViewer::updateMatrices(void)
{
	int	width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float	aspect = (float)width / (float)height;

	currentView = Mat4::translate(Mat4::identity(), Vec3(0.0f, 0.0f, -zoom));
	currentProjection = Mat4::perspective(FOV, aspect, NEAR_PLANE, FAR_PLANE);

	Mat4	autoRot = Mat4::rotate(Mat4::identity(), rotationAngle, Vec3(0.0f, 1.0f, 0.0f));
	currentModel = Mat4::translate(Mat4::identity(), objPos);
	currentModel = currentModel * rotationMatrix * autoRot;

	return ;
}

void	ModelViewer::renderScene(void)
{
	shader->use();
	shader->setMat4("model", currentModel);
	shader->setMat4("view", currentView);
	shader->setMat4("projection", currentProjection);
	shader->setFloat("mixValue", textureToggle.value);
	shader->setFloat("lightingValue", lightingToggle.value);
	shader->setFloat("uvMode", uvToggle.value);

	texture->bind(0);
	shader->setInt("ourTexture", 0);

	glBindVertexArray(modelBuf.VAO);
	glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, modelBuf.vertexCount);

	if (showNormals && normalBuf.vertexCount > 0)
	{
		normalShader->use();
		normalShader->setMat4("model", currentModel);
		normalShader->setMat4("view", currentView);
		normalShader->setMat4("projection", currentProjection);
		normalShader->setVec3("lineColor", NORMAL_LINE_COLOR);

		glBindVertexArray(normalBuf.VAO);
		glDrawArrays(GL_LINES, 0, normalBuf.vertexCount);

		shader->use();
	}

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

	float	dx = (float)xpos - viewer->mouse.lastX;
	float	dy = (float)ypos - viewer->mouse.lastY;

	if (viewer->mouse.dragging)
	{
		Mat4	rotX = Mat4::rotate(Mat4::identity(), dy * MOUSE_SENSITIVITY, Vec3(1.0f, 0.0f, 0.0f));
		Mat4	rotY = Mat4::rotate(Mat4::identity(), dx * MOUSE_SENSITIVITY, Vec3(0.0f, 1.0f, 0.0f));

		viewer->rotationMatrix = rotX * rotY * viewer->rotationMatrix;
	}
	else if (viewer->mouse.panning)
	{
		viewer->objPos.x += dx * MOUSE_SENSITIVITY;
		viewer->objPos.y -= dy * MOUSE_SENSITIVITY;
	}

	viewer->mouse.lastX = (float)xpos;
	viewer->mouse.lastY = (float)ypos;

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
			viewer->mouse.dragging = true;
			viewer->mouse.lastX = (float)x;
			viewer->mouse.lastY = (float)y;
		}
		else if (action == GLFW_RELEASE)
			viewer->mouse.dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			viewer->mouse.panning = true;
			viewer->mouse.lastX = (float)x;
			viewer->mouse.lastY = (float)y;
		}
		else if (action == GLFW_RELEASE)
			viewer->mouse.panning = false;
	}

	return ;
}

void ModelViewer::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return ;

    (void)xoffset;
    ModelViewer *viewer = (ModelViewer *)glfwGetWindowUserPointer(window);

    viewer->zoom -= (float)yoffset * SCROLL_SENSITIVITY;

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

int	ModelViewer::findFileIndex(std::vector<std::string> const &files, std::string const &filename)
{
	for (int i = 0; i < (int)files.size(); ++i)
	{
		if (files[i] == filename)
			return (i);
	}

	return (0);
}

void	ModelViewer::loadModel(const std::string &filename)
{
	std::string	path = "resources/" + filename;

	std::cout << "Loading model: " << filename << "..." << std::endl;

	deleteBuffers(modelBuf);
	deleteBuffers(normalBuf);

	ObjParser	parser(path);
	setBuffers(parser);
	setNormalBuffers(parser);

	rotationMatrix = Mat4::identity();
	rotationAngle = 0.0f;
	objPos = Vec3(0.0f, 0.0f, 0.0f);
	zoom = DEFAULT_ZOOM;

	modelPositionCount = parser.getPositionCount();
	modelFaceCount = parser.getFaceCount();

	std::cout << "Model loaded" << std::endl;
	
	return ;
}
