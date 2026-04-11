#ifndef SHADER_HPP
# define SHADER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <glad/glad.h>

#include "Math.hpp"

class Shader
{

public:
	unsigned int	ID;

	Shader(std::string vertexPath, std::string fragmentPath);
	~Shader();

	void	use(void);

	void	setBool(const std::string &name, bool value) const;
	void	setInt(const std::string &name, int value) const;
	void	setFloat(const std::string &name, float value) const;

	void	setMat4(const std::string &name, const Mat4 &data) const;

private:
	std::string		readFile(const std::string &filePath);
	unsigned int	createShader(const std::string &shaderCode, GLenum type);
	void			checkShaderErrors(unsigned int shader, GLenum type);
	void			checkProgramErrors(unsigned int program);

};

# endif
