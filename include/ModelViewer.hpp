#ifndef MODELVIEWER_HPP
# define MODELVIEWER_HPP

#include <string>

#include "ObjParser.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "KeyInputHandler.hpp"

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

	Shader *		shader;
	Texture *		texture;
	KeyInputHandler *	keyInput;

	Vec3				objPos;
	float				mixValue;
	bool				textureMode;
	bool				tKeyPressed;
	int					vertexCount;

	void	initWindow(void);
	void	initGL(void);
	void	setBuffers(const ObjParser &parser);

	void	processInput(void);
	void	render(void);

	static void	framebufferSizeCallback(GLFWwindow * window, int w, int h);

};

#endif
