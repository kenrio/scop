#include "scop.h"

GLuint	compile_shader(GLenum type, const char * source)
{
	const char *shader_type_name = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
	GLuint	shader = glCreateShader(type);
	if (shader == 0)
	{
		std::cerr << "Failed to create " << shader_type_name << " shader" << std::endl;
		return (0);
	}
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[512];
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cerr << shader_type_name << " shader compilation error: \n" << info_log << std::endl;
		std::cerr << "Shader source:\n" << source << std::endl;
		return (0);
	}

	std::cout << shader_type_name << " shader compiled successfully (ID: " << shader << ")" << std::endl;
	return (shader);
}

GLuint	load_shader(const char *vertex_path, const char *fragment_path)
{
	std::ifstream	vertex_file(vertex_path);
	std::ifstream	fragment_file(fragment_path);

	if (!vertex_file.is_open())
	{
		std::cerr << "Failed to open vertex shader: " << vertex_path << std::endl;
		return (0);
	}

	if (!fragment_file.is_open())
	{
		std::cerr << "Failed to open fragment shader: " << fragment_path << std::endl;
		return (0);
	}

	std::cout << "Shader files opened successfully" << std::endl;

	std::stringstream	vertex_stream, fragment_stream;
	vertex_stream << vertex_file.rdbuf();
	fragment_stream << fragment_file.rdbuf();

	std::string	vertex_code = vertex_stream.str();
	std::string	fragment_code = fragment_stream.str();

	std::cout << "Vertex shader (" << vertex_code.length() << " bytes)" << std::endl;
	std::cout << "Fragment shader (" << fragment_code.length() << " bytes)" << std::endl;

	GLuint	vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_code.c_str());
	GLuint	fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_code.c_str());

	if (vertex_shader == 0 || fragment_shader == 0)
	{
		std::cerr << "Shader compilation failed" << std::endl;
		return (0);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint	success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char	info_log[512];
		glGetProgramInfoLog(program, 512, NULL, info_log);
		std::cerr << "Shader linking error:\n" << info_log << std::endl;
		return (0);
	}

	std::cout << "Shader program linked successfully" << std::endl;

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return (program);
}

