#ifndef MODELVIEWER_HPP
# define MODELVIEWER_HPP

#include <string>
#include <filesystem>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ObjParser.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "KeyInputHandler.hpp"

float const			WINDOW_WIDTH = 1280.0f;
float const			WINDOW_HEIGHT = 1024.0f;
std::string const	WINDOW_TITLE = "scop";

Vec3 const		BACKGROUND_COLOR = {0.25f, 0.25f, 0.25f};

float const		FOV = 0.785f;
float const		NEAR_PLANE = 0.1f;
float const		FAR_PLANE = 3000.0f;

float const		ROTATION_SPEED = 0.01f;
float const		MOUSE_SENSITIVITY = 0.01f;
float const		SCROLL_SENSITIVITY = 0.01f;
float const		DEFAULT_ZOOM = 10.0f;
float const		ZOOM_MIN = 1.0f;
float const		ZOOM_MAX = 2000.0f;
float const		MOVE_SPEED = 0.02f;
float const		TRANSITION_SPEED = 0.01f;

int const		VERTEX_STRIDE = 13;

std::string const	DEFAULT_OBJ_PATH = "resources/42.obj";
std::string const	TEXTURE_PATH = "resources/snoutBeetle.bmp";
std::string const	VERTEX_SHADER = "shaders/vertex.glsl";
std::string const	FRAGMENT_SHADER = "shaders/fragment.glsl";

class	ModelViewer
{

public:

	ModelViewer(std::string const &objPath);
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
	
	int		modelPositionCount = 0;
	int		modelFaceCount = 0;
	
	Mat4	rotationMatrix = Mat4::identity();
	
	float	mouseLastX = 0.0f;
	float	mouseLastY = 0.0f;
	bool	mouseDragging = false;
	bool	mousePanning = false;
	
	float	zoom = DEFAULT_ZOOM;
	
	unsigned int	normalVAO = 0;
	unsigned int	normalVBO = 0;

	Shader *		normalShader = nullptr;

	int				normalVertexCount = 0;
	bool			showNormals = false;
	bool			nKeyPressed = false;
	
	unsigned int	axisVAO = 0;
	unsigned int	axisVBO = 0;

	Shader *		axisShader = nullptr;

	std::vector<std::string>	objFiles;
	std::vector<std::string>	bmpFiles;
	int							currentObjIndex = 0;
	int							currentBmpIndex = 0;

	void	initWindow(void);
	void	initGL(void);
	void	setBuffers(ObjParser const &parser);
	void	setNormalBuffers(ObjParser const &parser);
	void	setAxisBuffers(void);

	void	processInput(void);
	void	handleToggle(int key, bool &keyPressed, bool &mode);
	void	resetView(void);

	void	render(void);
	void	renderGUI(void);
	void	renderAxis(void);
	void	updateTransitions(void);
	void	smoothTransition(bool mode, float &value, float speed);
	void	renderScene(void);

	static void	framebufferSizeCallback(GLFWwindow * window, int w, int h);
	static void	mousePositionCallback(GLFWwindow * window, double xpos, double ypos);
	static void	mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	static void	scrollCallback(GLFWwindow * window, double xoffset, double yoffset);

	std::vector<std::string>	findFiles(std::string const &dir, std::string const &ext);
	void						loadModel(std::string const &filename);
};

#endif
