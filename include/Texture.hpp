#ifndef TEXTURE_HPP
# define TEXTURE_HPP

#include <iostream>
#include <string>

#include "glad/glad.h"
#include "stb_image/stb_image.h"

class Texture
{

public:
	unsigned int	ID;
	std::string		type;
	std::string		path;

	Texture(const std::string &path, bool generateMipmap = true);
	~Texture();

	void	bind(unsigned int unit = 0) const;
	void	unbind(void) const;	

private:
	void	load(const std::string &path, bool generateMipmap);

};

#endif
