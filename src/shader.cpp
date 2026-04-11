#include "Shader.hpp"

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{

	std::string	vShaderCode = readFile(vertexPath);
	std::string	fShaderCode = readFile(fragmentPath);

	unsigned int vertex = createShader(vShaderCode, GL_VERTEX_SHADER);
	unsigned int fragment = createShader(fShaderCode, GL_FRAGMENT_SHADER);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkProgramErrors(ID);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader() {}

void	Shader::use(void)
{
	glUseProgram(ID);
	return ;
}

void	Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	return ;
}

void	Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	return ;
}

void	Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	return ;
}

void	Shader::setMat4(const std::string &name, const Mat4 &matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matrix.value_ptr());
	return ;
}

std::string	Shader::readFile(const std::string &filePath)
{
	std::string		shaderCode;
	std::ifstream	shaderFile;

	shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		shaderFile.open(filePath);
		std::stringstream	shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure &e)
	{
		std::cerr	<< "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << "\n"
					<< e.what() << "\n -- --------------------------------------------------- -- "
					<< std::endl;		
	}

	return (shaderCode);
}

unsigned int	Shader::createShader(const std::string &shaderCode, GLenum type)
{
	unsigned int	shader;
	const char		*sourceCode;

	shader = glCreateShader(type);
	sourceCode = shaderCode.c_str();
	glShaderSource(shader, 1, &sourceCode, NULL);
	glCompileShader(shader);
	checkShaderErrors(shader, type);

	return (shader);
}

void	Shader::checkShaderErrors(unsigned int shader, GLenum type)
{
	int			success;
	char		infoLog[1024];
	std::string	typeStr;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		if (type == GL_VERTEX_SHADER)
			typeStr = "VERTEX";
		else
			typeStr = "FRAGMENT";

		std::cerr	<< "ERROR::SHADER::" << typeStr << "::COMPILATION_FAILED\n"
					<< infoLog << "\n -- --------------------------------------------------- -- "
					<< std::endl;
	}
	return ;
}

void	Shader::checkProgramErrors(unsigned int program)
{
	int		success;
	char	infoLog[1024];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 1024, NULL, infoLog);
		std::cerr	<< "ERROR::PROGRAM::LINKIN_FAILED\n"
					<< infoLog << "\n -- --------------------------------------------------- -- "
					<< std::endl;
	}
	return ;
}
