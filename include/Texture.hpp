#ifndef TEXTURE_HPP
# define TEXTURE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "glad/glad.h"

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
	void	reload(const std::string &newPath, bool generateMipmap);

private:

	void			load(const std::string &path, bool generateMipmap);
	unsigned char *	loadBMP(const std::string &path, int &width, int &height, int &channels);

};

#endif
