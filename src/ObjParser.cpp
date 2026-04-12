#include "ObjParser.hpp"

ObjParser::ObjParser(std::string const &filepath)
{
	parse(filepath);
}

std::vector<float> const &	ObjParser::getVertices(void) const
{
	return (vertices);
}

std::vector<unsigned int> const &	ObjParser::getIndices(void) const
{
	return (indices);
}

Vec3	ObjParser::getCenter(void) const
{
	Vec3	center;

	for (size_t i = 0; i < positions.size(); ++i)
		center = center + positions[i];

	float	n = static_cast<float>(positions.size());

	return (Vec3(center.x / n, center.y / n, center.z / n));
}

void	ObjParser::parse(std::string const &filepath)
{
	std::ifstream	file(filepath);

	if (!file.is_open())
	{
		std::cerr << "Failed to open: " << filepath << std::endl;
		return ;
	}

	std::string	line;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue ;

		std::istringstream	iss(line);
		std::string	prefix;

		iss >> prefix;
		if (prefix == "v")
		{
			float	x, y, z;

			iss >> x >> y >> z;
			positions.push_back(Vec3(x, y, z));
		}
		else if (prefix == "f")
			parseFace(line);
	}
	file.close();

	center = getCenter();

	for (size_t i = 0; i < positions.size(); ++i)
		positions[i] = positions[i] - center;

	for (size_t i = 0; i < positions.size(); ++i)
	{
		vertices.push_back(positions[i].x);
		vertices.push_back(positions[i].y);
		vertices.push_back(positions[i].z);
	}

	return ;
}

void	ObjParser::parseFace(std::string const &line)
{
	std::istringstream	iss(line);
	std::string			prefix;

	iss >> prefix;

	std::vector<unsigned int>	faceIndices;
	unsigned int				index;

	while (iss >> index)
		faceIndices.push_back(index - 1);

	for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
	{
		indices.push_back(faceIndices[0]);
		indices.push_back(faceIndices[i]);
		indices.push_back(faceIndices[i + 1]);
	}

	return ;
}
