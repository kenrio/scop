#ifndef MODELVIEWER_HPP
# define MODELVIEWER_HPP

#include <string>
#include <filesystem>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ObjParser.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "KeyInputHandler.hpp"

const float			WINDOW_WIDTH = 800.0f;
const float			WINDOW_HEIGHT = 600.0f;
const std::string	WINDOW_TITLE = "scop";

const float		FOV = 0.785f;
const float		NEAR_PLANE = 0.1f;
const float		FAR_PLANE = 3000.0f;

const float		ROTATION_SPEED = 0.01f;
const float		MOUSE_SENSITIVITY = 0.01f;
const float		SCROLL_SENSITIVITY = 0.01f;
const float		DEFAULT_ZOOM = 5.0f;
const float		ZOOM_MIN = 1.0f;
const float		ZOOM_MAX = 2000.0f;
const float		MOVE_SPEED = 0.02f;
const float		TRANSITION_SPEED = 0.01f;

const int		VERTEX_STRIDE = 13;

const std::string	DEFAULT_OBJ_PATH = "resources/42.obj";
const std::string	TEXTURE_PATH = "resources/snoutBeetle.bmp";
const std::string	VERTEX_SHADER = "shaders/vertex.glsl";
const std::string	FRAGMENT_SHADER = "shaders/fragment.glsl";

class	ModelViewer
{

public:

	ModelViewer(const std::string &objPath);
	~ModelViewer();

	void	run(void);
	bool	isValid(void) const;

private:

	bool			valid;

	GLFWwindow *	window;

	unsigned int	VAO = 0;
	unsigned int	VBO = 0;

	Texture *			texture = nullptr;
	Shader *			shader = nullptr;
	KeyInputHandler *	keyInput = nullptr;

	Shader *			normalShader = nullptr;

	Vec3	objPos;

	float	mixValue = 0.0f;
	bool	textureMode = false;
	bool	tKeyPressed = false;

	float	lightingValue = 0.0f;
	bool	lightingMode = false;
	bool	lKeyPressed = false;

	float	uvModeValue = 0.0f;
	bool	uvMode = false;
	bool	uKeyPressed = false;

	float	rotationAngle = 0.0f;
	bool	rotating = true;
	bool	spaceKeyPressed = false;

	bool	wireframe = false;
	bool	fKeyPressed = false;

	int		vertexCount;

	Mat4	rotationMatrix = Mat4::identity();

	float	mouseLastX = 0.0f;
	float	mouseLastY = 0.0f;
	bool	mouseDragging = false;
	bool	mousePanning = false;

	float	zoom = DEFAULT_ZOOM;

	unsigned int	normalVAO = 0;
	unsigned int	normalVBO = 0;
	int				normalVertexCount = 0;
	bool			showNormals = false;
	bool			nKeyPressed = false;

	std::vector<std::string>	objFiles;
	std::vector<std::string>	bmpFiles;
	int							currentObjIndex = 0;
	int							currentBmpIndex = 0;

	void	initWindow(void);
	void	initGL(void);
	void	setBuffers(const ObjParser &parser);
	void	setNormalBuffers(const ObjParser &parser);

	void	processInput(void);
	void	handleToggle(int key, bool &keyPressed, bool &mode);
	void	resetView(void);

	void	render(void);
	void	renderGUI(void);
	void	updateTransitions(void);
	void	smoothTransition(bool mode, float &value, float speed);
	void	renderScene(void);

	static void	framebufferSizeCallback(GLFWwindow * window, int w, int h);
	static void	mousePositionCallback(GLFWwindow * window, double xpos, double ypos);
	static void	mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	static void	scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

	std::vector<std::string>	findFiles(const std::string &dir, const std::string &ext);
	void						loadModel(const std::string &filename);
};

#endif
