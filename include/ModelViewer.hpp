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
const float		CAMERA_DIST = -5.0f;

const float		MOVE_SPEED = 0.02f;
const float		MIX_SPEED = 0.01f;

const int		VERTEX_STRIDE = 13;

const std::string	TEXTURE_PATH = "resources/snoutBeetle.bmp";
const std::string	VERTEX_SHADER = "shaders/vertex.glsl";
const std::string	FRAGMENT_SHADER = "shaders/fragment.glsl";

class	ModelViewer
{

public:

	ModelViewer(const std::string &objPath);
	~ModelViewer();

	void	run(void);

private:

	GLFWwindow *	window;

	unsigned int	VAO;
	unsigned int	VBO;

	Shader *			shader;
	Texture *			texture;
	KeyInputHandler *	keyInput;

	Vec3	objPos;

	float	mixValue;
	bool	textureMode;
	bool	tKeyPressed;

	float	lightingValue;
	bool	lightingMode;
	bool	lKeyPressed;

	float	uvModeValue;
	bool	uvMode;
	bool	uKeyPressed;

	float	rotationAngle;
	bool	rotating;
	bool	spaceKeyPressed;

	int		vertexCount;

	Mat4	rotationMatrix;

	float	mouseLastX;
	float	mouseLastY;
	bool	mouseDragging;

	bool	mousePanning;

	float	zoom;

	std::vector<std::string>	objFiles;
	std::vector<std::string>	bmpFiles;
	int							currentObjIndex;
	int							currentBmpIndex;

	void	initWindow(void);
	void	initGL(void);
	void	setBuffers(const ObjParser &parser);

	void	processInput(void);
	void	handleToggle(int key, bool &keyPressed, bool &mode);

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
