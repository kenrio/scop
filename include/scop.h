#ifndef SCOP_H
# define SCOP_H

// Standard libraries
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// OpenGL
#include <GL/gl.h>
#include <GL/glu.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "scop"

typedef struct	s_gl_context
{
	GLFWwindow	*window;

	GLuint	shader_program;
	GLuint	vao;
	GLuint	vbo;

	float	rotation_angle;
}		t_gl_context;

GLuint	load_shader(const char *vertex_path, const char * fragment_path);
GLuint	compile_shader(GLenum type, const char *source);

#endif
