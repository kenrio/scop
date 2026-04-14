#include "Texture.hpp"

Texture::Texture(const std::string &path, bool generateMipmap)
: path(path)
{
	load(path, generateMipmap);
}

Texture::~Texture() {}

void	Texture::bind(unsigned int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
	return ;
}

void	Texture::unbind(void) const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	return ;
}

void	Texture::load(const std::string &path, bool generateMipmap)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					generateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int	width, height, nrChannels;
	unsigned char	*data;

	data = loadBMP(path, width, height, nrChannels);

	if (data)
	{
		GLenum	format = 0;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		if (generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cerr << "Failed to load texture: " << path << std::endl;

	delete[] data;

	return ;
}

unsigned char *	Texture::loadBMP(const std::string &path, int &width, int &height, int &channels)
{
	std::ifstream	file(path, std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "Failed to open BMP: " << path << std::endl;
		return (NULL);
	}

	unsigned char	header[54];
	file.read(reinterpret_cast<char *>(header), 54);

	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cerr << "Not a BMP file: " << path << std::endl;
		return (NULL);
	}

	int	dataOffset = *(int *)&header[10];
	width = *(int *)&header[18];
	height = *(int *)&header[22];
	bool	topDown = (height < 0);
	if (height < 0)
		height = -height;
	int	bitsPerPx = *(short *)&header[28];

	channels = bitsPerPx / 8;
	
	file.seekg(dataOffset);
	int				dataSize = width * 	height * channels;
	unsigned char	*data = new unsigned char[dataSize];
	file.read(reinterpret_cast<char *>(data), dataSize);
	file.close();
	
	std::cout << "BMP width: " << width << std::endl;
	std::cout << "BMP height: " << height << std::endl;
	std::cout << "BMP bitsPerPx: " << bitsPerPx << std::endl;
	std::cout << "BMP dataOffset: " << dataOffset << std::endl;
	std::cout << "BMP dataSize: " << dataSize << std::endl;

	for (int i = 0; i < dataSize; i += channels)
	{
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

	if (topDown)
	{
		int rowSize = width * channels;
		unsigned char *tmp = new unsigned char[rowSize];
		for (int y = 0; y < height / 2; y++)
		{
			int topRow = y * rowSize;
			int bottomRow = (height - 1 - y) * rowSize;
			std::memcpy(tmp, &data[topRow], rowSize);
			std::memcpy(&data[topRow], &data[bottomRow], rowSize);
			std::memcpy(&data[bottomRow], tmp, rowSize);
		}
		delete[] tmp;
	}

	return (data);
}
